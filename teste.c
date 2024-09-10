#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define LISTENQ 10
#define MAXDATASIZE 100

int main (int argc, char **argv) {
    int    listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    char   buf[MAXDATASIZE];
    time_t ticks;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(8080); 

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    socklen_t size_servaddr = sizeof(servaddr);
    if (getsockname(listenfd, (struct sockaddr *)&servaddr, &size_servaddr) == -1) {
        perror("getsockname");
        exit(1);
    }

    printf("O servidor está escutando na porta %d\n", ntohs(servaddr.sin_port));
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen");
        exit(1);
    }

    for ( ; ; ) {
        clilen = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) == -1 ) {
            perror("accept");
            exit(1);
        }

        // Obter informações do socket remoto
        struct sockaddr_in peeraddr;
        socklen_t peerlen = sizeof(peeraddr);
        if (getpeername(connfd, (struct sockaddr *)&peeraddr, &peerlen) == -1) {
            perror("getpeername");
            exit(1);
        }

        char peer_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &peeraddr.sin_addr, peer_ip, sizeof(peer_ip));
        printf("IP remoto: %s\n", peer_ip);
        printf("Porta remota: %d\n", ntohs(peeraddr.sin_port));

        // Obter informações do socket local
        struct sockaddr_in localaddr;
        socklen_t locallen = sizeof(localaddr);
        if (getsockname(connfd, (struct sockaddr *)&localaddr, &locallen) == -1) {
            perror("getsockname");
            exit(1);
        }

        char local_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &localaddr.sin_addr, local_ip, sizeof(local_ip));
        printf("IP local: %s\n", local_ip);
        printf("Porta local: %d\n", ntohs(localaddr.sin_port));

        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "Hello from server!\nTime: %.24s\r\n", ctime(&ticks));
        write(connfd, buf, strlen(buf));

        close(connfd);
    }
    return(0);
}