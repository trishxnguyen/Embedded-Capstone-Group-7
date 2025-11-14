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
const unsigned long periodFan = 500;

task tasks[2];

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
    tasks[1].Function = &Fan;
    TimerSet (period);
    TimerOn ();

    while (1);

    return 0;
}
// Task implementations

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