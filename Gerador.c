//Maria Joao Mira Paulo e Nuno Ramos
//2º Trabalho SOPE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){

  int time_generation=atoi(argv[1]),u_clock=atoi(argv[2]);

  if(argc != 3){
    perror("Invalid number of arguments.\n");
  }

  do{
    //generateCar();
  time_generation--;
  }while(time_generation>0);

  return 0;
}
