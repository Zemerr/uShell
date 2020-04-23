#include "header.h"

static void muteChar(bool *sQ, bool *dQ, bool *a, char *l, int i) {
    if (l[i] == 96 && l[i - 1] != 92 && (*sQ) == false && (*dQ) == false) {
        if ((*a) == false)
            *a = true;
        else
            *a = false;
    }
    else if (l[i] == 34 && l[i - 1] != 92 && (*a) == false && (*sQ) == false) {
        if ((*dQ) == false)
            *dQ = true;
        else
            *dQ = false;
    }
    else if (l[i] == 39 && (*a) == false && (*dQ) == false) {
        if ((*sQ) == false)
            *sQ = true;
        else
            *sQ = false;
    }
}

static int worksCount(char *line) {
    int count = 1;
    bool sQ = false;
    bool dQ = false;
    bool iSs = false;

    for (int i = 0; line[i] != '\0'; i++) {
        muteChar(&sQ, &dQ, &iSs, line, i);
        if (line[i] == ';' && sQ == false && dQ == false && iSs == false)
            count += 1;
    }
    return count;
}

static bool onlySpaces(char *line, int st, int end) {
    for (int i = st; i < end; i++) {
        if (line[i] != ' ')
            return false;
    }
    return true;
}

static void quotesCheck(bool *ap, bool *sQ, bool *d, char *c, int i) {
    if (c[i] == 34 && c[i - 1] != 92 && (*ap) == false && (*sQ) == false) {
        if ((*d) == false)
            *d = true;
        else
            *d = false;
    }
    else if (c[i] == 39 && (*d) == false && (*ap) == false) {
        if ((*sQ) == false)
            *sQ = true;
        else
            *sQ = false;
    }
    else if (c[i] == 96 && c[i - 1] != 92 && (*sQ) == false && (*d) == false) {
        if ((*ap) == false)
            *ap = true;
        else
            *ap = false;
    }
}

static char *commandCut(char *line, int start, int end) {
    char *command = (char *)malloc(sizeof(char) * (end - start + 2));
    int q = 0;
    bool ap = false;
    bool iSsq = false;
    bool iSdq = false;

    for (int i = start; i < end; i++) {
        if (i == start && line[i] == ' ')
            for (; line[i] == ' '; i++);
        if (line[i] == 34 || line[i] == 39 || line[i] == 96) {
            quotesCheck(&ap, &iSsq, &iSdq, line, i);
        }
        if (line[i - 1] != 92 && line[i] == 92
            && (line[i + 1] == 34 || line[i + 1] == 39 || line[i + 1] == 96)
            && ap == false && iSsq == false && iSdq == false)
            {
                i++;
            }
            else if (line[i] == 92 && line[i + 1] == 92 && ap == false && iSsq == false && iSdq == false)
                for (int k = 0; line[i + 1] == 92 && k < 4; i++, k++);
            if (onlySpaces(line, i, end) == false) {
                command[q] = line[i];
                q++;
        }
    }
    command[q] = '\0';
    command = realloc(command, strlen(command));
    return command;
}

static void pushBack(t_list **jobs, char *line, int start, int end) {
    t_list **p = jobs;

    if ((*jobs) == NULL) {
        (*p) = (t_list *)malloc(sizeof(t_list));
        (*p)->command = commandCut(line, start, end);
        (*p)->next = NULL;
    }
    else {
        for (; (*p)->next; *p = (*p)->next);
        (*p)->next = (t_list *)malloc(sizeof(t_list));
        (*p)->next->command = commandCut(line, start, end);
        (*p)->next->next = NULL;
    }
}

static t_list *jobsSplit(char *line) {
    t_list *jobs = NULL;
    bool sQ = false;
    bool dQ = false;
    bool iSs = false;
    int start = 0;
    int i = 0;

    for (; line[i] != '\0'; i++) {
        muteChar(&sQ, &dQ, &iSs, line, i);
        if (sQ == false && dQ == false && iSs == false && line[i] == ';') {
            pushBack(&jobs, line, start, i);
            start = i + 1;
        }
    }
    pushBack(&jobs, line, start, i);
    return jobs;
}

void jobs_delete(t_list **jobs) {
    t_list *p1 = *jobs;
    t_list *p2 = *jobs;

    for (; p1->next; p1 = p1->next);
    while (p1 != *jobs) {
        mx_strdel(&p1->command);
        for (; p2->next != p1; p2 = p2->next);
        free(p1);
        p1 = p2;
        p2 = *jobs;
    }
    free(*jobs);
}

t_queue **mx_works_queue(char *line) {
    int size = worksCount(line);
    t_list *jobs = jobsSplit(line);
    t_queue **list = (t_queue **)malloc(sizeof(t_queue *) * (size + 1));
    int i = 0;

    for (t_list *p = jobs; p && i < size; p = (*p).next, i++) {
        list[i] = NULL;
        mx_logicOp((*p).command, &list[i]);
    }
    list[size] = NULL;
    jobs_delete(&jobs);
    // system("leaks -q ush");
    return list;
}
