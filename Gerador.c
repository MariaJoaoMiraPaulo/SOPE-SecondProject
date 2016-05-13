//Maria Joao Mira Paulo e Nuno Ramos
//2º Trabalho SOPE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char* argv[]){

  int time_generation=atoi(argv[1]),u_clock=atoi(argv[2]);

  if(argc != 3){
    perror("Invalid number of arguments.\n");
  }

  do{
    printf("Vou adormecer\n");
    //suspends execution of the calling thread for (at least) u_clock*10^3 microseconds.
    usleep(u_clock*pow(10,3));
    printf("Vou acordar\n");
    //generateCar();
  time_generation--;
  }while(time_generation>0);

  return 0;
}
