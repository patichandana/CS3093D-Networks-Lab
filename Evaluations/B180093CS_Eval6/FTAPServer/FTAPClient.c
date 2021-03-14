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
#define DEST_PORT 2021
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

void sendFile(int socket_fd,char *fileName){
	FILE *fp = fopen(fileName,"r");
	char data[SIZE] = {0};
	while(fgets(data,SIZE,fp)!=NULL){
		int value = send(socket_fd, data, sizeof(data),0);
		checkError(value,"send");
		printf("%s",data);
		bzero(data,SIZE);
	}
  fclose(fp);
	char *eof = "EOF";
	send(socket_fd,eof,strlen(eof)+1,0);
	green();
	printf("........FILE SENT.........\n");
}

void sendMessage(char *message,int socket_fd){
  int value;
  //printf("%s",message);
  blue();
  value = send(socket_fd,(void *)message,strlen(message)+1,0);
  checkError(value,"send");
  const char s[2] = " ";
  char *command,*name;
  command = strtok(message, s);
  if(strcmp(command,"StoreFile")==0){
      name = strtok(NULL, s);
      sendFile(socket_fd,name);
    }
  green();
}

void recvMessage(int socket_fd){
    char message[500];
    while(1){
      int value = recv(socket_fd,message,500,0);
    checkError(value,"recieve");
    yellow();
    if(strcmp(message,"200") == 0){
      printf("connection established...\n");
      return ;
    }else if(strcmp(message,"300") == 0){
      printf("Correct Username; Need password...\n");
      return ;
    } else if(strcmp(message,"301") == 0){
      printf("InCorrect Username...\n");
      return ;
    } else if(strcmp(message,"305") == 0){
      printf("User Authenticated with password...\n");
      return ;
    } else if(strcmp(message,"310") == 0){
      printf("Incorrect password...\n");
      return ;
    } else if(strcmp(message,"505") == 0){
      printf("Command not supported...\n");
      return ;
    } else if(strcmp(message,"OVER")==0){
      return ;
    } if(strcmp(message,"100") == 0){
      printf("Success...\n");
      return ;
    }if(strcmp(message,"0") == 0){
      printf("Failed...\n");
      return ;
    }else{
      printf("%s\n",message);
    }
    bzero(message,500);
    }
}

void recvFile(int socket_fd,char *fileName){
	char message[SIZE];
	//char *fileName = "receivedFile.dat";
  char currentDir[100] = "./";
  strcat(currentDir,fileName);
  bzero(message, SIZE);
	FILE *fp = fopen(currentDir,"w");
  if(fp ==NULL)
    printf("failed file pointer\n");
	while(1){
		int n = recv(socket_fd,message,SIZE,0);
   // printf("%s",message);
		if(strcmp(message,"EOF")==0){
      fclose(fp);
			printf(".........FILE RECEIVED........\n");
			return ;
		}
		fprintf(fp,"%s",message);
		printf("%s",message);
		bzero(message, SIZE);
	}
}

int main(int argc, char *argv[]) {
  int socket_fd;
  char message[500];
  struct sockaddr_in serv_addr;
  blue();
  socket_fd = createSocket();
  printf(">> ");
  gets(message);
  while(1){
  if(strcmp(message,"START")==0){
    serverConnect(socket_fd);
  //recvMessage(socket_fd);
  break;
  }
  else{
    printf("Enter START to start the connection to the server...\n");
    gets(message);
  }
  }
  while(1){
    white();
    printf(">> ");
    gets(message);
    //printf("%s\n",message);
    sendMessage(message,socket_fd);
    if(strcmp(message,"QUIT")==0){
      close(socket_fd);
      red();
      printf("connection closed...\n");
      return 0;
    }else
    if(strcmp(message,"ListDir")==0){
      recvMessage(socket_fd);
    }
    else{
      char token[100];
      char s[2] = " ";
      strcpy(token, strtok(message,s));
     if(strcmp(token,"GetFile")==0){
      //strcpy(token, strtok(NULL,s));
      //printf("%s",token);
      recvFile(socket_fd,token);
    }
    else
    	recvMessage(socket_fd);
    }
  }
}
