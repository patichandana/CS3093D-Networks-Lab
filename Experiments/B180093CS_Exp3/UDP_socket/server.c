#include<arpa/inet.h>
#include "../colors.c"
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
  socket_fd = socket(AF_INET,SOCK_DGRAM,0);
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

int main(){
  int *num;
  char message[1024],*reply;
  reply = "Hey there";
  int socket_fd;
  struct sockaddr_in my_addr,client_addr;
  socket_fd = createSocket();
  my_addr = bindToPort(socket_fd);
  int len, n;
  len = sizeof(client_addr);
  n = recvfrom(socket_fd, (char *)message,sizeof(message), MSG_WAITALL, ( struct sockaddr *) &client_addr, &len);
  message[n] = '\0';
  yellow();
  printf("Client : %s\n", message);
  sendto(socket_fd, (const char *)reply, strlen(reply),  MSG_CONFIRM, (const struct sockaddr *) &client_addr, len);
  green();
  printf("Message sent...\n");
  return 0;
}
