#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "daytime.h"

int main(void)
{
    struct sockaddr_in server, from;
    socklen_t_ fromlen;
    char buf[BUFFER_SIZE];
    SOCKET s;
    int n;
    time_t deadline;
    // initialize socket
    SOCK_INIT();
    // set bytes to zero
    memset(&server, 0, sizeof server);
    // set address to IPv4
    server.sin_family = AF_INET;
    // store server port from header
    server.sin_port = htons(SERVER_PORT);
    // handle failure
    if (inet_pton(AF_INET, SERVER_IP, &server.sin_addr) != 1) 
    {
        fprintf(stderr, "Invalid SERVER_IP\n");
        return 1;
    }
    // create UDP socket
    s = socket(AF_INET, SOCK_DGRAM, 0);
    // check for failure
    if (s == INVALID_SOCKET) { perror("socket"); return 1; }

    // Receive timeout so a lost datagram or only-spoofed traffic can't hang forever. 
#ifdef _WIN32
    DWORD tv = RECV_TIMEOUT_SEC * 1000;
#else
    struct timeval tv = { RECV_TIMEOUT_SEC, 0 };
#endif
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);

    // Request an empty datagram.
    if (sendto(s, "", 0, 0, (struct sockaddr *)&server, sizeof server) < 0) 
    {
        perror("sendto");
        return 1;
    }

    deadline = time(NULL) + RECV_TIMEOUT_SEC;
    for (;;) {
        fromlen = sizeof from;
        n = recvfrom(s, buf, sizeof buf - 1, 0, (struct sockaddr *)&from, &fromlen);
        if (n < 0) {
            fprintf(stderr, "No reply from %s:%d (timed out)\n", SERVER_IP, SERVER_PORT);
            CLOSESOCK(s);
            SOCK_CLEANUP();
            return 1;
        }
        // Accept only packets whose sender IP matches server
        if (from.sin_addr.s_addr != server.sin_addr.s_addr) 
        {
            fprintf(stderr, "Discarded packet from unexpected sender %s\n",
                    inet_ntoa(from.sin_addr));
            // print error if timeout
            if (time(NULL) >= deadline) 
            {
                fprintf(stderr, "Timed out waiting for the real server\n");
                CLOSESOCK(s);
                SOCK_CLEANUP();
                return 1;
            }
            continue;
        }
        buf[n] = '\0';
        printf("%s", buf);
        break;
    }
    // close out socket
    CLOSESOCK(s);
    SOCK_CLEANUP();
    return 0;
}
