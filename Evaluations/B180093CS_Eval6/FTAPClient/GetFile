#include<arpa/inet.h>
#include "colors.c"
#include <dirent.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>

#define MYPORT 2021
#define BACKLOG 10
#define SIZE 500

char username[500], password[500];

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

void replyMessage(char *message,int socket_fd){
  int value = send(socket_fd,message,strlen(message)+1,0);
  white();
  printf("message sent to client...\n");
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
		if(fprintf(fp,"%s",message)){
      printf("YESSSsss\n");
    }
		printf("%s",message);
		bzero(message, SIZE);
	}
}

int checkUser(char *username){
  printf("Checking USER.....\n");
  const char s[2] = ",";
  char command[500],*name;
  FILE *fptr = fopen("logincred.txt","r");
  printf("file opened\n");
  int n=0;
  while(fscanf(fptr, "%[^\n]", command) != EOF&&n<4){
    fgetc(fptr);
    printf("%s\n",command);
    name = strtok(command, s);
    ++n;
    if(strcmp(username,name)==0){
      strcpy(password,strtok(NULL,s));
     return 1;
   } 
  }
  return 0;
}

int checkUserPassword(char *password1){
  printf("Password: %s, Password1: %s\n",password,password1);
  if(strcmp(password,password1)==0){
    return 1;
  } return 0;
  
}

void sendListFiles(int socket_fd){
  struct dirent *de;  // Pointer for directory entry 
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir("."); 
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        return ;
    } 
    while ((de = readdir(dr)) != NULL){ 
            printf("%s\n", de->d_name); 
            replyMessage(de->d_name,socket_fd);
    }
    closedir(dr);     
    char over[100] = "OVER";
    replyMessage(over,socket_fd);
}

void recvMessage(int socket_fd){
  char client_message[500];
  recv(socket_fd,client_message,sizeof(client_message),0);
  yellow();
  printf("client message: %s\n",client_message);
  const char s[2] = " ";
  char command[500],name[500];
   strcpy(name, strtok(client_message, s));
   strcpy(command,name);
  if(strcmp(command,"USERN") == 0){
    strcpy(name ,strtok(NULL, s));
      printf("%s\n %s\n",command,name);
      if(checkUser(name)){
        replyMessage("300",socket_fd);
        strcpy(username,name);
      }else{
        replyMessage("301",socket_fd);
      }
  }   
    else if(strcmp(command, "QUIT")==0){
	    close(socket_fd);
	    printf("client connection ended ...\n");
	    exit(0);
    }else if(strcmp(command,"PASSWD")==0){
      strcpy(name ,strtok(NULL, s));
      if(checkUserPassword(name)){
        replyMessage("305",socket_fd);
      }else{
        replyMessage("310",socket_fd);
      }
    } else if(strcmp(command,"CreateFile")==0){
      strcpy(name ,strtok(NULL, s));
      printf("File name: %s\n",name);
      char path[500]="./";
      strcat(path,name);
      FILE *fptr1 = fopen(path,"w");
      if(fptr1 == NULL){
        replyMessage("0",socket_fd);
        return ;
      }
      replyMessage("100",socket_fd);
      printf("File created...\n");
      fclose(fptr1);
      return ;
    } else if(strcmp(command,"ListDir")==0){
      printf("sending the list of dir\n");
      sendListFiles(socket_fd);
      replyMessage("100",socket_fd);
    } else if(strcmp(command,"StoreFile")==0){
      strcpy(name ,strtok(NULL, s));
      recvFile(socket_fd,name);
      replyMessage("100",socket_fd);
    } else if(strcmp(command,"GetFile")==0){
      strcpy(name ,strtok(NULL, s));
      sendFile(socket_fd,name);
      replyMessage("100",socket_fd);
    } 
   return ; 
  
}

int main(){
  char message[500];
  int socket_fd;
  struct sockaddr_in my_addr;
  socket_fd = createSocket();
  my_addr = bindToPort(socket_fd);
  blue();
  printf("listening to incoming requests...\n");
  listen(socket_fd,BACKLOG);
  int client_fd = acceptRequests(socket_fd);
  while(1){
    recvMessage(client_fd);
  }
  close(socket_fd);
  red();
  printf("connection closed...");
}
