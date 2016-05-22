//Maria Joao Mira Paulo e Nuno Ramos
//2º Trabalho SOPE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> // For O_* constants
#include <semaphore.h>

#define FIFO_NAME_LENGTH 10
#define OK 0

int id=0;

//Direction enums are the four cardinal points of access to the park
typedef enum {NORTH, SOUTH, EAST, WEST} Direction;

typedef struct {
  Direction direction;
  int id;
  float parking_time;
  char fifo_name[FIFO_NAME_LENGTH] ;

}Vehicle;

//Function that the thread with tid executes when is created
void* func_vehicle(void* arg){
  Vehicle vehicle= *(Vehicle*) arg;
  void* ret=NULL;
  int fd_read, fd_write;
  int state;

  char name[]="/sem";
  sem_t *semaphore = sem_open(name, O_CREAT ,0660,1);

  mkfifo(vehicle.fifo_name, 0660);

  sem_wait(semaphore);
  switch (vehicle.direction){
    case NORTH:
    fd_write = open("fifoN", O_WRONLY | O_NONBLOCK);
    break;
    case SOUTH:
    fd_write = open("fifoS", O_WRONLY | O_NONBLOCK);
    break;
    case EAST:
    fd_write = open("fifoE", O_WRONLY | O_NONBLOCK);
    break;
    case WEST:
    fd_write = open("fifoW", O_WRONLY | O_NONBLOCK);
    break;
  }

  printf("Cheguei à thread!! \n");
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

  /*printf("ID %d\n",vehicle.id);
  printf("Parking time %f\n",vehicle.parking_time);
  printf("Fifo Name %s\n",vehicle.fifo_name);*/
  printf("Aqui\n");
  if(fd_write != -1){
    write(fd_write, &vehicle, sizeof(Vehicle));
    close(fd_write);
  }
  printf("Passei o write\n");
  sem_post(semaphore);
  sem_close(semaphore);

  fd_read = open(vehicle.fifo_name, O_RDONLY | O_NONBLOCK);
  if(fd_read != -1){
    printf("Vou ler ID: %d\n", vehicle.id);
    read(fd_read,&state,sizeof(int));
    printf("Recebi informação: %d, ID : %d\n",state, vehicle.id);
  }
  else printf("O parque esta fechado ID %d\n", vehicle.id);

  //CRASHA
  //free(&vehicle);

  unlink(vehicle.fifo_name);

  return ret;
}

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
#include <semaphore.h>
int get_tick_for_next_car(){
  int r = rand() % 10;
  int ticks_for_next_car;
  //50% probability
  if(r<5)
  ticks_for_next_car=0;
  //30% probability
  else if(r<8)
  ticks_for_next_car=1;
  //20% probability
  else ticks_for_next_car=2;

  return ticks_for_next_car;
}

int generate_car(float u_clock){

  pthread_t tid;
  //no final da funcao gerar a probabilidade
  Vehicle *vehicle = (Vehicle*)malloc(sizeof(Vehicle));

  vehicle->direction=get_car_direction();
  switch (vehicle->direction){
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
  vehicle->id=id;
  printf("ID %d\n",vehicle->id);
  vehicle->parking_time=get_car_parking_time(u_clock);
  printf("Parking time %f\n",vehicle->parking_time);
  id++;
  sprintf(vehicle->fifo_name,"%s%d","fifo",id);
  printf("Fifo Name %s\n",vehicle->fifo_name);

  //Create thread Vehicle
  if(pthread_create(&tid,NULL,func_vehicle,vehicle) != OK){
    perror("Gerador::Error on creating thread\n");
  }

  return get_tick_for_next_car();

}

int main(int argc, char* argv[]){

  srand(time(NULL));
  float time_generation=atoi(argv[1]);
  float u_clock=atoi(argv[2]);
  float total_number_ticks=(time_generation*pow(10,3))/u_clock;
  int ticks_for_next_car=0;

  if(argc != 3){
    perror("Invalid number of arguments.\n\n");
  }

  printf("total_number_ticks%f\n",total_number_ticks );

  do{
    if(ticks_for_next_car == 0)
    //Generate one car
    ticks_for_next_car=generate_car(u_clock);
    else ticks_for_next_car--;
    //suspends execution of the calling thread for (at least) u_clock*10^3 microseconds.
    usleep(u_clock*pow(10,3));
    total_number_ticks--;
  }while(total_number_ticks>0);

  pthread_exit(NULL);
  //return 0;
}
