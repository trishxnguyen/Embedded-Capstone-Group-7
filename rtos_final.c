// RTOS final
// RTOS 4 + refactoring
#include "RIMS.h"

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

task tasks[2];

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

int main ()
{
    tasks[0].state = BL0;
    tasks[0].period = periodBlinkLED;
    tasks[0].elapsedTime = tasks[0].period;
    tasks[0].Function = &BlinkLED;
    tasks[1].state = TL0;
    tasks[1].period = periodThreeLED;
    tasks[1].elapsedTime = tasks[1].period;
    tasks[1].Function = &ThreeLED;
    TimerSet (period);
    TimerOn ();

    while (1);

    return 0;
}
// Task implementations

//Communications Handler
//Current Sensing
//Device Scheduler
//Data Logger


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