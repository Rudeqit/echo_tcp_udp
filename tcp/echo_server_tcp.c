#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/**
 * struct sockaddr {
 *     unsigned short    sa_family;    // Семейство адресов, AF_xxx
 *     char              sa_data[14];  // 14 байтов для хранения адреса
 * * };
 * 
 * struct sockaddr_in {
 *     short int          sin_family;  // Семейство адресов
 *     unsigned short int sin_port;    // Номер порта
 *     struct in_addr     sin_addr;    // IP-адрес
 *     unsigned char      sin_zero[8]; // "Дополнение" до размера структуры sockaddr
 * };
 * 
 * struct in_addr {
 *     unsigned long s_addr;
 * };
  */

int main() {
    int sock, listener;

    struct sockaddr_in addr;
    char buff[1024];
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);     // SOCK_STREAM for TCP
    if (listener < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);      // host to network shorts(host byte order)
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // loopback
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);    // accept request queue

    while (1) {
        sock = accept(listener, NULL, NULL);
        if (sock < 0) {
            perror("accept");
            exit(3);
        }

        while (1) {
            bytes_read = recv(sock, buff, 1024, 0);
            if (bytes_read <= 0)
                break;
            printf("Message received: %s", buff);
            
            buff[1] = 'F';
            buff[bytes_read] = '\0';
            printf("Sent message: %s", buff);
            send(sock, buff, bytes_read, 0);

            // for (int i = 0; i < 1024; i++)
                // buff[i] = 0;
            memset(buff, 0, 1024);
        }

        close(sock);
    }

    return 0;
}