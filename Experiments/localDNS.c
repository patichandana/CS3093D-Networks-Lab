#include<arpa/inet.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<time.h>
#include<unistd.h>
#include<netinet/in.h>
#include <netdb.h>
#include<arpa/nameser.h>
#include<resolv.h>
#include <errno.h>
#include "../colors.c"

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
  socket_fd = socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP);
  checkError(socket_fd,"socket");
  green();
  printf("socket created at socket number: %d\n",socket_fd);
  return socket_fd;
}

struct sockaddr_in bindToPort(int socket_fd){
  struct sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(MYPORT);
  my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(my_addr.sin_zero,'\0',sizeof(my_addr.sin_zero));
  int integer = bind(socket_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
  checkError(integer,"bind");
  green();
  printf("binded the socket to a port...\n");
  return my_addr;
}

void splitString(char strings[50][50],char domain[100],char delim[10],int *num){
    char *temp;
    *num = 0;
    temp=strtok(domain,delim);
    while(temp!=NULL){
       strcpy(strings[*num],temp);
       ++(*num);
      temp=strtok(NULL,delim);
    }
    for(int i=(*num)-2;i>=0;--i){
      strcat(strings[i],".");
      strcat(strings[i],strings[i+1]);
    }
}

void FetchFirstNSentry(char *nameServer,char file[100],char *domain){
  char token[1000];
  char delim[10] = " = ";
  FILE *fptr = fopen(file,"r");
  printf("-----------------------------\n");
  //printf("DOMAIN: %s",domain);
  while(fscanf(fptr,"%[^\n]",token)!=EOF){
    fgetc(fptr);
    char temp[50];
    if(strncmp(token,domain,strlen(domain))==0){
      printf("TOKEN: %s\n",token);
      printf("%s\n",strtok(token,delim));
      strcpy(nameServer,strtok(NULL,delim));
      printf("%s\n",nameServer);
      system("rm output.txt");
      printf("-----------------------------\n");
      fclose(fptr);
      return ;
    }
    }
}

void sendRequest(char DomainName[100],char type[5],char server[100],char *nameServer,int finalQuery){
  char command[300];
  strcpy(command,"nslookup -type=");
  strcat(command,type);
  strcat(command, DomainName);
  if(server!=NULL){
    strcat(command,"  ");
    strcat(command,server);
    printf("SENDING REQUEST TO SERVER: %s\n",server);
  }
  strcat(command," >>output.txt");
  printf("DOMAIN NAME: %s\n",DomainName);
  printf("COMMAND: %s\n",command);
  system(command);
  if(finalQuery == 0)
    FetchFirstNSentry(nameServer,"output.txt",DomainName);
}

void SendQueries(char strings[50][50],char domain[100],char type[20],int num){
  int i=num-1;
  char temp[50],server[100],nameServer[100];
  sendRequest(".","ns ",NULL,nameServer,0);
  while(i>=0){
    char temp[50];
    strcpy(temp,strings[i]);
    printf("DOMAIN NOW: %s\n",temp);
    sendRequest(temp,"ns ",nameServer,nameServer,0);
    --i;
  }
  strcat(type," ");
  sendRequest(domain,type,nameServer,nameServer,1);
}

int CheckCache(char DNSQuery[100],int socket_fd,struct sockaddr_in client_addr){
  int len=sizeof(client_addr);
  char message[100];
  char newline[10]="\n";
  char old[100];
  FILE *fptr=fopen("cache.txt","r");
  while(fscanf(fptr,"%[^\n]",message)!=EOF){
    fgetc(fptr);
    strcat(message,"\n");
    if(strcmp(message,DNSQuery)==0){
        while(fscanf(fptr,"%[^\n]",message)!=EOF){
          fgetc(fptr);
          if(strcmp(message,".")==0){
            fclose(fptr);
            return 1;
          }
          if(strcmp(message,old)!=0)
            sendto(socket_fd, (const char *)message, strlen(message), MSG_CONFIRM, (const struct sockaddr *)&client_addr,len);
          sendto(socket_fd, (const char *)newline, strlen(newline), MSG_CONFIRM, (const struct sockaddr *)&client_addr,len);
          strcpy(old,message);
        }
    }
  }
  fclose(fptr);
  fptr=fopen("cache.txt","a");
  printf("Writing query to the cache file\n");
  printf("%s\n",DNSQuery);
  fprintf(fptr,"%s",DNSQuery);
  //fwrite(DNSQuery , 1 , 100,fptr);
  fclose(fptr);
  return 0;
}

void main(int argc, char *argv[]){
    char domain[100],strings[50][50],DNSQuery[1000],temp[1000],type[100],*dummy,message[100];
    int socket_fd,num;
    struct sockaddr_in servaddr,client_addr  ;
    int len = sizeof(client_addr);
    socklen_t addrlen;
    MYPORT = atoi(argv[1]);
    socket_fd = createSocket();
    servaddr = bindToPort(socket_fd);
    printf("listening to incoming requests...\n");
    while(1){
      listen(socket_fd,BACKLOG);
      recvfrom(socket_fd, (char *)DNSQuery,1000, MSG_WAITALL, ( struct sockaddr *) &client_addr,&len);
      printf("Checking Cache..\n");
      if(CheckCache(DNSQuery,socket_fd,client_addr)){
        printf("Cache hit\n");
        continue;
      }
      strcpy(temp,DNSQuery);
      char delim[20]="=";
      dummy=strtok(temp,delim);
      dummy=strtok(NULL,delim);
      strcpy(type,strtok(NULL," "));
      printf("%s\n",type);
      strcpy(domain,strtok(NULL," "));
      printf("%s\n",domain);
      strcpy(temp,domain);
      splitString(strings,temp,".",&num);
      SendQueries(strings,domain,type,num);
      FILE *fptr=fopen("output.txt","r");
      char newline[10]="\n";
      char old[100];
      FILE *fptr2 = fopen("cache.txt","a");
      while(fscanf(fptr,"%[^\n]",message)!=EOF){
      fgetc(fptr);
      if(strcmp(message,old)!=0){
        sendto(socket_fd, (const char *)message, strlen(message), MSG_CONFIRM, (const struct sockaddr *)&client_addr,len);
        fprintf(fptr2,"%s",message);
        //fwrite(message , 1 , sizeof(message),fptr2);
      }
      sendto(socket_fd, (const char *)newline, strlen(newline), MSG_CONFIRM, (const struct sockaddr *)&client_addr,len);
      fprintf(fptr2,"%s",newline);
      //fwrite(newline , 1 , sizeof(newline),fptr2);
      strcpy(old,message);
    }
    fclose(fptr);
    char stop[20]=".\n";
    fprintf(fptr2,"%s",stop);
    fclose(fptr2);
    system("rm output.txt");
    }
}