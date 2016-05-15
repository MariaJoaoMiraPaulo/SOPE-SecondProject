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

//Park_close variable indicates park's state (0 means is open, 1 means is closed)
int park_close;

int *park;
int park_capacity;
int busy_places = 0;

//Direction enums are the four cardinal points of access to the park
typedef enum {NORTH, SOUTH, EAST, WEST} Direction;

typedef struct {
  Direction direction;
  int id;
  float parking_time;
  char fifo_name[FIFO_NAME_LENGTH] ;
} Vehicle;

/*void* vehicle_guide(void* arg){
Vehicle vehicle= *(Vehicle*) arg;
void* ret=NULL;
int fd_read, fd_write;

//ler os dados do veiculo


//tentar estacionar
}*/

//Function that the thread with tid_n executes when is created
void* func_north(void* arg){
  void* ret = NULL;
  int fd_read, fd_write;
  Vehicle vehicle;
  int read_ret;

  mkfifo("fifoN", 0660);

  printf("Vou abrir o fifo\n");

  fd_read = open("fifoN", O_RDONLY | O_NONBLOCK);
  //open on write mode to avoid busy waiting
  fd_write = open("fifoN", O_WRONLY);

  printf("Já abri os fifos\n");

  while(!park_close){
    read_ret = read(fd_read, &vehicle, sizeof(Vehicle));
    if(read_ret > 0)
      printf("PARQUE NORTE ID : %d\n", vehicle.id);
  }

  printf("Norte: vou ler o resto\n");

  while(read_ret != -1){
    read_ret = read(fd_read, &vehicle, sizeof(Vehicle));
    printf("Read_ret: %d", read_ret);
    if(read_ret > 0)
      printf("PARQUE NORTE ID : %d\n", vehicle.id);
  }

  printf("Norte: vou acabar\n");

  close(fd_read);

  return ret;

}

//Function that the thread with tid_s executes when is created
void* func_south(void* arg){
  void* ret = NULL;
  int fd_read, fd_write;
  Vehicle vehicle;
  int read_ret;

  mkfifo("fifoS", 0660);

  printf("Vou abrir o fifo\n");

  fd_read = open("fifoS", O_RDONLY | O_NONBLOCK);
  //open on write mode to avoid busy waiting
  fd_write = open("fifoS", O_WRONLY);

  printf("Já abri os fifos\n");

  while(!park_close){
    read_ret = read(fd_read, &vehicle, sizeof(Vehicle));
    if(read_ret > 0)
      printf("PARQUE SUL ID : %d\n", vehicle.id);
  }

  printf("Sul: vou ler o resto\n");

  while(read_ret != -1){
    read_ret = read(fd_read, &vehicle, sizeof(Vehicle));
    if(read_ret > 0)
      printf("PARQUE SUL ID : %d\n", vehicle.id);
  }

  printf("Sul: vou acabar\n");

  close(fd_read);

  return ret;
}

//Function that the thread with tid_e executes when is created
void* func_east(void* arg){
  void* ret = NULL;
  int fd_read, fd_write;
  Vehicle vehicle;
  int read_ret;

  mkfifo("fifoE", 0660);

  printf("Vou abrir o fifo\n");

  fd_read = open("fifoE", O_RDONLY | O_NONBLOCK);
  //open on write mode to avoid busy waiting
  fd_write = open("fifoE", O_WRONLY);

  printf("Já abri os fifos\n");

  while(!park_close){
    read_ret = read(fd_read, &vehicle, sizeof(Vehicle));
    if(read_ret > 0)
      printf("PARQUE ESTE ID : %d\n", vehicle.id);
  }

  printf("Este: vou ler o resto\n");

  while(read_ret != -1){
    read_ret = read(fd_read, &vehicle, sizeof(Vehicle));
    if(read_ret > 0)
      printf("PARQUE ESTE ID : %d\n", vehicle.id);
  }

  printf("Este: vou acabar\n");

  close(fd_read);

  return ret;
}

//Function that the thread with tid_w executes when is created
void* func_west(void* arg){
  void* ret = NULL;
  int fd_read, fd_write;
  Vehicle vehicle;
  int read_ret;

  mkfifo("fifoW", 0660);

  printf("Vou abrir o fifo\n");

  fd_read = open("fifoW", O_RDONLY | O_NONBLOCK);
  //open on write mode to avoid busy waiting
  fd_write = open("fifoW", O_WRONLY);

  printf("Já abri os fifos\n");

  while(!park_close){
    read_ret = read(fd_read, &vehicle, sizeof(Vehicle));
    if(read_ret > 0)
      printf("PARQUE OESTE ID : %d\n", vehicle.id);
  }

  printf("Oeste: vou ler o resto\n");

  while(read_ret != -1){
    read_ret = read(fd_read, &vehicle, sizeof(Vehicle));
    if(read_ret > 0)
      printf("PARQUE OESTE ID : %d\n", vehicle.id);
  }

  printf("Oeste: vou acabar\n");

  close(fd_read);

  return ret;
}

int main(int argc, char* argv[]){

  int number_of_spots=atoi(argv[1]);
  int time_open=atoi(argv[2]);

  pthread_t tid_n, tid_s, tid_e, tid_w;

  //Initializing the park with the number of spots
  park = (int *)malloc(number_of_spots*sizeof(int));
  park_capacity = number_of_spots;

  //The park is open
  park_close = 0;

  if(argc != 3){
    perror("Invalid number of arguments.\n\n");
  }

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

  sleep(time_open);
  printf("Vou acabar\n");

  park_close = 1;

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

  pthread_exit(NULL);
}
