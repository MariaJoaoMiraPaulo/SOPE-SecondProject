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

float get_car_parking_time(float u_clock){

  float r;
  return r = ((rand() % 10)+1)*u_clock;
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

int generate_car(float u_clock){
//no final da funcao gerar a probabilidade
  Vehicles vehicle;
  vehicle.direction=get_car_direction();
  switch (vehicle.direction){
    case NORTH:
      printf("Direction North \n");
      break;
    case SOUTH:
        printf("Direction South \n");
        break;
    case EAST:
        printf("Direction East \n");
        break;
    case WEST:
        printf("Direction West \n");
        break;
  }
  vehicle.id=id;
  printf("ID %d\n",id);
  vehicle.parking_time=get_car_parking_time(u_clock);
  printf("Parking time %f\n",vehicle.parking_time);
  id++;
  sprintf(vehicle.fifo_name,"%s%d","fifo",id);
  printf("Fifo Name %s\n",vehicle.fifo_name);

  //Create thread Vehicle

return get_tick_for_next_car();

}

int main(int argc, char* argv[]){

  srand(time(NULL));
  float time_generation=atoi(argv[1]),u_clock=atoi(argv[2]);
  float total_number_ticks=(time_generation*pow(10,3))/u_clock;
  int ticks_for_next_car=0;

  if(argc != 3){
    perror("Invalid number of arguments.\n\n");
  }

  printf("total_number_ticks%f\n",total_number_ticks );
  do{
    if(ticks_for_next_car == 0)
      ticks_for_next_car=generate_car(u_clock);
    else ticks_for_next_car--;
    //suspends execution of the calling thread for (at least) u_clock*10^3 microseconds.
    usleep(u_clock*pow(10,3));
    total_number_ticks--;
  }while(total_number_ticks>0);

  return 0;
}
