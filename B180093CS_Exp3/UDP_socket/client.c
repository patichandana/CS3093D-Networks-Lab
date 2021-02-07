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
  socket_fd = socket(AF_INET,SOCK_DGRAM,0);
  checkError(socket_fd,"socket");
  green();
  printf("socket created at socket number: %d\n",socket_fd);
  return socket_fd;
}

int main(){
  int socket_fd,n,len;
  char message[1024];
  struct sockaddr_in serv_addr;
  socket_fd = createSocket();
  white();
  printf("Enter the message: ");
  fgets(message,1024,stdin);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port =  htons(DEST_PORT);
  serv_addr.sin_addr.s_addr = inet_addr(DEST_IP);
  memset(&(serv_addr.sin_zero),'\0',8);
  sendto(socket_fd, (const char *)message, strlen(message), MSG_CONFIRM, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
  green();
  printf("Message sent...\n");
  n = recvfrom(socket_fd, (char *)message, sizeof(message), MSG_WAITALL, (struct sockaddr *) &serv_addr, &len);
  message[n] = '\0';
  yellow();
  printf("Server : %s\n", message);
  close(socket_fd);
  return 0;
}
