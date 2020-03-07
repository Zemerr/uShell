 #include "header.h"

unsigned char *mx_read_line(bool *trig, t_history_name **history) {
    struct termios savetty;
    struct termios tty;
    t_len_name *len = mx_creat_len();
    unsigned char *mystr = (unsigned char *)malloc(sizeof(char) * 1);
    char *buf_first = NULL;

    mystr[0] = '\0';
    tcgetattr (0, &tty);
    savetty = tty;
    tty.c_lflag &= ~(ICANON | ECHO | ISIG);
    tty.c_cc[VMIN] = 1;
    tcsetattr (0, TCSAFLUSH, &tty);
    mx_get_width(&(len->col));
    mx_printstr("u$h> ");
    mx_main_cycle_key(history, &mystr, len, buf_first);
    *trig = len->trig;
    free(len);
    tcsetattr (0, TCSAFLUSH, &savetty);
    return mystr;
}

// static void printAlias(t_alias *list) {
    // t_alias *p = list;
// 
    // printf("ALIAS:\n");
    // for (; p; p = p->next) {
        // printf("NAME -> %s\n", (*p).name);
        // printf("MEANS -> %s\n", (*p).meaning);
    // }
    // printf("END FOR NOW\n");
// }

void ush_loop(void) {
    unsigned char *line;
    int status = 2;
    t_queue **work = NULL;
    t_queue *p = NULL;
    bool trig = false;
    t_history_name *history = NULL;
    t_alias *aliasList = NULL;

    while (trig == false) {
        // mx_printstr("u$h> ");
        line = mx_read_line(&trig, &history);
       
        if (line[0] != '\0') {
            work = mx_works_queue((char *)line);
            for (int i = 0; work[i]; i++) {
                p = work[i];
                for (; p; p = (*p).next) {
                    (*p).command = mx_parameter_exp((*p).command);
                    (*p).command = mx_substitute((*p).command, &aliasList);
                    status = mx_redirection((*p).command, &aliasList);
                    if (((*p).op == '&' && status == 1)
                        || ((*p).op == '|' && status == 0))
                        {
                            p = (*p).next;
                        }
                }
            }
            // printf("NAME -> %s\n", aliasList->name);
            // printAlias(aliasList);
        }
        free(line);
        //system("leaks -q ush");
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



    ush_loop();
    //system("leaks -q ush");
    // lsh_loop();
    // mx_ush_pipe_execute(); 
    
}


