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
float number_ticks=0; //Atual number of ticks of the program
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Write to the file named "gerador.log" all the vehicles' information
void write_to_log_file(Vehicle *vehicle, int state){
  int ret;
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
  //If the Vehicle could enter the park and now is going out, then it is necessary to print information like t_vida
  if (state==VEHICLE_OUT)
  sprintf(buffer, "%-8d ; %7d ;    %s   ; %10d ; %6d ; %s\n",(int)(vehicle->initial_ticks+vehicle->parking_time_ticks), vehicle->id, dest, (int)vehicle->parking_time,(int)(number_ticks-vehicle->initial_ticks),status);
  else //If the vehicle's state is VEHICLE_IN/PARK_FULL/PARK_CLOSED that means the vehicle doesn't have t_vida.
  sprintf(buffer, "%-8d ; %7d ;    %s   ; %10d ;      ? ; %s\n",vehicle->initial_ticks, vehicle->id, dest, (int)vehicle->parking_time, status);

  //Writes the string Buffer to gerador.log with filedes "fd_gerador_log"
  ret = write(fd_gerador_log,buffer,strlen(buffer));
  if(ret == OK){
    perror("Error on writing to gerador log file ");
  }

  strcpy(buffer, "");

}

//Function that the thread with pthread_t tid executes when is created
//Receives one vehicle
void* func_vehicle(void* arg){
  Vehicle vehicle= *(Vehicle*) arg;
  int fd_read, fd_write;
  int state=0;
  int ret_mkfifo, ret_sem_wait, ret_write, ret_sem_post, ret_unlink;


  sem_t *semaphore = sem_open(CONST_CHAR_NAME_SEMAPHORE, O_CREAT ,PERMISSONS,1);
  if(semaphore == SEM_FAILED){  //testing for some error on opening the semaphore
    perror("Error on opening semaphore ");
  }

  //Creates her own Fifo
  ret_mkfifo = mkfifo(vehicle.fifo_name, PERMISSONS);
  if(ret_mkfifo != OK){
    perror("Error on opening vehicle fifo ");
  }

  ret_sem_wait = sem_wait(semaphore);
  if(ret_sem_wait != OK){
    perror("Error on doing sem_wait ");
  }
  switch (vehicle.direction){  //Opens the respective controller (depending to the direction loaded on the struct vehicle) to send the vehicle information to Park
    case NORTH:
    fd_write = open(FIFO_N, O_WRONLY | O_NONBLOCK); //if some process opened the FIFO for reading, returns the FIFO's descriptor if not, returns -1 (Error and FIFO could not be opened), which means the park is still closed
    break;
    case SOUTH:
    fd_write = open(FIFO_S, O_WRONLY | O_NONBLOCK); //if some process opened the FIFO for reading, returns the FIFO's descriptor if not, returns -1 (Error and FIFO could not be opened), which means the park is still closed
    break;
    case EAST:
    fd_write = open(FIFO_E, O_WRONLY | O_NONBLOCK); //if some process opened the FIFO for reading, returns the FIFO's descriptor if not, returns -1 (Error and FIFO could not be opened), which means the park is still closed
    break;
    case WEST:
    fd_write = open(FIFO_W, O_WRONLY | O_NONBLOCK); //if some process opened the FIFO for reading, returns the FIFO's descriptor if not, returns -1 (Error and FIFO could not be opened), which means the park is still closed
    break;
    default:
    break;
  }


  //Verify the value of fd_write, to avoid the blocking when Park is still closed
  if(fd_write != ERROR){
    ret_write = write(fd_write, &vehicle, sizeof(Vehicle)); //send Vehicle to Park's controller
    if(ret_write == OK){
      perror("Error on writing to controller fifo ");
    }
    close(fd_write);

    ret_sem_post =sem_post(semaphore);
    if(ret_sem_post != OK){
      perror("Error on doing sem_post to the semaphore ");
    }
    sem_close(semaphore);

    fd_read = open(vehicle.fifo_name, O_RDONLY); //Blocks until some other process opens the same FIFO for writing
    if(fd_read != ERROR){
      //Receives state value when the car enters the park
      read(fd_read,&state,sizeof(int));

      pthread_mutex_lock(&mutex);//When the lock is set, no other thread can access the locked region of code. It is important to write to file only one by one to avoid lost of information
      write_to_log_file(&vehicle, state);//writes to file gerador.log vehicle's state
      pthread_mutex_unlock(&mutex);//Releases the lock and allows others thread to write to the file

      //Receives state value when the car exits the park
      read(fd_read,&state,sizeof(int));
    }
  }
  else {//The Park is still closed!! Can't receive vehicles

  ret_sem_post = sem_post(semaphore);
  if(ret_sem_post != OK){
    perror("Error on doing sem_post to the semaphore ");
  }
  sem_close(semaphore);

  //When Park is closed, state have value of 2
  state = PARK_CLOSED;
}

ret_unlink = unlink(vehicle.fifo_name); //deletes a the vehicle's fifo from the file system
if(ret_unlink != OK){
  perror("Error on unlinking the vehicle fifo ");
}

pthread_mutex_lock(&mutex); //When the lock is set, no other thread can access the locked region of code. It is important to write to file only one by one to avoid lost of information
write_to_log_file(&vehicle, state); //writes to file gerador.log vehicle's information
pthread_mutex_unlock(&mutex);//Releases the lock and allow others thread to write to the file


pthread_exit(0);
}

Direction get_car_direction(){ //Generates a random direction
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

float get_car_parking_time(float u_clock){ //Generates a random parking_time

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

int generate_car(float u_clock, int ticks){ //Generates a car

  pthread_t tid;
  Vehicle *vehicle = (Vehicle*)malloc(sizeof(Vehicle));

  vehicle->initial_ticks = ticks;
  vehicle->direction=get_car_direction();
  vehicle->id=id;
  vehicle->parking_time=get_car_parking_time(u_clock);
  vehicle->parking_time_ticks=vehicle->parking_time/u_clock;
  id++;
  sprintf(vehicle->fifo_name,"%s%d",VEHICLE_FIFO_NAME,id);

  //Create thread Vehicle
  if(pthread_create(&tid,NULL,func_vehicle,vehicle) != OK){
    perror("Gerador::Error on creating thread\n");
  }

  return get_tick_for_next_car(); //Generates the new probability of creating a new Vehicle

}

void preparing_log_file(){

  int ret_write;

  //Reseting File Gerador.log
  //fopen creates an empty file for writing.
  //If a file with the same name already exists, its content is erased and the file is considered as a new empty file.
  FILE* file1 = fopen(GERADOR_FILE_NAME,"w");
  if(file1 == NULL){
    perror("Error on opening gerador file log ");
  }
  fclose(file1);

  fd_gerador_log = open(GERADOR_FILE_NAME, O_WRONLY | O_CREAT,PERMISSONS);
  if(fd_gerador_log < OK){
    perror("Error on opening gerador file log ");
  }

  char buffer[] = "t(ticks) ; id_viat ; destin ; t_estacion ; t_vida ; observ\n";

  ret_write = write(fd_gerador_log,buffer,strlen(buffer));
  if(ret_write == OK){
    perror("Error on writing to gerador file log ");
  }

}

int main(int argc, char* argv[]){

  srand(time(NULL));
  float time_generation=atoi(argv[1]);
  float u_clock=atoi(argv[2]);
  float total_number_ticks=(time_generation*pow(10,3))/u_clock;
  int ticks_for_next_car=0;

  if(argc != 3){
    perror("Invalid number of arguments. Should be used in this format:\ngerador <generation_time> <update_rate>\n");
  }

  preparing_log_file(); //Prepares gerador.log file

  do{
    if(ticks_for_next_car == 0)
      ticks_for_next_car=generate_car(u_clock, number_ticks);  //Generates one car
    else ticks_for_next_car--;
    usleep(u_clock*pow(10,3));//suspends execution of the calling thread for (at least) u_clock*10^3 microseconds.
    number_ticks++;
  }while(total_number_ticks!=number_ticks);

  pthread_exit(NULL);
}
