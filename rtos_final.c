// RTOS final
// RTOS 4 + refactoring
// #include "RIMS.h"
// #include "C:\Users\t1m0t\Documents\GitHub Repositories\ImageProcessing-Project\Embedded-Capstone-Group-7\CurrentSensor.c"
// sense_current
unsigned char B;
typedef struct task
{
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*Function) (int);
} task;

const unsigned int numTasks = 2;
const unsigned long period = 100;
const unsigned long periodBlinkLED = 1500;
const unsigned long periodFan = 500;

task tasks[4];

// BLUETOOTH RELATED STATE VARIABLES and functions
unsigned char rx_flag       = 0;
unsigned char tx_flag       = 0;
unsigned char read_data  = 0;
unsigned char write_data = 0;

unsigned char read_bluetooth()
{
    //
    //read from characteristics and take actions accordingly:
    //- READ SCHEDULE THAT CLIENT(LAPTOP) WOULD HAVE WRITTEN AND ASSIGN THE VALUE TO VARIABLE
    //- READ FAN_CONTROL THAT CLIENT(LAPTOP) WOULD HAVE WRITTEN AND ASSIGN THE VALUE TO VARIABLE
    //- READ LIGHTS_CONTROL THAT CLIENT(LAPTOP) WOULD HAVE WRITTEN AND ASSIGN THE VALUE TO VARIABLE
    //This should be done say, every 10 seconds

    return(5);
}
unsigned char write_bluetooth()
{
    // write to the bluetooth characteristics:
    //- WRITE CURRENT READING
    return(1);
}

enum BLE_States { BLE_IDLE, BLE_INIT, BLE_READ_B, BLE_WRITE_B, BLE_ERROR } BLE_State;

void TickFct_BLE_State() {
   switch(BLE_State) { // Transitions
      case BLE_INIT:
         BLE_State = BLE_IDLE;
         break;
         case BLE_IDLE: 
         if (!rx_flag & !tx_flag) {
            BLE_State = BLE_IDLE;
         }
         else if (rx_flag & !tx_flag) {
            BLE_State = BLE_READ_B;
         }
         else if (tx_flag & !rx_flag) {
            BLE_State = BLE_WRITE_B;
         }
         else if (rx_flag & tx_flag) {
            BLE_State = BLE_ERROR;
         }
         break;
      case BLE_READ_B: 
         if (rx_flag) {
            BLE_State = BLE_READ_B;
         }
         break;
      case BLE_WRITE_B: 
         if (tx_flag) {
            BLE_State = BLE_WRITE_B;
         }
         else if (!tx_flag) {
            BLE_State = BLE_IDLE;
         }
         break;
      case BLE_ERROR: 
         if (1) {
            BLE_State = BLE_INIT;
         }
         break;
      default:
         BLE_State = BLE_INIT;
   } // Transitions

   switch(BLE_State) { // State actions
      case BLE_IDLE:
         break;
      case BLE_INIT:
         //Turn on Bluetooth Chip
         rx_flag=0;
         tx_flag=0;
         break;
      case BLE_READ_B:
         read_data = read_bluetooth();
         rx_flag = 0;
         
         break;
      case BLE_WRITE_B:
         write_data = write_bluetooth();
         tx_flag=0;
         break;
      case BLE_ERROR:
         printf("error");
         //Turn off Bluetooth Chip
         break;
      default: // ADD default behaviour below
      break;
   } // State actions

}


//---

enum BL_states {BL0, BL1};
int BlinkLED (int state);

enum F_states {F0, F1, F2};
int Fan (int state);

void TimerISR ()
{
    unsigned char i;
    for (i = 0; i < numTasks; i++)
    {
        if (tasks[i].elapsedTime >= tasks[i].period)
        {
        tasks[i].state = tasks[i].Function(tasks[i].state);
        tasks[i].elapsedTime = 0;
        }
        tasks[i].elapsedTime += period;
    }
}

int main ()
{
    tasks[0].state = BL0;
    tasks[0].period = periodBlinkLED;
    tasks[0].elapsedTime = tasks[0].period;
    tasks[0].Function = &BlinkLED;
    tasks[1].state = F0;
    tasks[1].period = periodFan;
    tasks[1].elapsedTime = tasks[1].period;
<<<<<<< HEAD
    tasks[1].Function = &Fan;
=======
    tasks[1].Function = &ThreeLED;

    // Bluetooth State mMachine Setup

>>>>>>> f7755cae3a5eb5fa72ce648a4e18316c58f9d27b
    TimerSet (period);
    TimerOn ();

    while (1);

    return 0;
}
// Task implementations

//Communications Handler
//Current Sensing
//Device Scheduler
//User Input Handler

int BlinkLED (int state)
{
    switch (state)
    {
        case (BL0):
            B = 0x00;
            if (A1) // schedule queue logic needs
            state = BL1;
        break;

        case (BL1):
            B = 0x01;
            if (!A1) 
            state = BL0;
        break;
    }
    return state;
}

int Fan (int state) // change fan
{
    switch (state)
    {
        case (F0):
            B = (B & 0x01) | 0x80;
            state = F1;
        break;
        case (F1):
            B = (B & 0x01) | 0x40;
            state = F2;
        break;
        case (F2):
            B = (B & 0x01) | 0x20;
            state = F0;
        break;
    }
    return state;
}