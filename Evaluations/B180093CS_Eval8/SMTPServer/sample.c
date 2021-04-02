#include<stdio.h>

void main(){
	char path[100]="./stallings/mymailbox.mail";
	FILE *fptr=fopen(path,"r");
	char temp[100];
	while(fgets(temp,100,fptr)!=NULL){
		fgetc(fptr);
		printf("%s",temp);
	}
}

