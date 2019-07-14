#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

char msg1[] = "Hello there!\n";
// char msg2[] = "Bye bye!\n";

int main() {
    int sock;
    struct sockaddr_in addr;
    char buff[1024];
    int bytes_read;
    int address_len = 1024;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    // bzero(&(addr.sin_zero),8);

    bytes_read = sendto(sock, msg1, sizeof(msg1), MSG_CONFIRM,
           (struct sockaddr *)&addr, sizeof(addr));
    if (bytes_read < 0) {
        perror("sendto");
        exit(3);
    }
    printf("Sent message: %s", msg1);

    bytes_read = recvfrom(sock, buff, bytes_read, MSG_WAITALL, 
            (struct sockaddr *)&addr, &address_len);
    if (bytes_read < 0) {
        perror("recvfrom");
        exit(3);
    }
    buff[bytes_read] = '\0';
    printf("Message received: %s", buff);

    close(sock);

    return 0;
}