
#include <stdio.h>
#include "pico/stdlib.h"
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/btstack_cyw43.h"
#include "pico/sync.h"
#include <stdint.h>
//custom code libraries
// #include "Bluetooth_Server.c"
// #include "bluetooth_setup.h"


#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

//HELPER FUNCTIONS-----
// Perform initialisation of led pin
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}

// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}
//---------

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
const unsigned long periodThreeLED = 500;

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
         BLE_State = BLE_INIT;
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

enum TL_states {TL0, TL1, TL2};
int ThreeLED (int state);

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
            B = B & 0xE0;
            state = BL1;
        break;
        case (BL1):
            B = B | 0x01;
            state = BL0;
        break;
    }
    return state;
}

int ThreeLED (int state)
{
    switch (state)
    {
        case (TL0):
            B = (B & 0x01) | 0x80;
            state = TL1;
        break;
        case (TL1):
            B = (B & 0x01) | 0x40;
            state = TL2;
        break;
        case (TL2):
            B = (B & 0x01) | 0x20;
            state = TL0;
        break;
    }
    return state;
}

int main ()
{
    //---RTOS MAIN----
    // tasks[0].state = BL0;
    // tasks[0].period = periodBlinkLED;
    // tasks[0].elapsedTime = tasks[0].period;
    // tasks[0].Function = &BlinkLED;
    // tasks[1].state = TL0;
    // tasks[1].period = periodThreeLED;
    // tasks[1].elapsedTime = tasks[1].period;
    // tasks[1].Function = &ThreeLED;

    // // Bluetooth State mMachine Setup

    // TimerSet (period);
    // TimerOn ();

    // while (1);

    // return 0;
    //--------------------

    //-----DEMO CODE-----

    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    while (true) {
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS);
        pico_set_led(false);
        sleep_ms(LED_DELAY_MS);
    }

    //-------------------


}
// int main()
// {
//     int rc = pico_led_init();
//     hard_assert(rc == PICO_OK);
//     while (true) {
//         pico_set_led(true);
//         sleep_ms(LED_DELAY_MS);
//         pico_set_led(false);
//         sleep_ms(LED_DELAY_MS);
//     }
// }
