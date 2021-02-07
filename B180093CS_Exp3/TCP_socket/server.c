#include<arpa/inet.h>
#include "colors.c"
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>

#define MYPORT 8888
#define BACKLOG 10

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

int recvMessage(int socket_fd){
  int client_message;
  recv(socket_fd,(void *)&client_message,sizeof client_message,0);
  yellow();
  printf("client message: %d\n",client_message );
  return client_message;
}

void replyMessage(int socket_fd,int number){
  int square_number = number*number;
  send(socket_fd,(void *)&square_number,sizeof square_number,0);
  white();
  printf("message sent to client...\n");
}

int main(){
  int *num;
  int socket_fd;
  struct sockaddr_in my_addr;
  socket_fd = createSocket();
  my_addr = bindToPort(socket_fd);
  blue();
  printf("listening to incoming requests...\n");
  listen(socket_fd,BACKLOG);
  int client_fd = acceptRequests(socket_fd);
  while(1){
    int client_message = recvMessage(client_fd);
    replyMessage(client_fd,client_message);
  }
  close(socket_fd);
  red();
  printf("connection closed...");
}
