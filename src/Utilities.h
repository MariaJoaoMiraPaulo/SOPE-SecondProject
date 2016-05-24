//Maria Joao Mira Paulo e Nuno Ramos
//2º Trabalho SOPE

#define VEHICLE_IN 0
#define PARK_FULL 1
#define PARK_CLOSED 2
#define VEHICLE_OUT 3
#define PARKING 4
#define OK 0
#define FIFO_NAME_LENGTH 10
#define STATUS_MAX_LENGTH 20
#define FILE_LINE_MAX_LENGTH 100
#define CONST_CHAR_NAME_SEMAPHORE "/sem"
#define FIFO_N "fifoN"
#define FIFO_S "fifoS"
#define FIFO_E "fifoE"
#define FIFO_W "fifoW"
#define VEHICLE_FIFO_NAME "fifo"
#define PERMISSONS 0660

//Direction enums are the four cardinal points of access to the park
typedef enum {NORTH, SOUTH, EAST, WEST} Direction;

typedef struct {
  Direction direction;
  int id;
  float parking_time;
  float parking_time_ticks;
  char fifo_name[FIFO_NAME_LENGTH] ;
  int initial_ticks;
}Vehicle;
