#include "header.h"

static void redirect(int oldfd, int newfd) {
    if (oldfd != newfd) {
        dup2(oldfd, newfd);
        close(oldfd);
    }
}

int mx_pipe_rec(t_reddir *command, int pos, int in_fd) {
    int status = 0;
    char **task = NULL;
    char *path = NULL;
    pid_t pid;

    if (command[pos].op == '-') {
        if (command[pos].path->next)
            mx_fd_change(command, pos, in_fd);
        else {
            pid = fork();
            if (pid == 0) {
                redirect(in_fd, 0);
                task = mx_strsplit(command[pos].task, ' ');
                path = mx_read_env(task[0]);
                if (execvp(path, task) == -1)
                    perror("lsh");
            }
            else
            {
                waitpid(pid, &status, WUNTRACED);
            }
        }
    }
    else if (command[pos].op == '|'){
        if (command[pos].path->next)
            mx_fd_change(command, pos, in_fd);
        int fd[2];
        pipe(fd);
        pid = fork();
        if (pid == 0) {
            close(fd[0]);
            redirect(in_fd, 0);
            redirect(fd[1], 1); /* write to fd[1] */
            task = mx_strsplit(command[pos].task, ' ');
            path = mx_read_env(task[0]);
            if (execvp(path, task) == -1) {
                perror("ush");
            }
        }
        else {
            if (fd[1] != 1)
                close(fd[1]);
            if (in_fd != 0)
                close(in_fd); /* unused */
            mx_pipe_rec(command, pos + 1, fd[0]);
        }
    }
    return status;
}