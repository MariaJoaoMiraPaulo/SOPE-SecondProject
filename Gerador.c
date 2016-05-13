//Maria Joao Mira Paulo e Nuno Ramos
//2º Trabalho SOPE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define FIFO_NAME_LENGTH 10
int id=0;

typedef enum {NORTH, SOUTH, EAST, WEST} Direction;

typedef struct {
  Direction direction;
  int id;
  float parking_time;
  char fifo_name[FIFO_NAME_LENGTH] ;

}Vehicles;

Direction get_car_direction(){

  int r = rand() % 4;

  if(r==0)
    return NORTH;
  else if(r==1)
    return SOUTH;
  else if(r==2)
    return EAST;
  else
    return WEST;
}

int get_tick_for_next_car(){
  int r = rand() % 10;
  int ticks_for_next_car;
  if(r<5)
    ticks_for_next_car=0;
  else if(r<8)
    ticks_for_next_car=1;
  else ticks_for_next_car=2;

  return ticks_for_next_car;
}

int generate_car(){
//no final da funcao gerar a probabilidade
  Vehicles vehicle;
  vehicle.direction=get_car_direction();
  vehicle.id=id;
  id++;
  sprintf(vehicle.fifo_name,"%s%d","fifo",id);

  //Create thread Vehicle

return get_tick_for_next_car();

}

int main(int argc, char* argv[]){

  srand(time(NULL));
  int time_generation=atoi(argv[1]),u_clock=atoi(argv[2]);
  int ticks_for_next_car=0;

  if(argc != 3){
    perror("Invalid number of arguments.\n");
  }

  do{
    if(ticks_for_next_car == 0)
      ticks_for_next_car=generate_car();
    else ticks_for_next_car--;
    //suspends execution of the calling thread for (at least) u_clock*10^3 microseconds.
    usleep(u_clock*pow(10,3));
  time_generation--;
  }while(time_generation>0);

  return 0;
}
