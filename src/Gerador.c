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

#include "Utilities.h"

#define GERADOR_FILE_NAME "gerador.log"
#define DEST_MAX_LENGTH 10


int id=0;
int fd_gerador_log;
float number_ticks=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void write_to_log_file(Vehicle *vehicle, int state){
  char buffer[FILE_LINE_MAX_LENGTH];
  char dest[DEST_MAX_LENGTH];
  char status[STATUS_MAX_LENGTH];

  switch (vehicle->direction){
    case NORTH:
    strcpy(dest, "N");
    break;
    case SOUTH:
    strcpy(dest, "S");
    break;
    case EAST:
    strcpy(dest, "E");
    break;
    case WEST:
    strcpy(dest, "O");
    break;
  }

  switch(state){
    case VEHICLE_IN:
    strcpy(status, "entrada");
    break;
    case PARK_FULL:
    strcpy(status, "cheio");
    break;
    case PARK_CLOSED:
    strcpy(status, "fechado");
    break;
    case VEHICLE_OUT:
    strcpy(status, "saida");
    break;
  }
  if (state==VEHICLE_OUT)
    sprintf(buffer, "%-8d ; %7d ;    %s   ; %10d ; %6d ; %s\n",(int)(vehicle->initial_ticks+vehicle->parking_time_tikes), vehicle->id, dest, (int)vehicle->parking_time,(int)(number_ticks-vehicle->initial_ticks),status);
  else
    sprintf(buffer, "%-8d ; %7d ;    %s   ; %10d ;      ? ; %s\n",vehicle->initial_ticks, vehicle->id, dest, (int)vehicle->parking_time, status);

  write(fd_gerador_log,buffer,strlen(buffer));

  strcpy(buffer, "");

}

//Function that the thread with tid executes when is created
void* func_vehicle(void* arg){
  Vehicle vehicle= *(Vehicle*) arg;
  void* ret=NULL;
  int fd_read, fd_write;
  int state=0;


  sem_t *semaphore = sem_open(CONST_CHAR_NAME_SEMAPHORE, O_CREAT ,PERMISSONS,1);

  mkfifo(vehicle.fifo_name, PERMISSONS);

  sem_wait(semaphore);
  switch (vehicle.direction){
    case NORTH:
    fd_write = open(FIFO_N, O_WRONLY | O_NONBLOCK);
    break;
    case SOUTH:
    fd_write = open(FIFO_S, O_WRONLY | O_NONBLOCK);
    break;
    case EAST:
    fd_write = open(FIFO_E, O_WRONLY | O_NONBLOCK);
    break;
    case WEST:
    fd_write = open(FIFO_W, O_WRONLY | O_NONBLOCK);
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

  //To avoid that the thread blocks when park is close
  if(fd_write != -1){
    write(fd_write, &vehicle, sizeof(Vehicle));
    close(fd_write);

    printf("Passei o write\n");
    sem_post(semaphore);
    sem_close(semaphore);

    fd_read = open(vehicle.fifo_name, O_RDONLY);
    if(fd_read != -1){
      printf("Vou ler ID: %d\n", vehicle.id);
      read(fd_read,&state,sizeof(int));
      pthread_mutex_lock(&mutex);
      write_to_log_file(&vehicle, state);
      pthread_mutex_unlock(&mutex);
      printf("Recebi informação: %d, ID : %d\n",state, vehicle.id);
      read(fd_read,&state,sizeof(int));
    }
    else printf("O parque esta fechado ID %d\n", vehicle.id);
  }
  else {
    printf("O parque ainda esta fechado\n");
    sem_post(semaphore);
    sem_close(semaphore);
    state = 2;
  }
  //CRASHA
  //free(&vehicle);

  unlink(vehicle.fifo_name);

  pthread_mutex_lock(&mutex);
  write_to_log_file(&vehicle, state);
  pthread_mutex_unlock(&mutex);

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

int generate_car(float u_clock, int ticks){

  pthread_t tid;
  //no final da funcao gerar a probabilidade
  Vehicle *vehicle = (Vehicle*)malloc(sizeof(Vehicle));

  vehicle->initial_ticks = ticks;
  printf("ticks : %d\n", vehicle->initial_ticks);

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
  vehicle->parking_time_tikes=vehicle->parking_time/u_clock;
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

  //Reseting File Gerador.log
  //fopen creates an empty file for writing.
  //If a file with the same name already exists, its content is erased and the file is considered as a new empty file.
  FILE* file1 = fopen(GERADOR_FILE_NAME,"w");
  fclose(file1);

  if(argc != 3){
    perror("Invalid number of arguments.\n\n");
  }

  printf("total_number_ticks%f\n",total_number_ticks );

  fd_gerador_log = open(GERADOR_FILE_NAME, O_WRONLY | O_CREAT  , PERMISSONS);

  char buffer[] = "t(ticks) ; id_viat ; destin ; t_estacion ; t_vida ; observ\n";

  write(fd_gerador_log,buffer,strlen(buffer));

  do{
    if(ticks_for_next_car == 0)
    //Generate one car
    ticks_for_next_car=generate_car(u_clock, number_ticks);
    else ticks_for_next_car--;
    //suspends execution of the calling thread for (at least) u_clock*10^3 microseconds.
    usleep(u_clock*pow(10,3));
    number_ticks++;
  }while(total_number_ticks!=number_ticks);

  pthread_exit(NULL);
  //return 0;
}
