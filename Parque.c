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
#include <fcntl.h>

#define FIFO_NAME_LENGTH 10
#define OK 0

int parque_close;

typedef enum {NORTH, SOUTH, EAST, WEST} Direction;

typedef struct {
  Direction direction;
  int id;
  float parking_time;
  char fifo_name[FIFO_NAME_LENGTH] ;
} Vehicle;

void* func_north(void* arg){
  void* ret = NULL;
  int fd_read, fd_write;
  Vehicle vehicle;

  mkfifo("fifoN", 0660);

  printf("Vou abrir o fifo\n");

  fd_read = open("fifoN", O_RDONLY | O_NONBLOCK);
  //open on write mode to avoid busy waiting
  fd_write = open("fifoN", O_WRONLY);

    printf("Já abri os fifos\n");

  while(read(fd_read, &vehicle, sizeof(Vehicle))){
    printf("PARQUE NORTE ID : %d\n", vehicle.id);
    if(parque_close == 1)
      break;
  }

  close(fd_read);

  return ret;

}

void* func_south(void* arg){
  void* ret = NULL;
  int fd_read, fd_write;
  Vehicle vehicle;

  mkfifo("fifoS", 0660);

  printf("Vou abrir o fifo\n");

  fd_read = open("fifoS", O_RDONLY | O_NONBLOCK);
  //open on write mode to avoid busy waiting
  fd_write = open("fifoS", O_WRONLY);

    printf("Já abri os fifos\n");

  while(read(fd_read, &vehicle, sizeof(Vehicle))){
    printf("PARQUE SUL ID : %d\n", vehicle.id);
    if(parque_close == 1)
      break;
  }

  close(fd_read);

  return ret;
}


void* func_east(void* arg){
  void* ret = NULL;
  int fd_read, fd_write;
  Vehicle vehicle;

  mkfifo("fifoE", 0660);

  printf("Vou abrir o fifo\n");

  fd_read = open("fifoE", O_RDONLY | O_NONBLOCK);
  //open on write mode to avoid busy waiting
  fd_write = open("fifoE", O_WRONLY);

    printf("Já abri os fifos\n");

  while(read(fd_read, &vehicle, sizeof(Vehicle))){
    printf("PARQUE ESTE ID : %d\n", vehicle.id);
    if(parque_close == 1)
      break;
  }

  close(fd_read);

  return ret;
}


void* func_west(void* arg){
  void* ret = NULL;
  int fd_read, fd_write;
  Vehicle vehicle;

  mkfifo("fifoW", 0660);

  printf("Vou abrir o fifo\n");

  fd_read = open("fifoW", O_RDONLY | O_NONBLOCK);
  //open on write mode to avoid busy waiting
  fd_write = open("fifoW", O_WRONLY);

  printf("Já abri os fifos\n");

  while(read(fd_read, &vehicle, sizeof(Vehicle))){
    printf("PARQUE OESTE ID : %d\n", vehicle.id);
    if(parque_close == 1)
      break;
  }

  close(fd_read);

  return ret;
}


int main(int argc, char* argv[]){

  float time_generation=atoi(argv[1]);
  float u_clock=atoi(argv[2]);
  float total_number_ticks=(time_generation*pow(10,3))/u_clock;
  pthread_t tid_n, tid_s, tid_e, tid_w;

  parque_close = 0;

  if(argc != 3){
    perror("Invalid number of arguments.\n\n");
  }

  //creating 4 threads controllers
  if(pthread_create(&tid_n,NULL,func_north,NULL) != OK)
    perror("Parque::Error on creating thread\n");
  if(pthread_create(&tid_s,NULL,func_south,NULL) != OK)
    perror("Parque::Error on creating thread\n");
  if(pthread_create(&tid_e,NULL,func_east,NULL) != OK)
    perror("Parque::Error on creating thread\n");
  if(pthread_create(&tid_w,NULL,func_west,NULL) != OK)
    perror("Parque::Error on creating thread\n");

  printf("total_number_ticks %f\n",total_number_ticks );
  do{
    //suspends execution of the calling thread for (at least) u_clock*10^3 microseconds.
    usleep(u_clock*pow(10,3));
    total_number_ticks--;
    printf("Um tick\n");
  }while(total_number_ticks>0);
  printf("VOu acabar\n");

  parque_close = 1;

  if(pthread_join(tid_n, NULL) != OK)
    perror("Parque::Error on joinning thread\n");
  if(pthread_join(tid_s, NULL) != OK)
    perror("Parque::Error on joinning thread\n");
  if(pthread_join(tid_e, NULL) != OK)
    perror("Parque::Error on join thread\n");
  if(pthread_join(tid_w, NULL) != OK)
    perror("Parque::Error on join thread\n");

  pthread_exit(NULL);
}
