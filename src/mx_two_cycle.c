 #include "header.h"

void mx_two_cycle(unsigned char **mystr, t_len_name *len, FILE *file) {
    if (len->ch[0] != '\n') 
        len->trig = true;
    fprintf(file, "%s\n", *mystr);
    fflush(file);
}

void mx_three_cycle(unsigned char **mystr, t_len_name *len, FILE *file) {
    fprintf(file, "%s", *mystr);
    fflush(file);   
    len->cur_pos_x = len->col - ((((len->n_cursor + 4)/len->col + 1) 
                     * len->col) - (len->n_cursor + 5)) + 1;
    if ((((len->n_len + 4)/len->col - (len->n_cursor + 4)/len->col)) > 0) {
        fprintf(file, "\033[%dF", 
                ((len->n_len + 4)/len->col - (len->n_cursor + 4)/len->col));
        fprintf(file, "\033[%dG", len->cur_pos_x);
        fflush(file);
    }
    else {
        fprintf(file, "\033[%dG", len->cur_pos_x);
        fflush(file);
    }
}

