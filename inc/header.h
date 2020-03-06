#ifndef _HEADER_H
#define _HEADER_H

#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <malloc/malloc.h>
#include <termios.h>
#include <stdbool.h>
#include <curses.h>
#include <term.h>
typedef struct s_queue t_queue;
typedef struct s_path t_path;
typedef struct s_reddir t_reddir;
typedef struct s_list t_list;

struct s_list {
    char *command;
    struct s_list *next;
};

struct s_queue {
    char *command;
    char op;
    struct s_queue *next;
};

struct s_path {
    char *file;
    char op;
    struct s_path *next;
};

struct s_reddir {
    char op;
    char *task;
    t_path *input;
    t_path *output;
};

typedef struct s_history_name {
    unsigned char *name;
    int n_byte;
    int n_len;
    struct s_history_name *next;
    struct s_history_name *previous;
}               t_history_name;

typedef struct s_len_name {
    unsigned char ch[4];
    int n_bute;
    int n_len;
    int n_cursor;
    int cur_pos_x;
    int n_cursor_b;
    int col;
    int cursor_delta_len;
    int cursor_delta_b;
    bool first_line;
    int first_line_len;
    int first_line_byte;
    bool trig;
    bool trig_copy;
    unsigned char *buf;
}              t_len_name;

typedef struct s_pid_name {
    int pid; 
    struct s_pid_name *next;
}               t_pid_name;



//int main(void);
int mx_count_words(const char *str, char c);
char *mx_strnew(const int size);
char **mx_strsplit(const char *s, char c);
int mx_strlen(const char *s);
char *mx_read_env(char *file);
void mx_del_strarr(char ***arr);

char *mx_strjoin_two( char *s1,  char *s2);
char *mx_strjoin( char *s1,  char *s2);
char *mx_strcat(char *restrict s1, const char *restrict s2);

void mx_strdel(char **str);
int mx_redirection(char *command, t_pid_name **pid_ar);
int mx_pipe_rec(t_reddir *command, int pos, int in_fd, bool extInput);
int mx_ush_execute(char *argv, t_pid_name **pid_ar);

char *mx_itoa(int number);

void mx_fd_change(t_reddir *command, int pos, int in_fd, bool extInput);
void mx_command_cut(char *command, int s, int f, t_reddir *tasks);
char *mx_file_to_str(const char *file);
void mx_printstr(const char *s);
t_queue **mx_works_queue(char *line);
char **mx_tokens(char *line, char sp);
int mx_strcmp(const char *s1, unsigned const char *s2);
void mx_logicOp(char *line, t_queue **list);
char *mx_substitute(char *command, t_pid_name **pid_ar);
void mx_push_back_history(t_history_name **history, unsigned char *str,
                          t_len_name *len);
void mx_delete_history(t_history_name **history);
t_len_name *mx_creat_len();
void mx_main_cycle_key(t_history_name **history, unsigned char **mystr, 
                        t_len_name *len, char *buf_first);
void mx_get_width(int *col);
void mx_one_cycle(unsigned char **mystr, t_len_name *len, unsigned char *buf);
void mx_two_cycle(unsigned char **mystr, t_len_name *len);
void mx_three_cycle(unsigned char **mystr, t_len_name *len);
void mx_four_cycle(unsigned char **mystr, t_len_name *len, char **buf_first,
                   t_history_name **his);
void mx_byte_check_add(unsigned char *ch, int *delte_len, int *delta_byte,
                       int start);
void mx_byte_check_back(unsigned char *str, int *len, int *minus, int n);
void mx_arrow_left(unsigned char **mystr, t_len_name *len);
void mx_arrow_right(unsigned char **mystr, t_len_name *len);
void mx_arrow_up(unsigned char **mystr, t_len_name *len, char **buf_first,
                t_history_name **his);
void mx_arrow_down(unsigned char **mystr, t_len_name *len, char **buf_first,
                t_history_name **his);
void mx_add_to_str(unsigned char **str, t_len_name **le);
void mx_back_to_str(unsigned char **str,  t_len_name **le);
void mx_delete_pid(t_pid_name **pid_list);
void mx_push_back_pid(t_pid_name **pid_list,int pid);

#endif
