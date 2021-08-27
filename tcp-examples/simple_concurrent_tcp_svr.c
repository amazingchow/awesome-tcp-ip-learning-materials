#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define PORT 18989

int
msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

void
sig_chld(int signo) {
    pid_t pid;
    int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        printf("child-%d terminated\n", pid);
    }
    return;
}

int
main() {
    int svr_socket, cli_conn, svr_cnt, reuse;
    socklen_t len;
    struct sockaddr_in svr_addr, cli_addr;
    struct sigaction actions;
    pid_t pid;

    svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_socket < 0) {
        fprintf(stderr, "Failed to create server-side socket\n");
        exit(-1);
    }

    reuse = 1;
#ifdef SO_REUSEPORT
    if (setsockopt(svr_socket, SOL_SOCKET, SO_REUSEPORT, (const char *)&reuse, sizeof(reuse)) == -1) {
        fprintf(stderr, "Server-side socket failed to set option SO_REUSEPORT\n");
        exit(-1);
    }
#endif

    bzero(&svr_addr, sizeof(svr_addr));
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    svr_addr.sin_port = htons(PORT);

    if (bind(svr_socket, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) == -1) {
        close(svr_socket);
        fprintf(stderr, "Server-side socket failed to bind\n");
        exit(-1);
    }

    if (listen(svr_socket, 5) == -1) {
        close(svr_socket);
        fprintf(stderr, "Server-side socket failed to listen\n");
        exit(-1);
    }

    actions.sa_handler = sig_chld;
    sigemptyset(&actions.sa_mask);
    actions.sa_flags = 0;
    if (sigaction(SIGCHLD, &actions, NULL) < 0) {
        close(svr_socket);
        fprintf(stderr, "Server-side failed to bind signal handle\n");
        exit(-1);
    }

    len = sizeof(cli_addr);
    svr_cnt = 0;
    for (;;) {
        if (svr_cnt == 100) {
            break;
        }
        printf("Waiting for client connection ...\n");
        cli_conn = accept(svr_socket, (struct sockaddr *)&cli_addr, &len);
        if (cli_conn < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                fprintf(stderr, "Server-side socket failed to accept\n");
                exit(-1);
            }
        }
        if ((pid = fork()) == 0) {
            int resend = 100;
            char buf[1024];

            close(svr_socket);
            while (resend--) {
                read(cli_conn, buf, 1024);
                write(cli_conn, "Hi, I'm a simple concurrent server!", 35);
                msleep(300);
            }
            close(cli_conn);
            exit(0);
        } else {
            printf("Connected by %d:%d, send to child-%d\n", cli_addr.sin_addr.s_addr, cli_addr.sin_port, pid);
        }
        svr_cnt++;
        close(cli_conn);
    };

    close(svr_socket);
}
