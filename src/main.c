 #include "header.h"



// char *mx_read_line() {
//     char c;
//     int bufsize = 1024;
//     int position = 0;
//     char *buffer = (char *)malloc(sizeof(char) * bufsize);

//     while(1) {
//         c = getchar();
//         if (c == '\0' || c == '\n') {
//             buffer[position] = '\0';
//             return buffer;
//         }
//         else {
//             buffer[position] = c;
//         }
//         position++;
//     }
// }

void mx_get_twidth(int *col) {
    char *termtype = getenv("TERM");
    char *buff = malloc(2048);
    bool if_term = 0;

    if (termtype == 0) {
        termtype = strdup("xterm-256color");
        if_term = 1;
    }
    if (tgetent (buff, termtype) == 0) {
        perror("ush: ");
        exit(1);
    }
    free(buff);
    if (if_term)
        mx_strdel(&termtype);
    *col = tgetnum("co");
}

void mx_byte_check_add(unsigned char *ch, int *delte_len, int *delta_byte,
                       int start) {
    if (ch[start] >> 7 == 0)
        *delta_byte = 1;
    if (ch[start] >> 5 == 6 && ch[start + 1] >> 6 == 2 )
        *delta_byte = 2;
    if (ch[start] >> 4 == 14 && ch[start + 1] >> 6 == 2
        && ch[start + 2] >> 6 == 2 )
        *delta_byte = 3;
    if (ch[start] >> 3 == 30 && ch[start + 1] >> 6 == 2 
        && ch[start + 2] >> 6 == 2 && ch[start + 3] >> 6 == 2) {
        *delta_byte = 4;
        *delte_len = 2;
    }
}

void mx_byte_check_back(unsigned char *str, int *len, int *minus, int n) {
    if ( str[n-2] >> 7 == 0)
        *minus = 1;
    if (str[n-3] >> 5 == 6 && str[n-2] >> 6 == 2 )
        *minus = 2;
    if (str[n-4] >> 4 == 14 && str[n-3] >> 6 == 2 && str[n-2] >> 6 == 2 )
        *minus = 3;
    if (str[n-5] >> 3 == 30 && str[n-4] >> 6 == 2 && str[n-3] >> 6 == 2
        && str[n-2] >> 6 == 2) {
        *minus = 4;
        *len = 2;
    }
}



void add_to_str(unsigned char **str, unsigned char *ch, int *n, int *n_len, int *n_cursor, int *n_cursor_b) {
    int plus = 0;
    int len = 1;
    int buf_plus = 0;
    unsigned char *buf_str = NULL;

    mx_byte_check_add(ch, &len, &plus, 0);
    if (ch[0] >> 7 == 0 && ch[1] >> 7 == 0 && ch[2] >> 7 == 0 && ch[3] >> 7 == 0 && ch[1] != 0 && ch[2] != 0 && ch[3] != 0) {
        plus = 4;
        len = 4;
}

    if ((*n_cursor_b) == (*n)) {
        (*n) = (*n) + plus;
        (*n_cursor_b) = (*n_cursor_b) + plus;
        (*n_len) = (*n_len) + len;
        (*n_cursor) = (*n_cursor) + len;
        (*str) = (unsigned char *)realloc(*str, (*n));
        (*str)[(*n)-1] = '\0';
        buf_plus = plus;
        for (int i = 0; i < plus; i++) {
            (*str)[(*n)-1-buf_plus] = ch[i];
            buf_plus--;
        }
    }
    else {
        buf_str = (unsigned char *)malloc(sizeof(char) * ((*n) + plus));
        for (int i = 0; i < (*n_cursor_b) - 1; i++)
            buf_str[i] = (*str)[i];
        for (int i = 0; i < plus; i++)
            buf_str[(*n_cursor_b) + i - 1] = ch[i];
        for (int i = (*n_cursor_b) + plus - 1; i <= (*n); i++)
            buf_str[i] = (*str)[i - plus];
        free(*str);
        *str = buf_str;
         (*n) = (*n) + plus;
         (*n_cursor_b) = (*n_cursor_b) + plus;
         (*n_cursor) = (*n_cursor) + len;
         (*n_len) = (*n_len) + len;
    }

}

void back_to_str(unsigned char **str, int *n, int *n_len, int *n_cursor, int *n_cursor_b) {
    int minus = 0;
    int len = 1;
    unsigned char *buf_str = NULL;
    int q = 0;

    mx_byte_check_back(*str, &len, &minus, *n);
    if ((*n_cursor_b) == (*n)) {
        if ((*n) > 1) {
            for (int i = 0; i < minus; i++) {
            (*str)[(*n) - 2 - i] = '\0';
        }
        (*n) = (*n) - minus;
        (*str) = (unsigned char *)realloc(*str, (*n));
        (*n_len) = (*n_len) - len;
        (*n_cursor) = (*n_cursor) - len;
        (*n_cursor_b) = (*n_cursor_b) - minus;
        }
    }
    else {
        if ((*n_cursor_b) > 1) {
            buf_str = (unsigned char *)malloc(sizeof(char) * ((*n) - minus));
            for (int i = 0; i < (*n_cursor_b) - minus - 1; i++)
                buf_str[i] = (*str)[i];
            q = (*n_cursor_b) - minus - 1;
            for (int i = (*n_cursor_b) - 1; i < (*n); i++) {
                buf_str[q] = (*str)[i];
                q++;
            }
            free(*str);
            *str = buf_str;
            (*n_len) = (*n_len) - len;
            (*n_cursor) = (*n_cursor) - len;
            (*n_cursor_b) = (*n_cursor_b) - minus;
            (*n) = (*n) - minus;
        }
        
    }
}


unsigned char *mx_read_line(bool *trig, t_history_name **history) {
    struct termios savetty;
    struct termios tty;
    unsigned char ch[4];
    unsigned char *mystr = (unsigned char *)malloc(sizeof(char) * 1);

    int n_bute = 1;
    int n_len = 0;
    int n_cursor = 0;
    int cur_pos_x = 0;
    int n_cursor_b = 1;
    int col = 0;
    int cursor_delta_len = 1;
    int cursor_delta_b = 0;

    bool first_line = true;
    char *buf_first = NULL;
    int first_line_len = 0;
    int first_line_byte = 1;






    mystr[0] = '\0';
    

    tcgetattr (0, &tty);
    savetty = tty; /* Сохранить упр. информацию канонического режима */
    tty.c_lflag &= ~(ICANON | ECHO | ISIG);
    tty.c_cc[VMIN] = 1;
    tcsetattr (0, TCSAFLUSH, &tty);

    
    

    mx_get_twidth(&col);
    mx_printstr("u$h> ");
    

    while (1) {
        read (0, &ch, 4);
        //   fprintf(stdout, "%d--%d--%d--%d\n", ch[0], ch[1], ch[2] ,ch[3]);
        // fflush(stdout);
     
      

        // if (ch[0] == 27 && ch[1] == 91 && ch[2] == 66) // errow down
        // if (ch[0] == 27 && ch[1] == 91 && ch[2] == 65) // errow up
        // if (ch[0] == 27 && ch[1] == 91 && ch[2] == 68) // errow left
        // if (ch[0] == 27 && ch[1] == 91 && ch[2] == 67) // errow right
        //if (ch[0] == 9) //tab
    


        mx_get_twidth(&col);
        if (ch[0] != 27 ) {
        if (n_cursor + 5  > col) {
            fprintf(stdout, "\033[%dF", (n_cursor + 4)/col);
            fflush(stdout);
        }
        else {
            mx_printstr("\033[1G");
        }
        mx_printstr("\033[0J");
        mx_printstr("u$h> ");

        if (ch[0] == 127 && ch[1] == 0 && ch[2] == 0 && ch[3] == 0) {
            back_to_str(&mystr, &n_bute, &n_len, &n_cursor, &n_cursor_b);
        }
       else if (ch[0] == '\n' || ch[0] == '\0') {
           if (ch[0] != '\n') 
                *trig = true;
            fprintf(stdout, "%s", mystr);
            fflush(stdout);
            write(1, "\n", 1);
            if (mx_strcmp("exit", mystr) == 0) {
                if (malloc_size(buf_first))
                    free(buf_first);
                free(mystr);
                exit(0);
            }
                
            break;
        }
        else if (ch[0] != 27) {
            add_to_str(&mystr, ch, &n_bute, &n_len, &n_cursor, &n_cursor_b);
        }
        //fprintf(stdout, "%s", mystr);
        //fprintf(stdout, "\033[%dE", (n_cursor + 5)/col);
        //fflush(stdout);
        fprintf(stdout, "%s", mystr);
        fflush(stdout);
        // cur_pos_x = (((n_cursor + 3)/col + 1) * col) - n_cursor;
        cur_pos_x = col - ((((n_cursor + 4)/col + 1) * col) - (n_cursor + 5)) + 1;
        

         if ((((n_len + 4)/col - (n_cursor + 4)/col)) > 0) {
            fprintf(stdout, "\033[%dF", ((n_len + 4)/col - (n_cursor + 4)/col));
            fprintf(stdout, "\033[%dG", cur_pos_x);
            fflush(stdout);
        }
        else {
            fprintf(stdout, "\033[%dG", cur_pos_x);
            fflush(stdout);
        }
        }
        cur_pos_x = col - ((((n_cursor + 5)/col + 1) * col) - (n_cursor + 5)) + 1;

        
         if (ch[0] == 27 && ch[1] == 91 && ch[2] == 68) {// errow left
            if (n_cursor > 0) {
                mx_byte_check_back(mystr, &cursor_delta_len, &cursor_delta_b, n_cursor_b);
                cur_pos_x = cur_pos_x - cursor_delta_len;
                n_cursor = n_cursor - cursor_delta_len;
                n_cursor_b = n_cursor_b - cursor_delta_b;
                cursor_delta_len = 1;
                if (cur_pos_x < 1) {
                    fprintf(stdout, "\033[%dF",1);
                    fprintf(stdout, "\033[%dG", col);
                    fflush(stdout);
                    cur_pos_x = col;
                }
                else { 
                    fprintf(stdout, "\033[%dG", cur_pos_x);
                    fflush(stdout);
                }
            }
         }
        
        if (ch[0] == 27 && ch[1] == 91 && ch[2] == 67) {// errow right
            if (n_cursor < n_len) {
                mx_byte_check_add(mystr, &cursor_delta_len, &cursor_delta_b, n_cursor_b);
                cur_pos_x = cur_pos_x + cursor_delta_len;
                n_cursor = n_cursor + cursor_delta_len;
                n_cursor_b = n_cursor_b + cursor_delta_b;
                cursor_delta_len = 1;
                if (cur_pos_x > col) {
                    fprintf(stdout, "\033[%dE",1);
                    fprintf(stdout, "\033[%dG", 1);
                    fflush(stdout);
                    cur_pos_x = 1;
                }
                else {
                    fprintf(stdout, "\033[%dG", cur_pos_x);
                    fflush(stdout);
                }
            }
         }
         if (ch[0] == 27 && ch[1] == 91 && ch[2] == 65) {// errow up
            if (*history != NULL) {
                if (first_line == true) {
                    first_line_len = n_len;
                    first_line_byte = n_bute;
                    buf_first = strdup((char *) mystr);
                    free(mystr);
                    first_line = false;
                }
                if (n_cursor + 5 > col) {
                    fprintf(stdout, "\033[%dF", (n_cursor + 4)/col);
                    fflush(stdout);
                }
                else {
                    mx_printstr("\033[1G");
                    }
                mx_printstr("\033[0J");
                mx_printstr("u$h> ");
                n_cursor =(*history)->n_len;
                n_cursor_b = (*history)->n_byte;
                n_len = n_cursor;
                n_bute = n_cursor_b;
                mystr = (*history)->name;
                fprintf(stdout, "%s", (*history)->name);
                fflush(stdout);
                if ((*history)->next)
                    (*history) = (*history)->next;
           }
         }
        if (ch[0] == 27 && ch[1] == 91 && ch[2] == 66) { // errow down
                if ((*history)->previous != NULL || first_line == false) {
                    
                if (n_cursor + 5 > col) {
                    fprintf(stdout, "\033[%dF", (n_cursor + 4)/col);
                    fflush(stdout);
                }
                else {
                    mx_printstr("\033[1G");
                    }
                mx_printstr("\033[0J");
                mx_printstr("u$h> ");
                if ((*history)->previous != NULL) {
                    (*history) = (*history)->previous;
                    n_cursor =(*history)->n_len;
                    n_cursor_b = (*history)->n_byte;
                    n_len = n_cursor;
                    n_bute = n_cursor_b;
                    mystr = (*history)->name;
                    fprintf(stdout, "%s", (*history)->name);
                    fflush(stdout);
                }
                else {
                    if (first_line == false) {
                        n_cursor = first_line_len;
                        n_cursor_b = first_line_byte;
                        n_len = first_line_len;
                        n_bute = n_cursor_b;
                        mystr = (unsigned char *) buf_first;
                        fprintf(stdout, "%s", mystr);
                        fflush(stdout);
                        first_line = true;
                        first_line_len = 0;
                        first_line_byte = 1;
                    }
                }
                //mx_printstr("check");
            }
        }
        

        ch[0] = 0;
        ch[1] = 0;
        ch[2] = 0;
        ch[3] = 0;
        // fprintf(stdout, "%d--%d--%d--%d--%d\n", mystr[0], mystr[1], mystr[2] ,mystr[3], mystr[4]);
        // fflush(stdout);
        // fprintf(stdout, "%d--%d*", n_len, n_bute);
        // fflush(stdout);
    }
    // fprintf(stdout, "%d--%d--%d", (n_len+4)/col, n_len, n_bute);
    //         fflush(stdout);
    //fprintf(stdout, "%d", ((n_len + 3)/col - ((n_cursor + 3)/col)));
    //fprintf(stdout, "%d", ((n_len + 3)/col - ((n_cursor + 3)/col)));
    //fprintf(stdout, "%d--%d", n_cursor_b, n_bute);
    // fprintf(stdout, "n_len = %d, n_cursor = %d, mylen = %d, ", n_len, n_cursor, (n_len - n_cursor)/col);
    // fprintf(stdout, "mylen2 = %d", (n_len + 5)/col - (n_cursor + 5)/col);
    //fflush(stdout);
    if (first_line != true)
        free(buf_first);

    mx_push_back_history(history, mystr, n_bute, n_len);
    tcsetattr (0, TCSAFLUSH, &savetty);
    return mystr;
}


void lsh_loop(void) {
    unsigned char *line;
    int status = 2;
    t_tree **work = NULL;
    t_tree *p = NULL;
    pid_t pid = 1;
    bool trig = false;
    t_history_name *history = NULL;

    while (trig == false) {
        //mx_printstr("u$h> ");
        line = mx_read_line(&trig, &history);
        if (line[0] != '\0') {
        // fprintf(stdout, "%s", line);
        // fflush(stdout);
        work = mx_works_queue((char *)line);
        for (int i = 0; work[i]; i++) {
            p = work[i];
            for (; (*p).right_child; p = (*p).right_child);
            if ((*p).parent) {
                p = (*p).parent;
                status = mx_redirection((*(*p).right_child).command);
                // printf("\n\nSTATUS = %d\n\n\n", status);
                // if (status == 2)
                    // status = mx_ush_execute((*(*p).right_child).command);
                for (; p != NULL; p = (*p).parent) {
                    if ((*p).operant[1] == '|') {
                        if (status == 1) {
                            status = mx_redirection((*(*p).left_child).command);
                            // printf("\n\nSTATUS = %d\n\n\n", status);
                            // if (status == 2)
                                // status = mx_ush_execute((*(*p).left_child).command);
                        }
                    }
                    else if ((*p).operant[1] == '&') {
                        if (status == 0) {
                            status = mx_redirection((*(*p).left_child).command);
                            // printf("\n\nSTATUS = %d\n\n\n", status);
                            // if (status == 2)
                                // status = mx_ush_execute((*(*p).left_child).command);
                        }
                    }
                }
            }
            else
            {
                status = mx_redirection((*p).command);
                // printf("\n\nSTATUS = %d\n\n\n", status);
                // if (status == 2)
                    // status = mx_ush_execute((*p).command);
            }
        }
        //free(line);
    }
    pid = getpid();
     //free(line);
    }
}
void hdl(int sig)
{
    sig = sig +1 - 1;
    // printf("%d - pid\n", getpid());
    //printf("\n");
}

int main(void) {
    // struct sigaction act;
    // memset(&act, 0, sizeof(act));
    // act.sa_handler = hdl;
    // sigset_t set;
    // sigemptyset(&set);
    // act.sa_mask = set;
    // sigaddset(&set, SIGINT);
    // sigaddset(&set, SIGTSTP);
    

    // act.sa_mask = set;
    // sigaction(SIGINT, &act, 0);
    // sigaction(SIGTSTP, &act, 0);

    signal(SIGINT, hdl);
    signal(SIGTSTP, hdl);



    // signal(SIGTTIN, hdl);
    // signal(SIGTTOU, hdl);
    // signal(SIGQUIT, hdl);



    lsh_loop();
    // lsh_loop();
    // mx_ush_pipe_execute(); 
    
}


