#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

struct arg_struct {
    int threads_id;
    int sock;
    struct sockaddr_in client_address;
    char buff[1024];
    int bytes_read;
} args;

void *handle_connection(void *args) {
    struct arg_struct threads_args = *(struct arg_struct*)args;
    
    int client_address_len = sizeof(struct sockaddr_in);

    while (1) {
        if (sendto(threads_args.sock, threads_args.buff, threads_args.bytes_read, 0,
                (struct sockaddr *)&threads_args.client_address, sizeof(threads_args.client_address)) < 0) {
            perror("sendto");
            exit(3);
        }
        
        printf("Sent message(handler %d): %s", threads_args.threads_id, threads_args.buff);
        sleep(1);
    }

    close(threads_args.sock);
    pthread_exit(0); 
}

int main() {
    int sock;
    struct sockaddr_in addr;
    struct sockaddr_in client_address;
    char buff[1024];
    int bytes_read, pthreads_num = 0;
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

    pthread_t thread_id[5];

    while (bytes_read = recvfrom(sock, buff, 1024, MSG_WAITALL, 
                (struct sockaddr *)&client_address, &client_address_len)) {
        
        buff[bytes_read] = '\0';
        printf("Message received(main while): %s", buff);
        buff[1] = 'F';


        // init args
        struct arg_struct args_temp;
        args_temp.threads_id = pthreads_num;
        args_temp.sock = sock;
        args_temp.client_address = client_address;
        memcpy(args_temp.buff, buff, 1024);
        args_temp.bytes_read = bytes_read;

        if (pthread_create(&thread_id[pthreads_num], NULL, handle_connection, (void *)&args_temp) < 0) {
            perror("thread");
            exit(4);
        }
        
        if (pthread_detach(thread_id[pthreads_num])) {
            perror("detech");
            exit(7);
        }
        
        pthreads_num++;
        
        if (pthreads_num > 4) {
            printf("\nthreads limit\n");
            exit(6);
        }
    }

    if (bytes_read < 0) {
            perror("recvfrom");
            exit(3);
    }

    return 0;
}