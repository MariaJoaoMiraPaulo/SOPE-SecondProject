//Maria Joao Mira Paulo e Nuno Ramos
//2º Trabalho SOPE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define OK 0

int over ;

void* func_north(void* arg){
  void* ret = NULL;

  return ret;

}

void* func_south(void* arg){
  void* ret = NULL;

  return ret;
}


void* func_east(void* arg){
  void* ret = NULL;

  return ret;
}


void* func_west(void* arg){
  void* ret = NULL;

  return ret;
}


int main(int argc, char* argv[]){

  float time_generation=atoi(argv[1]);
  float u_clock=atoi(argv[2]);
  float total_number_ticks=(time_generation*pow(10,3))/u_clock;
  pthread_t tid_n, tid_s, tid_e, tid_w;

  over = 0;

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
  }while(total_number_ticks>0);

  over = 1;

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
