#include<arpa/inet.h>
#include<stdbool.h>
#include "./colors.c"
#include<netinet/in.h>
#include<sys/select.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<time.h>
#include<sys/types.h>
#include<unistd.h>

#define MYPORT 8888
#define BACKLOG 100
#define MAX_CLIENTS 100

void checkError(int number, char *error){
  if(number < 0){
  red();
  perror(error);
  exit(1);
  }
}

int createSocket(){
  int socket_fd;
  blue();
  printf("Creating a socket...\n");
  socket_fd = socket(AF_INET,SOCK_STREAM,0);
  checkError(socket_fd,"socket");
  green();
  printf("socket created at socket number: %d\n",socket_fd);
  return socket_fd;
}

struct sockaddr_in bindToPort(int socket_fd){
  struct sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(MYPORT);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(my_addr.sin_zero,'\0',sizeof(my_addr.sin_zero));
  int integer = bind(socket_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
  checkError(integer,"bind");
  green();
  printf("binded the socket to a port...\n");
  return my_addr;
}

int acceptRequests(int socket_fd){
  struct sockaddr_storage client_addr;
  int addr_size = sizeof(struct sockaddr_in);
  int client_sock = accept(socket_fd,(struct sockaddr*)&client_addr,&addr_size);
  checkError(client_sock,"request");
  green();
  printf("accepted request...\n");
  return client_sock;
}

void broadcastMessage(int i, char message[1000], char names[MAX_CLIENTS][100],int client_sockets[MAX_CLIENTS],int clientsNum){
    for(int j = 0;j<clientsNum;++j){
      printf("The message: %s",message);
      printf("Message length: %d\n",strlen(message));
        if(j!=i)
            send(client_sockets[j],(void *)message,strlen(message)+1,0);
    }
}

void sendNames(int i, char names[MAX_CLIENTS][100],int client_sockets[100],int clientsNum){
  char message[100] = "..........Welcome to chat room...........\n";
  send(client_sockets[i],message,strlen(message),0);
  if(clientsNum>1){
      for(int j=0;j<clientsNum;++j){
        if(j!=i){
          send(client_sockets[i],names[j],strlen(names[j]),0);
          printf("%s\n",names[j]);
        }
      }
    strcpy(message,"are online currently...\0");
    printf("First message: %s\n",message);
    printf("Length of the message: %d\n",strlen(message));
    send(client_sockets[i],(void *)message,strlen(message)+1,0);
  }
  else{
    strcpy(message,"no one's online currently...");
    printf("First message: %s\n",message);
      printf("Length of the message: %d\n",strlen(message));
      send(client_sockets[i],message,strlen(message)+1,0);
  }
  printf("%s",message);
}

int getName(int i,char names[MAX_CLIENTS][100],int client_sockets[MAX_CLIENTS],int clientsNum){
    char temp[100];
    recv(client_sockets[i],names[i],100,0);
    //names[i][strlen(names[i])] = '\0';
    printf("Client name: %s\n",names[i]);
    strcat(names[i]," ");
    strcpy(temp,names[i]);
    char string[50] = "has entered the chat...";
    strcat(temp,string);
    broadcastMessage(i,temp,names,client_sockets,clientsNum);
}

int recvMessage(int i, char names[MAX_CLIENTS][100],int client_sockets[MAX_CLIENTS],int clientsNum){
  char client_message[1000],temp[2000];
  strcpy(temp,names[i]);
  //printf("%s",temp);
  strcat(temp,": ");
  int return_val = recv(client_sockets[i],(void *)client_message,sizeof client_message,0);
  if(return_val<=0 || strcmp(client_message,"tata") == 0)
    return 0;
  strcat(temp,client_message);
  yellow();
  broadcastMessage(i,temp,names,client_sockets,clientsNum);
  //printf("client message: %s\n",client_message );
}

int main(){
  int master_socket_fd,client_fd;
  int clientsNum = 0;
  int client_sockets[MAX_CLIENTS];
  int names_given[100];
  char names[100][100];
  struct sockaddr_in my_addr;
  fd_set current_sockets,ready_sockets;
  master_socket_fd = createSocket();
  my_addr = bindToPort(master_socket_fd);
  blue();
  printf("listening to incoming requests...\n");
  listen(master_socket_fd,BACKLOG);
  FD_ZERO(&current_sockets);
  FD_SET(master_socket_fd,&current_sockets);
  for(int i = 0;i<MAX_CLIENTS;++i){
      names_given[i] = 0;
    client_sockets[i] = 0;
  }
  while(1){
      ready_sockets = current_sockets;
      select(MAX_CLIENTS,&ready_sockets,NULL,NULL,NULL);
      if(FD_ISSET(master_socket_fd,&ready_sockets)){
          int temp = acceptRequests(master_socket_fd);
          FD_SET(temp,&current_sockets);
          client_sockets[clientsNum] = temp;
          ++clientsNum;
      }
      ready_sockets = current_sockets;
      select(MAX_CLIENTS,&ready_sockets,NULL,NULL,NULL);
    for(int i = 0;i<clientsNum;++i){
        if(FD_ISSET(client_sockets[i],&ready_sockets)){
            if(!names_given[i]){
                getName(i,names,client_sockets,clientsNum);
                names_given[i] = 1;
                sendNames(i,names,client_sockets,clientsNum);
            }else{
                if(!recvMessage(i,names,client_sockets,clientsNum)){
                  client_sockets[i] = 0;
                  names_given[i] = 0;
                  strcat(names[i],"left the chat room...");
                  broadcastMessage(i,names[i],names,client_sockets,clientsNum);
                  close(client_sockets[i]);
                }
            }
        }
    }  
  }
  close(master_socket_fd);
  red();
  printf("connection closed...");
}
