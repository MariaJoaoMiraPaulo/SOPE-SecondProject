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

#define PARQUE_FILE_NAME "parque.log"
#define LAST_VEHICLE_ID -1


int fd_parque_log;
int park_close; //Park_close variable indicates park's state (0 means is open, 1 means is closed)
int park_capacity; //total number of car-parking spaces on park
int unavailable_space; //number of unavailable car-parking spaces on park
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

//Write to the file named "parque.log" all the vehicles'information
void write_to_log_file(Vehicle *vehicle, int state){
  char buffer[FILE_LINE_MAX_LENGTH];
  char status[STATUS_MAX_LENGTH];
  int ret_write;

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
    if(park_close == 1)
    strcpy(status, "encerrado");
    else
    strcpy(status, "saida");
    break;
    case PARKING:
    strcpy(status, "estacionamento");
    break;

  }
  if(state==VEHICLE_OUT)
  sprintf(buffer, "%-8d ; %4d ; %7d ; %s\n",(int)(vehicle->initial_ticks+vehicle->parking_time_ticks), (park_capacity - unavailable_space),vehicle->id, status);
  else
  sprintf(buffer, "%-8d ; %4d ; %7d ; %s\n",vehicle->initial_ticks, (park_capacity - unavailable_space),vehicle->id, status);

  ret_write = write(fd_parque_log,buffer,strlen(buffer));
  if(ret_write == OK){
    perror("write_to_log_file::Error on writing to parque log file ");
  }

}

void* vehicle_guide(void* arg){ //Responsible for guide the vehicle and atribute a specific value to the variable state of the vehicle
  Vehicle vehicle= *(Vehicle*) arg;
  int state;
  int fd_write;
  int ret_write;

  fd_write=open(vehicle.fifo_name,O_WRONLY); //Opens the vehicle's own FIFO in Write mode


  pthread_mutex_lock(&mutex); //Mutex that controlls the number of available spaces to park a car. The mutex lock all the others threads when they try to acess to varibles unavailable_space and park_capacity
  //Verifying park state
  if(unavailable_space<park_capacity && !park_close){
    state=VEHICLE_IN;
    unavailable_space++; //Increments the number of unavailable Spaces

    ret_write = write(fd_write,&state,sizeof(int)); //Send the variable state to vehicle's own FIFO on thread Vehicle, on Gerador.c
    if(ret_write == OK){
      perror("vehicle_guide::Error on writing to vehicle fifo ");
    }

    pthread_mutex_lock(&file_mutex);//When the lock is set, no other thread can access the locked region of code. It is important to write to file only one by one to avoid lost of information
    write_to_log_file(&vehicle, PARKING);//writes to file parque.log vehicle's state (only if the vehicle is entering the park)
    pthread_mutex_unlock(&file_mutex);//Releases the lock and allows others thread to write to the file


    pthread_mutex_unlock(&mutex);//Releases the lock and allows others threads to acess to unavailable_space and park_capacity varibles

    //pow(10,3) to convert milliseconds to microseconds
    usleep(vehicle.parking_time*pow(10,3)); //Simulating Vehicle's parking_time
    state=VEHICLE_OUT;
    unavailable_space--;
  }
  else if(park_close){
    pthread_mutex_unlock(&mutex);
    state=PARK_CLOSED;
  }
  else {
    pthread_mutex_unlock(&mutex);
    state=PARK_FULL;
  }
  ret_write = write(fd_write,&state,sizeof(int));
  if(ret_write == OK){
    perror("vehicle_guide::Error on writing to vehicle fifo ");
  }

  pthread_mutex_lock(&file_mutex);//When the lock is set, no other thread can access the locked region of code. It is important to write to file only one by one to avoid lost of information
  write_to_log_file(&vehicle, state);//writes to file parque.log vehicle's state (if the vehicle is exiting the park, the park is closed or full)
  pthread_mutex_unlock(&file_mutex);//Releases the lock and allows others thread to write to the file

  close(fd_write);

  pthread_exit(NULL);
}

//Function that the thread with pthread_t tid_n executes when is created
void* func_north(void* arg){

  int fd_read;
  Vehicle vehicle;
  int ret_read, ret_mkfifo, ret_unlink;
  pthread_t tid_n;

  ret_mkfifo =  mkfifo(FIFO_N, PERMISSONS);
  if(ret_mkfifo != OK){
    perror("func_North::Error on opening north controller fifo ");
  }

  fd_read = open(FIFO_N, O_RDONLY | O_NONBLOCK); //Opens fifo in read mode

  open(FIFO_N, O_WRONLY); //Opens fifo in write mode to avoid busy waiting

  while(1){
    ret_read = read(fd_read, &vehicle, sizeof(Vehicle)); //Reads one vehicle
    if(vehicle.id == LAST_VEHICLE_ID) //Tests if the last vehicle loaded is the last vehicle (with id==-1)
    break;
    else  if(ret_read > 0){ //return a non-negative integer indicating the number of bytes actually read
      if(pthread_create(&tid_n,NULL,vehicle_guide,&vehicle) != OK)
      perror("func_North::Error on creating thread\n");
    }
  }

  close(fd_read); //closing controller fifo reading descriptor

  ret_unlink = unlink(FIFO_N);
  if(ret_unlink != OK){
    perror("func_North::Error on unlinking north controller fifo ");
  }

  pthread_exit(NULL);
}

//Function that the thread with pthread_t tid_s executes when is created
void* func_south(void* arg){

  int fd_read;
  Vehicle vehicle;
  int ret_read, ret_mkfifo, ret_unlink;
  pthread_t tid_n;

  ret_mkfifo = mkfifo(FIFO_S, PERMISSONS);
  if(ret_mkfifo != OK){
    perror("func_south::Error on opening south controller fifo ");
  }

  fd_read = open(FIFO_S, O_RDONLY | O_NONBLOCK); //Opens fifo in read mode

  open(FIFO_S, O_WRONLY); //Opens fifo in write mode to avoid busy waiting

  while(1){
    ret_read = read(fd_read, &vehicle, sizeof(Vehicle));//Reads one vehicle
    if(vehicle.id == LAST_VEHICLE_ID)//Tests if the last vehicle loaded is the last vehicle (with id==-1)
    break;
    else  if(ret_read > 0){ //return a non-negative integer indicating the number of bytes actually read
      if(pthread_create(&tid_n,NULL,vehicle_guide,&vehicle) != OK)
      perror("func_South::Error on creating thread\n");
    }
  }

  close(fd_read); //closing controller fifo reading descriptor

  ret_unlink = unlink(FIFO_S);
  if(ret_unlink != OK){
    perror("func_south::Error on unlink south controller fifo ");
  }

  pthread_exit(NULL);
}

//Function that the thread with pthread_t tid_e executes when is created
void* func_east(void* arg){
  int fd_read;
  Vehicle vehicle;
  int ret_read, ret_mkfifo, ret_unlink;
  pthread_t tid_n;


  ret_mkfifo = mkfifo(FIFO_E, PERMISSONS);
  if(ret_mkfifo != OK){
    perror("func_east::Error on opening east controller fifo ");
  }

  fd_read = open(FIFO_E, O_RDONLY | O_NONBLOCK); //Opens fifo in read mode

  open(FIFO_E, O_WRONLY); //Opens fifo in write mode to avoid busy waiting

  while(1){
    ret_read = read(fd_read, &vehicle, sizeof(Vehicle)); //Reads one vehicle
    if(vehicle.id == -1) //Tests if the last vehicle loaded is the last vehicle (with id==-1)
    break;
    else  if(ret_read > 0){//return a non-negative integer indicating the number of bytes actually read
      if(pthread_create(&tid_n,NULL,vehicle_guide,&vehicle) != OK)
      perror("func_east::Error on creating thread\n");
    }
  }
  close(fd_read);

  ret_unlink = unlink(FIFO_E);
  if(ret_unlink != OK){
    perror("func_east::Error on unlink east controller fifo ");
  }

  pthread_exit(NULL);
}

//Function that the thread with pthread_t tid_w executes when is created
void* func_west(void* arg){
  int fd_read;
  Vehicle vehicle;
  int ret_read, ret_mkfifo, ret_unlink;
  pthread_t tid_n;

  ret_mkfifo = mkfifo(FIFO_W, PERMISSONS);
  if(ret_mkfifo != OK){
    perror("func_west::Error on opening west controller fifo ");
  }

  fd_read = open(FIFO_W, O_RDONLY | O_NONBLOCK); //Opens fifo in read mode

  open(FIFO_W, O_WRONLY); //open in write mode to avoid busy waiting

  while(1){
    ret_read = read(fd_read, &vehicle, sizeof(Vehicle));  //Reads one vehicle
    if(vehicle.id == -1) //Tests if the last vehicle loaded is the last vehicle (with id==-1)
    break;
    else  if(ret_read > 0){ //return a non-negative integer indicating the number of bytes actually read
      if(pthread_create(&tid_n,NULL,vehicle_guide,&vehicle) != OK)
      perror("func_west::Error on creating thread\n");
    }
  }

  close(fd_read);

  ret_unlink = unlink(FIFO_W);
  if(ret_unlink != OK){
    perror("func_west::Error on unlinking west controller fifo ");
  }

  pthread_exit(NULL);
}

void preparing_log_file(){
  int ret_write;

  //Reseting File Parque.log
  //fopen creates an empty file for writing.
  //If a file with the same name already exists, its content is erased and the file is considered as a new empty file.
  FILE *fp=fopen(PARQUE_FILE_NAME,"w");
  if(fp == NULL){
    perror("preparing_log_file::Error on opening parque log file ");
  }
  fclose(fp);

  //opening the file "parque.log" to write vehicles information
  fd_parque_log = open(PARQUE_FILE_NAME, O_WRONLY | O_CREAT  , PERMISSONS);

  char buffer[] = "t(ticks) ; nlug ; id_viat ; observ\n";

  ret_write = write(fd_parque_log,buffer,strlen(buffer));
  if(ret_write == OK){
    perror("preparing_log_file::Error on writing to parque log file ");
  }
}


int main(int argc, char* argv[]){

  int number_of_spots=atoi(argv[1]);
  int time_open=atoi(argv[2]);
  int ret_sem_wait, ret_sem_post, ret_write, ret_sem_unlink, ret_mutex_1_destroy, ret_mutex_file_destroy;

  //Initializing some global variables
  park_capacity = number_of_spots;//Initializing the park with the number of spots
  unavailable_space = 0; // On the beginning the park has 0 places occupied
  park_close = 0; //The park is open

  pthread_t tid_n, tid_s, tid_e, tid_w;

  //Initialize the last vehicle to inform the park is closed. last_vehicle id is equal to -1
  Vehicle last_vehicle;
  last_vehicle.id = -1;
  last_vehicle.parking_time = 0;
  strcpy(last_vehicle.fifo_name, "over");

  sem_t *semaphore = sem_open(CONST_CHAR_NAME_SEMAPHORE, O_CREAT ,PERMISSONS,1);
  if(semaphore == SEM_FAILED){  //testing for some error on opening the semaphore
    perror("Parque::Error on opening semaphore ");
  }

  //name of program, park's number of spots and park's time open
  if(argc != 3){
    perror("Parque::Invalid number of arguments. Should be used in this format:\nParque <number_of_spots> <opening_time>\n");
  }

  preparing_log_file();//Prepares parque.log file

  //Creating the thread controller on the north pole of the park
  if(pthread_create(&tid_n,NULL,func_north,NULL) != OK)
  perror("Parque::Error on creating thread\n");
  //Creating the thread controller on the south pole of the park
  if(pthread_create(&tid_s,NULL,func_south,NULL) != OK)
  perror("Parque::Error on creating thread\n");
  //Creating the thread controller on the east pole of the park
  if(pthread_create(&tid_e,NULL,func_east,NULL) != OK)
  perror("Parque::Error on creating thread\n");
  //Creating the thread controller on the west pole of the park
  if(pthread_create(&tid_w,NULL,func_west,NULL) != OK)
  perror("Parque::Error on creating thread\n");

  printf("Park Open!\n");

  sleep(time_open);

  printf("Park Close!\n");

  //The park is closed
  park_close = 1;

  int fd_north = open(FIFO_N, O_WRONLY);//Opens North controller's FIFO in write mode
  int fd_south = open(FIFO_S, O_WRONLY);//Opens South controller's FIFO in write mode
  int fd_east = open(FIFO_E, O_WRONLY); //Opens East controller's FIFO in write mode
  int fd_west = open(FIFO_W, O_WRONLY); //Opens West controller's FIFO in write mode


  //Semaphore used to ensure that the thread vehicle doesn't try to send another vehicle to the controllers at the same time of the last vehicle
  ret_sem_wait = sem_wait(semaphore);
  if(ret_sem_wait != OK){
    perror("Parque::Error on doing sem_wait ");
  }

  //Send to north controller a vehicle that tells the park is closed (last_vehicle has id -1)
  ret_write = write(fd_north, &last_vehicle, sizeof(Vehicle));
  if(ret_write == OK){
    perror("Parque::Error on writing to north controller fifo the last vehicle ");
  }

  //Send to south controller a vehicle that tells the park is closed (last_vehicle has id -1)
  ret_write = write(fd_south, &last_vehicle, sizeof(Vehicle));
  if(ret_write == OK){
    perror("Parque::Error on writing to south controller fifo the last vehicle ");
  }

  //Send to east controller a vehicle that tells the park is closed (last_vehicle has id -1)
  ret_write = write(fd_east, &last_vehicle, sizeof(Vehicle));
  if(ret_write == OK){
    perror("Parque::Error on writing to east controller fifo the last vehicle ");
  }

  //Send to west controller a vehicle that tells the park is closed (last_vehicle has id -1)
  ret_write = write(fd_west, &last_vehicle, sizeof(Vehicle));
  if(ret_write == OK){
    perror("Parque::Error on writing to west controller fifo the last vehicle ");
  }

  ret_sem_post = sem_post(semaphore);
  if(ret_sem_post != OK){
    perror("Parque::Error on doing sem_post to the semaphore ");
  }
  sem_close(semaphore);

  close(fd_north);
  close(fd_south);
  close(fd_east);
  close(fd_west);

  //pthread_join() function waits for the north thread to terminate
  if(pthread_join(tid_n, NULL) != OK)
    perror("Parque::Error on joinning thread\n");
  //pthread_join() function waits for the south thread to terminate
  if(pthread_join(tid_s, NULL) != OK)
    perror("Parque::Error on joinning thread\n");
  //pthread_join() function waits for the east thread to terminate
  if(pthread_join(tid_e, NULL) != OK)
    perror("Parque::Error on join thread\n");
  //pthread_join() function waits for the west thread to terminate
  if(pthread_join(tid_w, NULL) != OK)
    perror("Parque::Error on join thread\n");

  ret_sem_unlink = sem_unlink(CONST_CHAR_NAME_SEMAPHORE);
  if(ret_sem_unlink != OK){
    perror("Parque::Error on unlinking main semaphore ");
  }

  ret_mutex_1_destroy = pthread_mutex_destroy(&mutex);
  if(ret_mutex_1_destroy != OK){
    perror("Parque::Error on destroying parque mutex ");
  }

  ret_mutex_file_destroy = pthread_mutex_destroy(&mutex);
  if(ret_mutex_file_destroy != OK){
    perror("Parque::Error on destroying parque mutex ");
  }



  pthread_exit(NULL);
}
