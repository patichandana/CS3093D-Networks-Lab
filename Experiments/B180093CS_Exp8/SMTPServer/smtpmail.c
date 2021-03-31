#include<stdbool.h>
#include<stdio.h>
#include "../colors.c"
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<time.h>
#include<unistd.h>

#define BACKLOG 10

long int MYPORT;

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

void sendResponseCode(int socket_fd, int responseCode){
    printf("Response code: %d sending it...\n",responseCode);
    int value = send(socket_fd,(void *)&responseCode,sizeof responseCode,0);
    checkError(value,"send");
    green();
    printf("Response code sent!\n");
}

int acceptRequests(int socket_fd){
  struct sockaddr_storage client_addr;
  int addr_size = sizeof(struct sockaddr_in);
  int client_sock = accept(socket_fd,(struct sockaddr*)&client_addr,&addr_size);
  checkError(client_sock,"request");
  green();
  printf("accepted request...\n");
  sendResponseCode(client_sock,220);
  return client_sock;
}

void recvMessage(int socket_fd,char client_message[1000]){
  int value = recv(socket_fd,(void *)client_message,1000,0);
  checkError(value,"receive");
  yellow();
  printf("client message: %s\n",client_message);
}

bool userExists(char mailID[100]){
    char delim[2]="@";
    char temp[100];
    char username[100];
    char name[100];
    FILE *fptr = fopen("logincred.txt","r");
    printf("file opened\n");
    char command[1000];
    strcpy(temp,mailID);
    strcpy(name,strtok(temp,delim));
    printf("Name: %s\n",name);
    while(fscanf(fptr, "%[^\n]", command) != EOF){
    fgetc(fptr);
    printf("%s\n",command);
    const char delim2[2] = ",";
    strcpy(username ,strtok(command,delim2));
    printf("Username: %s\n",username);
    if(strcmp(username,name)==0){
        printf("%s\n",username);
        fclose(fptr);
      return true;
   } 
  }
  fclose(fptr);
  return false;
}

void smtpHandShake(int socket_fd,char sender[100],char recvr[100]){
    char message[1000];
    recvMessage(socket_fd,message);
    const char delim[2]="'";
    if(strcmp(message,"EHLO")==0){
        sendResponseCode(socket_fd,250);
        recvMessage(socket_fd,message);
        if(strcmp(strtok(message,delim),"MAIL FROM ")==0){
            strcpy(sender,strtok(NULL,delim));
            if(userExists(sender)){
            sendResponseCode(socket_fd,250);
            recvMessage(socket_fd,message);
            if(strcmp(strtok(message,delim),"RCPT TO ")==0){
                strcpy(recvr,strtok(NULL,delim));
                printf("----SENDER: %s----\n",sender);
                printf("----RECVR: %s-----\n",recvr);
                if(userExists(recvr)){
                sendResponseCode(socket_fd,250);
                return ;
                }
                sendResponseCode(socket_fd,553);
                close(socket_fd);
                return ;
            }
            }
            sendResponseCode(socket_fd,553);
            close(socket_fd);
            return ;
        }
    }
    sendResponseCode(socket_fd,501);
    close(socket_fd);
    return ;
}

void recvData(char message[100][1000],int socket_fd){
    int i=0;
    time_t currentTime;
    time(&currentTime);
    recvMessage(socket_fd,message[i]);
    sendResponseCode(socket_fd,200);
    ++i;
    strcpy(message[i],"Received: ");
    strcat(message[i],ctime(&currentTime));
    while(strcmp(message[i],".")!=0){
        //printf("Message: %s\n",message[i]);
        ++i;
        recvMessage(socket_fd,message[i]);
        sendResponseCode(socket_fd,200);
    }
    sendResponseCode(socket_fd,250);
}

void storeInDir(char message[100][1000],char sender[100],char recvr[100]){
    printf("----------STORING THE MAIL IN THE RECIEPIENT MAIL BOX----------\n");
    char from[1000]  = "From: ";
    strcat(from,sender);
    strcat(from,"\n");
    char to[1000] = "To: ";
    strcat(to,recvr);
    strcat(to,"\n");
    char path[1000]="./";
    strcat(path,strtok(recvr,"@"));
    strcat(path,"/");
    strcat(path,"mymailbox.mail");
    printf("Path: %s\n",path);
    FILE *fptr = fopen(path,"a");
    green();
    printf("THE MAIL: \n");
    blue;
    fwrite(from,1,strlen(from)+1,fptr);
    printf("%s",from);
    fwrite(to,1,strlen(to)+1,fptr);
    printf("%s",to);
    int i = 0;
    while(strcmp(message[i],".")!=0){
        strcat(message[i],"\n");
        printf("%s",message[i]);
        fwrite(message[i],1,strlen(message[i])+1,fptr);
        ++i;
    }
    strcat(message[i],"\n");
    fwrite(message[i],1,strlen(message[i])+1,fptr);
    printf("------MAIL STORED SUCCESSFULLY----------\n");
    fclose(fptr);
}

int checkUser(int socket_fd){
    char username[100], password1[100],password2[100],command[1000],name[100];
    bzero(name,100);
    bzero(username,100);
    bzero(password1,100);
    bzero(password2,100);
    recvMessage(socket_fd,name);
    FILE *fptr = fopen("logincred.txt","r");
    printf("file opened\n");
    while(fscanf(fptr, "%[^\n]", command) != EOF){
        fgetc(fptr);
        printf("%s\n",command);
        const char delim2[2] = ",";
        strcpy(username ,strtok(command,delim2));
        strcpy(password1,strtok(NULL,delim2));
        if((strcmp(username,name)==0)){
            sendResponseCode(socket_fd,251);
            recvMessage(socket_fd,password2);
            printf("%s\n",username);
            if(strcmp(password1,password2)!=0){
                sendResponseCode(socket_fd,555);
                fclose(fptr);
                close(socket_fd);
                return 0;
             } else{
                printf("%s\n",password2);
                sendResponseCode(socket_fd,220);
                return 1;
               }
        } 
    }
    sendResponseCode(socket_fd,554);
    fclose(fptr);
    close(socket_fd);
    return 0;
}

int main(int argc, char *argv[]){
  int socket_fd;
  struct sockaddr_in my_addr;
  char client_message[100],message[100][1000],sender[100],recvr[100],command[100];
  MYPORT = atoi(argv[1]);
  socket_fd = createSocket();
  my_addr = bindToPort(socket_fd);
  blue();
  printf("listening to incoming requests...\n");
  while(1){
    listen(socket_fd,BACKLOG);
  int client_fd = acceptRequests(socket_fd);
  while(1){
       bzero(client_message,100);
     for(int i=0;i<50;++i)
        bzero(message[i],1000);
     bzero(sender,100);
    bzero(recvr,100);
        bzero(command,100);
      recvMessage(client_fd,command);
      if(strcmp(command,"Auth")==0){
          printf("Checking the auth credentials\n");
          if(checkUser(client_fd)){
              continue;
          }else{
              break;
          }
      }else
      if(strcmp(command,"Send Mail")==0){
  printf("----------SMTP handshake starting----------\n");
  smtpHandShake(client_fd,sender,recvr);
  printf("----------SMTP handshake over----------\n");
  recvMessage(client_fd,client_message);
  if(strcmp(client_message,"DATA")==0){
      sendResponseCode(client_fd,250);
      printf("---------------DATA RECIEVING---------\n");
      recvData(message,client_fd);
      printf("----------------DATA RECIEVING OVER------------\n");
      printf("--------------STORING THE DATA--------------\n");
      storeInDir(message,sender,recvr);
      printf("--------------OVER------------------\n");
  }
      }
      else if(strcmp(command,"QUIT")==0){
          sendResponseCode(client_fd,221);
         close(client_fd);
         break;
      }
  }
  }
}