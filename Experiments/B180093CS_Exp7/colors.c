#include<stdio.h>

void red () {
  printf("\033[1;31m");
}

void green(){
  printf("\033[0;36m");
}

void blue(){
  printf("\033[0;34m");
}

void yellow(){
    printf("\033[0;33m");
}

void white(){
  printf("\033[0;37m");
}

void reset(){
  printf("\033[0m");
}
