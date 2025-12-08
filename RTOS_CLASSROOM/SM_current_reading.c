#include <stdint.h>
#include <math.h>



// Global variable for storing current readings

uint16_t light_current = 0.0;
uint16_t  fan_current = 0.0;

// State machine states
typedef enum {
    IDLE,
    READING,
    PROCESSING,
    DONE
} 

state_t;
state_t light_state = IDLE;
state_t fan_state = IDLE;

// Task Scheduler
void task_scheduler() {
    if (light_state == IDLE && fan_state == IDLE) {
        // Prioritize light reading first
        light_state = READING;
    }

    if (light_state == READING) {
        light_current = get_current(0);  // Channel 0 for light
        light_state = PROCESSING;
    }

    if (fan_state == IDLE && light_state != READING) {
        fan_state = READING;
    }

    if (fan_state == READING) {
        fan_current = get_current(1);  // Channel 1 for fan
        fan_state = PROCESSING;
    }

    if (light_state == PROCESSING) {
        // Process the light current 
        light_state = DONE;
    }

    if (fan_state == PROCESSING) {
        // Process the fan current
        fan_state = DONE;
    }

    if (light_state == DONE && fan_state == DONE) {
        // Both state machines are done, reset to IDLE
        light_state = IDLE;
        fan_state = IDLE;
    }
}


// Main loop
int main() {
    while (1) {
        task_scheduler();  // Call the task scheduler in a loop

    
        
        printf("Light Current: %.2f A, Fan Current: %.2f A\n", light_current, fan_current);

        // Add delay for task scheduling
        delay_ms(1000);  // Adjust as needed
    }
}
