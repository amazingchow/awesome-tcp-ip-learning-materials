#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 18989

int
main() {
    int svr_socket, cli_conn, svr_cnt, reuse;
    socklen_t len;
    struct sockaddr_in svr_addr, cli_addr;

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
        fprintf(stderr, "Server-side socket failed to bind\n");
        exit(-1);
    }

    if (listen(svr_socket, 5) == -1) {
        fprintf(stderr, "Server-side socket failed to listen\n");
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
            fprintf(stderr, "Server-side socket failed to accept\n");
            exit(-1);
        }
        printf("Connected by %d:%d\n", cli_addr.sin_addr.s_addr, cli_addr.sin_port);
        svr_cnt++;
        shutdown(cli_conn, SHUT_RDWR);
        close(cli_conn);
    };

    close(svr_socket);
}
