#include "header.h"

static char *operant_cut(char *input, int f, char c) {
    char *buf = NULL;
    int j = 0;
    int i = f;
    int s = 0;

    for (; input[i] == c; i--);
    s = i;
    buf = (char *)malloc(sizeof(char) * (f - s));
    for (; i <= f; i++, j++) {
        buf[j] = input[i];
    }
    buf[j] = '\0';
    return buf;
}

static char *command_cut(char *input, int s, int f) {
    char *buf = (char *)malloc(sizeof(char) * (f - s));
    int j = 0;

    for (int i = s + 1; i <= f; i++, j++) {
        buf[j] = input[i];
    }
    buf[j] = '\0';
    return buf;
}

t_tree *mx_parcing(char *input) {
    t_tree *work = (t_tree *)malloc(sizeof(t_tree));
    t_tree *p = NULL;
    t_tree *parent = NULL;
    int trig = 1;
    int size = strlen(input);

    (*work).parent = parent;
    for (int i = strlen(input) - 2; i >= 0; i--) {
        if (input[i] == '"') {
            if (trig == 1)
                trig = 0;
            else if (trig == 0)
                trig = 1;
        }
        if (input[i] == '|' && input[i - 1] == '|' && trig == 1) {
            parent = work;
            (*work).operant = operant_cut(input, i, '|');
            (*work).command = NULL;
            (*work).left_child = (t_tree *)malloc(sizeof(t_tree));
            p = (*work).left_child;
            (*p).parent = parent;
            (*p).operant = NULL;
            (*p).command = command_cut(input, i, size);
            (*work).right_child = (t_tree *)malloc(sizeof(t_tree));
            (*(*work).right_child).parent = parent;
            work = (*work).right_child;
            for (; input[i] == '|'; i--);
            size = i;
        }
        else if (input[i] == '&' && input[i - 1] == '&' && trig == 1) {
            parent = work;
            (*work).operant = operant_cut(input, i, '&');
            (*work).command = NULL;
            (*work).left_child = (t_tree *)malloc(sizeof(t_tree));
            p = (*work).left_child;
            (*p).parent = parent;
            (*p).operant = NULL;
            (*p).command = command_cut(input, i, size);
            (*work).right_child = (t_tree *)malloc(sizeof(t_tree));
            (*(*work).right_child).parent = parent;
            work = (*work).right_child;
            for (; input[i] == '&'; i--);
            size = i;
        }
        // else if(input[i] == '|' && input[i] != '|' && trig == 1) {
        //     parent = work;

        // }
    }
    (*work).operant = NULL;
    (*work).command = command_cut(input, -1, size);
    (*work).parent = parent;
    (*work).left_child = NULL;
    (*work).right_child = NULL;
    return work;
    // out(work);
}
