#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>

struct args{
    int client_fd;
    int index;
};

void* recvs(void* arguments){
    args* my_args = (args*)arguments;
    char msg[1024] = "";
    while(true){
        if(read(my_args->client_fd, &msg, 1024) <= 0){
            close(my_args->client_fd);
            exit(EXIT_SUCCESS);
        }
        if(strcmp(msg,"") == 0) continue;
        printf("%s\n", msg);
        strcpy(msg,"");
    }
}

void* sends(void* arguments){
    char msg[100] = "";
    args* my_args = (args*)arguments;
    while(true){
        scanf("%[^\n]", msg);
        getchar();
        if(strcmp(msg,"exit") == 0){
            close(my_args->client_fd);
            exit(EXIT_SUCCESS);
        }
        if(send(my_args->client_fd, msg, 1024, 0) == -1){
            perror("Failed to send message!");
            exit(EXIT_FAILURE);
        }
        usleep(300);
    }
}

int main(){

    int client_fd = 0;
    sockaddr_in socket_info;
    int socket_info_len = sizeof(socket_info);

    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    socket_info.sin_port = htons(5000);
    socket_info.sin_family = AF_INET;

    if(inet_pton(AF_INET, "127.0.0.1", &socket_info.sin_addr.s_addr) == -1){
        perror("Invalid IP address");
        exit(EXIT_FAILURE);
    }

    if(connect(client_fd, (sockaddr*) &socket_info, socket_info_len) == -1){
        perror("Failed to connect");
        exit(EXIT_FAILURE);
    }

    pthread_t thread_id[4];
    int count = 0;

    args *a = (args*)malloc(sizeof(args));
    a->client_fd = client_fd;

    if(pthread_create(&thread_id[count], NULL, &sends, a) != 0){
        perror("Failed to create thread!");
        return 1;
    }

    count++;

    if(pthread_create(&thread_id[count], NULL, &recvs, a) != 0){
        perror("Failed to create thread!");
        return 1;
    }

    // pthread_join(thread_id[count-1], NULL);
    pthread_join(thread_id[count], NULL);
    


    return 0;
}