#include<arpa/inet.h>
#include "colors.c"
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>

#define MYPORT 8080
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
  my_addr.sin_addr.s_addr = inet_addr("127.0.0.4");
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

void replyMessage(int socket_fd,char message[1000]){
  write(socket_fd, "HTTP/1.1 200 OK\n", 16);
    write(socket_fd, "Content-length: 151\n", 19);
    write(socket_fd, "Content-Type: text/html\n\n", 25);
    write(socket_fd,message, strlen(message)+1);
    //send(socket_fd,(void *)&square_number,sizeof square_number,0);
  white();
  printf("message sent to client...\n");
}

void recvMessage(int socket_fd){
  char client_message[1000];
  recv(socket_fd,(void *)&client_message,sizeof client_message,0);
  char method[8], route[128], version[16];
  sscanf(client_message, "%s %s %s", method, route, version);
  printf("%s\n", route);
  printf("MESSAGE: \n%s\n", client_message);
  char message[1000];
  if(strcmp(route,"/")==0)
  	strcpy(message,"<!DOCTYPE html><html><body><h1>Welcome to Dept of CSE!</h1></body></html>");
  else if(strcmp(route,"/mypage.html")==0)
  		strcpy(message,"<html><h1> Welcome to Networks Lab </h1>You are attending evaluation question.</html>");
       else
  		strcpy(message,"<html><h1>404 error: File not found</h1></html>");
  replyMessage(socket_fd,message);
  yellow();
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
  while(1){
  int client_fd = acceptRequests(socket_fd);
  recvMessage(client_fd);
  close(client_fd);
  }
  //close(socket_fd);
  red();
  printf("connection closed...");
}
