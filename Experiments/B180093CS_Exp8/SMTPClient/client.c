#include<arpa/inet.h>
#include<stdbool.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include"../colors.c"

#define DEST_IP "127.0.0.1"
#define DEST_PORT 25
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

void sendMessage(char message[1000],int socket_fd){
  int value;
  blue();
  printf("Client: %s\n",message);
  value = send(socket_fd,(void *)message,strlen(message)+1,0);
  checkError(value,"send");
  green();
}

void printResponse(int statusCode,int socket_fd){
    green();
    printf("Server: ");
    if(statusCode == 220)
        printf("SMTP Service ready\n");
        else if(statusCode == 221)
            printf("Service closing\n");
            else if(statusCode == 250)
                printf("Requested action taken and completed\n");
                else if(statusCode == 450)
                    printf("The requested command failed because the user’s mailbox was unavailable, try again later\n");
                    else if(statusCode == 501)
                        printf("A syntax error was encountered in command arguments\n");
                        else if(statusCode == 503)
                            printf("The server has encountered a bad sequence of commands\n");
                            else if(statusCode == 553)
                                printf("The command was aborted because the mailbox name is invalid\n");
                            else if(statusCode == 554){
                                printf("Wrong username, Please try again\n");                               
                            }
                                else if(statusCode == 555){
                                    printf("Wrong password, Please try again\n");
                                }else
                                if(statusCode == 251)
                                    printf("correct username\n");
}

int recvStatusCode(int socket_fd){
    int statusCode;
    int value = recv(socket_fd,&statusCode,sizeof(statusCode),0);
    checkError(value,"recieve");
    printResponse(statusCode,socket_fd);
    return statusCode;
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
  //printf("connection established...\n");
  recvStatusCode(socket_fd);
}

bool correctFormat(char fromAddr[1000],char toAddr[1000],char senderAddr[100],char recvrAddr[100]){
    const char delim[2]= " ";
    char temp[1000]= "'";
    if(strcmp(strtok(fromAddr,delim),"From:")==0){
        strcpy(senderAddr,strtok(NULL,delim));
        strcat(temp,senderAddr);
        strcpy(senderAddr,temp);
        strcpy(temp,"'");
        strcat(senderAddr,temp);
         if(strcmp(strtok(toAddr,delim),"To:")==0){
             strcpy(recvrAddr,strtok(NULL,delim));
             strcat(temp,recvrAddr);
             strcpy(recvrAddr,temp);
             strcpy(temp,"'");
             strcat(recvrAddr,temp);
             return true;
    }
    }
    return false;
}

void sendData(char message[100][1000],int socket_fd){
    yellow();
    printf("----------Sending Data start----------\n");
    for(int i=0;i!=-1;++i){
          sendMessage(message[i],socket_fd);
          recvStatusCode(socket_fd);
          if(strcmp(message[i],".")==0){
              break;
          }
    }
    yellow();
    printf("----------Sending Data done----------\n");
    green();
}

void authenticate(int socket_fd){
    char username[100],password[100];
    bzero(username,100);
    bzero(password,100);
    sendMessage("Auth",socket_fd);
    printf("Enter your username: ");
    gets(username);
    printf("Enter your password: ");
    gets(password);
    sendMessage(username,socket_fd);
    int statusCode = recvStatusCode(socket_fd);
     if(statusCode==554 || statusCode==555){
        close(socket_fd);
        exit(1); 
    }
    sendMessage(password,socket_fd);
    statusCode = recvStatusCode(socket_fd);
    if(statusCode==554 || statusCode==555){
        close(socket_fd);
        exit(1); 
    }
}

int main(int argc, char *argv[]) {
  int socket_fd;
  char fromAddr[1000],toAddr[1000],message[100][1000],senderAddr[50],recvrAddr[50],command[100];
  struct sockaddr_in serv_addr;
  blue();
  socket_fd = createSocket();
  serverConnect(socket_fd);
  yellow();
  //authentication
  authenticate(socket_fd);
  while(1){
  for(int i=0;i<100;++i)
    bzero(message[i],1000);
  printf(">> ");
  gets(command);
  if(strcmp(command,"Send Mail")==0){
  sendMessage(command,socket_fd);
  gets(fromAddr);
  gets(toAddr);
  for(int i = 0;i!=-1;++i){
      gets(message[i]);
      if(strcmp(message[i],".")==0){
        break;
      }
  }
  if(correctFormat(fromAddr,toAddr,senderAddr,recvrAddr)){
  printf("----------SMTP handshake starting----------\n");
  sendMessage("EHLO",socket_fd);
  recvStatusCode(socket_fd);
  char temp[100]="MAIL FROM ";
  strcat(temp,senderAddr);
  sendMessage(temp,socket_fd);
  recvStatusCode(socket_fd);
  strcpy(temp,"RCPT TO ");
  strcat(temp,recvrAddr);
  sendMessage(temp,socket_fd);
  recvStatusCode(socket_fd);
  yellow();
  printf("----------SMTP handshake over----------\n");
  sendMessage("DATA",socket_fd);
  recvStatusCode(socket_fd);
  sendData(message,socket_fd);
  recvStatusCode(socket_fd);
  }else{
      printf("Wrong format of the mail, please try again!\n");
  }
  }else if(strcmp(command,"QUIT")==0){
       sendMessage("QUIT",socket_fd);
        recvStatusCode(socket_fd);
        close(socket_fd);
        exit(0);
  }else{
      printf("Wrong command\n");
  }
  }
}
