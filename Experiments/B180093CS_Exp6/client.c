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
#define SIZE 500

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

void sendMessage(char *message,int socket_fd){
  int value;
  //printf("%s",message);
  blue();
  value = send(socket_fd,(void *)message,strlen(message)+1,0);
  checkError(value,"send");
  green();
}

void recvMessage(int socket_fd){
    char message[500];
    int value = recv(socket_fd,message,sizeof(message),0);
    checkError(value,"recieve");
    yellow();
    printf("Server: %s\n",message);
}

void recvFile(int socket_fd){
	char message[SIZE];
	char *fileName = "receivedFile.dat";
	FILE *fp = fopen(fileName,"w");
	while(1){
		int n = recv(socket_fd,message,SIZE,0);
		if(strcmp(message,"EOF")==0){
			printf(".........FILE RECEIVED........\n");
			return ;
		}
		fprintf(fp,"%s",message);
		//printf("%s",message);
		bzero(message, SIZE);
	}
}

int main(int argc, char *argv[]) {
  int socket_fd;
  char message[500];
  struct sockaddr_in serv_addr;
  blue();
  socket_fd = createSocket();
  serverConnect(socket_fd);
  while(1){
    white();
    printf("Enter your message: ");
    gets(message);
    sendMessage(message,socket_fd);
    if(strcmp(message,"bye")==0){
      close(socket_fd);
      red();
      printf("connection closed...\n");
      return 0;
    }
    if(strcmp(message,"GimmeYourVideo")==0){
	recvFile(socket_fd);
    }
    else
    	recvMessage(socket_fd);
  }
}
