#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int sock;
    struct sockaddr_in addr;
    struct sockaddr_in client_address;
    char buff[1024];
    int bytes_read;
    int client_address_len = sizeof(struct sockaddr_in);     // ONLY BECAUSE LEN WAS NOT ASSIGNED recvfrom DIDN'T WORK wtf (Address family not supported by protocol)

    sock = socket(AF_INET, SOCK_DGRAM, 0);     // SOCK_DFRAM for UPD
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);      // host to network shorts(host byte order)
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // loopback
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }

    while (1) {
        bytes_read = recvfrom(sock, buff, 1024, MSG_WAITALL, 
                (struct sockaddr *)&client_address, &client_address_len);
        if (bytes_read < 0) {
            perror("recvfrom");
            exit(3);
        }
        buff[bytes_read] = '\0';
        printf("Message received: %s", buff);
        buff[1] = 'F';

        if (sendto(sock, buff, bytes_read, 0,
                (struct sockaddr *)&client_address, sizeof(client_address)) < 0) {
            perror("sendto");
            exit(3);
        }
        
        printf("Sent message: %s", buff);
    }

    close(sock);

    return 0;
}