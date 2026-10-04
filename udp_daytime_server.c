#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "daytime.h"

int main(void)
{
    struct sockaddr_in addr, client;
    socklen_t_ clientlen;
    char req[BUFFER_SIZE], reply[BUFFER_SIZE];
    SOCKET s;
    time_t now;
    int n;

    SOCK_INIT();

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == INVALID_SOCKET) { perror("socket"); return 1; }

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERVER_PORT);
    if (bind(s, (struct sockaddr *)&addr, sizeof addr) < 0) {
        perror("bind");
        return 1;
    }
    printf("UDP Daytime server listening on port %d\n", SERVER_PORT);

    for (;;) {
        clientlen = sizeof client;
        n = recvfrom(s, req, sizeof req, 0, (struct sockaddr *)&client, &clientlen);
        if (n < 0) { perror("recvfrom"); continue; }

        now = time(NULL);
        strftime(reply, sizeof reply, "%A, %B %d, %Y %H:%M:%S\r\n", localtime(&now));
        sendto(s, reply, (int)strlen(reply), 0, (struct sockaddr *)&client, clientlen);
        printf("Served %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    }
}
