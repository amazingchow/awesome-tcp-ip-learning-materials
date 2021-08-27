#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int
main() {
    int cli_socket, n;
    struct sockaddr_in svr_addr;

    cli_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (cli_socket < 0) {
        fprintf(stderr, "Failed to create client-side socket\n");
        exit(-1);
    }

    bzero(&svr_addr, sizeof(svr_addr));
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = inet_addr("192.168.1.115");
    svr_addr.sin_port = htons(18989);

    if (connect(cli_socket, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) == -1) {
        fprintf(stderr, "Client-side socket failed to connect\n");
        exit(-1);
    }

    for (;;) {
        char buff[512];
        bzero(buff, sizeof(buff));
        if ((n = read(cli_socket, buff, sizeof(buff))) == 0) {
            break;
        }
    }

    shutdown(cli_socket, SHUT_RDWR);
    close(cli_socket);
}
