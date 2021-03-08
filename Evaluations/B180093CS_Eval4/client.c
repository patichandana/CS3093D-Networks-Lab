#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include"colors.c"

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

void sendMessage(char message[1000],int socket_fd){
  int value;
  //printf("%s",message);
  value = send(socket_fd,(void *)message,strlen(message)+1,0);
  checkError(value,"send");
  //printf("sent\n");
}

void recvMessage(int socket_fd){
    char message[1000];
    int value = recv(socket_fd,(void *)message,sizeof(message),0);
    checkError(value,"recieve");
    white();
    printf("%s\n",message);
}

int main(int argc, char *argv[]) {
  int socket_fd;
  char reply[100];
  char message[100];
  char password[1000];
  struct sockaddr_in serv_addr;
  blue();
  socket_fd = createSocket();
  serverConnect(socket_fd);
  printf("Enter your password: ");
  scanf("%[^\n]%*c",password);
  send(socket_fd,password,strlen(password)+1,0);
  recv(socket_fd,reply,strlen(reply),0);
  char wrong[100] = "wr";
  //printf("Reply from the server: %s\n",reply);
  if(strcmp(reply,wrong)==0){
    printf("Wrong password, Access denied!");
    close(socket_fd);
    return 0;
  }
  printf("Accepted request.....\n");
  char name[100];
  printf("Enter your name: ");
  scanf("%[^\n]%*c",name);
  send(socket_fd,name,strlen(name)+1,0);
  //recv(socket_fd,reply,strlen(reply),0);
  if(fork() == 0){
    while(1){
      white();
      //printf(">>");
      recvMessage(socket_fd);
      //printf(">>");
    }
  }else{
     while(1){
    white();
    scanf("%[^\n]%*c",message);
    yellow();
    printf("you : %s\n",message);
    if(strcmp(message,"bye")==0){
      sendMessage(message,socket_fd); 
      close(socket_fd);
      return 0;
    }
    white();
    sendMessage(message,socket_fd);   
  }
  }
}
