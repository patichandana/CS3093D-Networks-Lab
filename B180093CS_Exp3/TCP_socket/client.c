#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include"../colors.c"

#define DEST_IP "127.0.0.1"
#define DEST_PORT 8888

void checkError(int number, char *error){
  if(number < 0){
  red();
  perror(error);
  exit(1);
  }
}

int createSocket(){
  int socket_fd;
  printf("Creating a socket...\n");
  socket_fd = socket(AF_INET,SOCK_STREAM,0);
  checkError(socket_fd,"socket");
  green();
  printf("socket created at socket number: %d\n",socket_fd);
  return socket_fd;
}

void serverConnect(int socket_fd){
  int value;
  struct sockaddr_in serv_addr;
  blue();
  printf("Connecting to the server...\n");
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port =  htons(DEST_PORT);
  serv_addr.sin_addr.s_addr = inet_addr(DEST_IP);
  memset(&(serv_addr.sin_zero),'\0',8);
  value = connect(socket_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr));
  checkError(value,"connect");
  green();
  printf("connection established...\n");
}

void sendMessage(int num,int socket_fd){
  int value;
  value = send(socket_fd,(void *)&num,sizeof(num),0);
  checkError(value,"send");
  green();
  printf("Values sent to server...\n");
}

void recvMessage(int socket_fd){
    int num;
    int value = recv(socket_fd,(void *)&num,sizeof(num),0);
    checkError(value,"recieve");
    yellow();
    printf("Square of entered number: %d\n",num);
}

int main(int argc, char *argv[]) {
  int num, socket_fd;
  struct sockaddr_in serv_addr;
  blue();
  socket_fd = createSocket();
  serverConnect(socket_fd);
  while(1){
    white();
    printf("Enter a number: ");
    scanf("%d",&num);
    if(num == 0){
      close(socket_fd);
      red();
      printf("connection closed...\n");
      return 0;
    }
    sendMessage(num,socket_fd);
    recvMessage(socket_fd);
  }
}
