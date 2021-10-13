#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>

char names[3][25];

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

    int server_fd = 0;
    int opt_val = 1;
    pthread_t thread_id[4];

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Failed to create socket");
        return 1;
    }

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_val, sizeof(opt_val))){
        perror("Failed to setsockopt");
        return 1;
    }

    sockaddr_in socket_info;
    int socket_info_len = sizeof(socket_info);

    socket_info.sin_addr.s_addr = INADDR_ANY;
    socket_info.sin_family = AF_INET;
    socket_info.sin_port = htons(5000);

    if(bind(server_fd, (sockaddr*)&socket_info, socket_info_len) != 0){
        perror("Failed to bind");
        return 1;
    }

    if(listen(server_fd, 2) != 0){
        perror("Failed to listen");
        return 1;
    }

    int client_fds = 0;
    int count = 0;
    char name[25] = "";

    if((client_fds = accept(server_fd, (sockaddr*)&socket_info, (socklen_t*)&socket_info_len)) < 0){
        perror("Failed to accept");
        return 1;
    }

    args* a = (args*)malloc(sizeof(args));
    a->client_fd = client_fds;

    if(pthread_create(&thread_id[count], NULL, &sends, a) != 0){
        perror("Failed to create thread!");
        return 1;
    }

    count++;

    if(pthread_create(&thread_id[count], NULL, &recvs, a) != 0){
        perror("Failed to create thread!");
        return 1;
    }
    
    // pthread_join(thread_id[count - 1], NULL);
    pthread_join(thread_id[count], NULL);

    return 0;
}