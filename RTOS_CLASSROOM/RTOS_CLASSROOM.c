//Custom Header
#include "bluetooth_setup.h"
#include "btstack_config.h"
#include <stdio.h>
#include "hardware/adc.h"
#include <stdint.h>          
#include "pico/stdlib.h"
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/btstack_cyw43.h"
#include "pico/sync.h"


// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

#define APP_AD_FLAGS 0x06

#define PICO_GPIO_5 5
#define PICO_GPIO_6 6

// Constants for current sensing
#define V_REF           3.3     // Pico ADC reference voltage
#define ADC_RESOLUTION  4095.0  // 12-bit ADC (Pico uses 0-4095 range)
#define OFFSET_VOLTAGE  1.65    // 0A output voltage for ACS712 (half of V_REF)
#define SENSITIVITY     0.185   // V/A for ACS712-5A model





static unsigned char reset;


//----RTOS CLASSES
//--RTOS TASK RELATED CODE
typedef struct task
{
    unsigned char running; // 1 indicates task is running
    unsigned char priority; //1,2,3 where 1 is highest priority
    int state; //state of machine
    unsigned long period;
    unsigned long elapsedTime;
    int (*Function) (int);
} task;


typedef struct queue
{
    unsigned char buffer[4];
    unsigned char cnt; 
}queue;

queue my_queue;

// volatile static unsigned char override_value = 1;
//queue related functions
void initilize_queue(queue *Q)
{
    Q->cnt = 0; 
}

unsigned char queue_full(queue Q)
{
    return(Q.cnt >= 4);
}
unsigned char queue_empty(queue Q)
{
    return(Q.cnt ==0);
}

void print_queue(queue Q)
{
    unsigned char i;
    printf("Queue Contents\n\r");

    for(i=0;i<4;i++)
    {
        printf("Item: %d\n\r",i);
    
        printf("Value: %d\n\r",Q.buffer[i]);
    }
}

void push_queue(queue *Q, unsigned char item)
{
    if(!queue_full(*Q))
    {
        Q->buffer[Q->cnt] = item;
        Q->cnt++;
    }
}
unsigned char pop_queue(queue *Q)
{
    unsigned char item=0;
    unsigned char i;
    if(!queue_empty(*Q))
    {
        item = Q->buffer[0];
        Q->cnt--;
        for (i=0; i<Q->cnt; i++) {
        // shift fwd
        Q->buffer[i] = Q->buffer[i+1];

      }
    }
    return item;
}

const unsigned int numTasks = 4;
const unsigned long period = 1;
// const unsigned long periodBlinkLED = 200;
// const unsigned long periodThreeLED = 400;
const unsigned long periodTickFct_BLE_Poll = 100;
const unsigned long periodReadQueue = 500;
const unsigned long periodWriteQueue = 200;
const unsigned long periodCurrent = 250; //2000 khz



enum ReadQueue_States { ReadQueue_Init, ReadQueue_READ, ReadQueue_PUSH } ReadQueue_State;
int TickFct_Read_Override_Queue(int state);

enum WriteQueue_States { WriteQueue_Init, WriteQueue_READ, WriteQueue_PUSH } WriteQueue_State;
int TickFct_Write_Override_Queue(int state);

enum BLE_Poll { SM1_INIT, SM1_POLL } SM1_State;
int TickFct_BLE_Poll(int state);

//current sensing states

uint32_t light_current = 0.0;
float  fan_current = 0.0;

typedef enum {IDLE, READING, PROCESSING, DONE} state_t;
state_t light_state = IDLE;
state_t fan_state = IDLE;

int current_sense_tickFct(int state);
// enum BL_states {BL0, BL1};
// int BlinkLED (int state);

// enum TL_states {TL0, TL1, TL2};
// int ThreeLED (int state);

unsigned char B = 0;
char led_on = 0;

task tasks[4];

volatile static unsigned char currentPriority = 0; //holds the priority of the currently executing task
bool repeating_timer_callback(__unused struct repeating_timer *t) {
    // printf("Repeat at %lld\n", time_us_64());
    // printf("Timer tick: %d\n", );
    unsigned char i;
    for (i = 0; i < numTasks; i++)
    {
        // printf("============analysis==========: %d\n",i);
        // printf("task:%d,priority:%d,otherp:%d,currentT:%d\n",i,tasks[i].priority,tasks[currentTask].priority,currentTask);
        // printf("Task: %d\n",i);
        // printf("Priority: %d\n",tasks[i].priority);
        // printf("CurrentP: %d\n",currentPriority);
        // printf("============finished==========: %d\n",i);


        if ((tasks[i].elapsedTime >= tasks[i].period) 
        && (!tasks[i].running)
        && (tasks[i].priority>=currentPriority)){

            currentPriority = tasks[i].priority;
            //DisableInterrupts()
            // enabled = 0;
            tasks[i].elapsedTime = 0;
            tasks[i].running = 1;
            //EnableInterrupts()
            // enabled = 1;

            // printf("Executing Task %d, %d\n",i,tasks[i].priority);
            tasks[i].state = tasks[i].Function(tasks[i].state);
            tasks[i].running = 0;

            //DisableInterrupts()
            // enabled = 0;
            currentPriority-=1;
            // enabled = 1;
            //EnableInterrupts
        }

        tasks[i].elapsedTime += period;
    }
    currentPriority = 0;
    // led_on = ~led_on;
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);


    return true;
}

static struct repeating_timer timer;


//----------BLUETOOTH RELATED CODE--------------------

static uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    // UNUSED(con_handle);
    printf("Attribute Handle: %x\n",attribute_handle);
    printf("Con Handle: %x\n",con_handle);
    printf("Offset: %x\n",offset);
    printf("Current Handle: %x\n",service_object.characteristic_current_handle);
    printf("Fan Handle: %x\n",service_object.characteristic_fan_ctrl_handle);
    printf("Light Handle: %x\n",service_object.characteristic_lights_ctrl_handle);

    //CURRENT
    if(attribute_handle == service_object.characteristic_current_handle)
    {
    printf("READING-------------------------------\n");

        return att_read_callback_handle_blob(service_object.characteristic_current_value, strlen(service_object.characteristic_current_value), offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_current_configuration_handle)
    {
    printf("READING-------------------------------\n");

        return att_read_callback_handle_little_endian_16(service_object.characteristic_current_client_configuration, offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_current_user_description_handle)
    {
    printf("READING-------------------------------\n");

        return att_read_callback_handle_blob(service_object.characteristic_current_user_description,strlen(service_object.characteristic_current_user_description), offset, buffer, buffer_size);
    }
    //FAN CONTROL
    if(attribute_handle == service_object.characteristic_fan_ctrl_handle)
    {
    printf("READING-------------------------------\n");

        return att_read_callback_handle_blob(service_object.characteristic_fan_ctrl_value, strlen(service_object.characteristic_fan_ctrl_value), offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_fan_ctrl_configuration_handle)
    {
    printf("READING-------------------------------\n");

        return att_read_callback_handle_little_endian_16(service_object.characteristic_fan_ctrl_client_configuration, offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_fan_ctrl_user_description_handle)
    {
    printf("READING-------------------------------\n");

        return att_read_callback_handle_blob(service_object.characteristic_fan_ctrl_user_description,strlen(service_object.characteristic_fan_ctrl_user_description), offset, buffer, buffer_size);
    }
    //-----
// fan_ctrl_override
//FAN CONTROL
    if(attribute_handle == service_object.characteristic_fan_ctrl_override_handle)
    {
    printf("READING-------------------------------\n");

        return att_read_callback_handle_blob(service_object.characteristic_fan_ctrl_override_value, strlen(service_object.characteristic_fan_ctrl_override_value), offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_fan_ctrl_override_configuration_handle)
    {
    printf("READING-------------------------------\n");

        return att_read_callback_handle_little_endian_16(service_object.characteristic_fan_ctrl_override_client_configuration, offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_fan_ctrl_override_user_description_handle)
    {
    printf("READING-------------------------------\n");

        return att_read_callback_handle_blob(service_object.characteristic_fan_ctrl_override_user_description,strlen(service_object.characteristic_fan_ctrl_override_user_description), offset, buffer, buffer_size);
    }
    //-------
    // LIGHT CONTROL
    if(attribute_handle == service_object.characteristic_lights_ctrl_handle)
    {
    printf("READING-------------------------------\n");

        return att_read_callback_handle_blob(service_object.characteristic_lights_ctrl_value, strlen(service_object.characteristic_lights_ctrl_value), offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_lights_ctrl_configuration_handle)
    {
    printf("READING-------------------------------\n");

        return att_read_callback_handle_little_endian_16(service_object.characteristic_lights_ctrl_client_configuration, offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_lights_ctrl_user_description_handle)
    {
        return att_read_callback_handle_blob(service_object.characteristic_lights_ctrl_user_description,strlen(service_object.characteristic_lights_ctrl_user_description), offset, buffer, buffer_size);
    }

    // SCHEDULE
    if(attribute_handle == service_object.characteristic_schedule_handle)
    {
    printf("READING-------------------------------\n");

        return att_read_callback_handle_blob(service_object.characteristic_schedule_value, strlen(service_object.characteristic_schedule_value), offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_schedule_configuration_handle)
    {
        return att_read_callback_handle_little_endian_16(service_object.characteristic_schedule_client_configuration, offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_schedule_user_description_handle)
    {
        return att_read_callback_handle_blob(service_object.characteristic_schedule_user_description,strlen(service_object.characteristic_schedule_user_description), offset, buffer, buffer_size);
    }

    return 0;
}


static int att_write_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    // printf("%s",attribute_handle);
    // UNUSED(transaction_mode);
    // UNUSED(offset);
    // UNUSED(buffer_size);
    // printf("WRITING-------------------------------\n");

        //Current callback
    printf("Attribute Handle: %x\n",attribute_handle);
    printf("Con Handle: %x\n",con_handle);
    printf("Transaction Mode: %x\n",transaction_mode);
    printf("Offset: %x\n",offset);


    printf("Current Handle: %x\n",service_object.characteristic_current_handle);

    printf("Fan Handle: %x\n",service_object.characteristic_fan_ctrl_handle);

    printf("Light Handle: %x\n",service_object.characteristic_lights_ctrl_handle);

    //If current service config is set to NOTIFY
    if(attribute_handle == ATT_CHARACTERISTIC_3727f058_69dd_4d78_9b6f_9c2ebbb67d44_01_VALUE_HANDLE)
    {

        service_object.characteristic_current_client_configuration = little_endian_read_16(buffer,0);
        service_object.con_handle = con_handle;
    }
    if(attribute_handle == service_object.characteristic_current_handle)
    {

        custom_service * instance = &service_object;
        buffer[buffer_size] = 0;
        memset(service_object.characteristic_current_value, 0, strlen(service_object.characteristic_current_value));
        memcpy(service_object.characteristic_current_value, buffer, strlen(buffer));

        if(instance->characteristic_current_configuration_handle)
        {
            instance->callback_current.callback = &characteristic_current_callback;
            instance->callback_current.context = (void*) instance;
            att_server_register_can_send_now_callback(&instance->callback_current, instance->con_handle);
        }
    }
    if(attribute_handle== ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c7_01_VALUE_HANDLE)
    {
        printf("WRITING-------------------------------\n");

        custom_service * instance = &service_object;
        buffer[buffer_size] = 0;
        printf("Inside fan handler\n");
        memset(service_object.characteristic_fan_ctrl_value, 0, strlen(service_object.characteristic_fan_ctrl_value));
        memcpy(service_object.characteristic_fan_ctrl_value, buffer, strlen(buffer));

        if(instance->characteristic_fan_ctrl_configuration_handle)
        {
            instance->callback_fan_ctrl.callback = &characteristic_fan_ctrl_callback;
            instance->callback_fan_ctrl.context = (void*) instance;
            att_server_register_can_send_now_callback(&instance->callback_fan_ctrl, instance->con_handle);
        }
    

        //Alert application of bluetooth rx

    }
    if(attribute_handle== service_object.characteristic_lights_ctrl_handle)
    {
        printf("Inside fan handler\n");

        custom_service * instance = &service_object;
        buffer[buffer_size] = 0;
        memset(service_object.characteristic_lights_ctrl_value, 0, strlen(service_object.characteristic_lights_ctrl_value));
        memcpy(service_object.characteristic_lights_ctrl_value, buffer, strlen(buffer));

        if(instance->characteristic_lights_ctrl_configuration_handle)
        {
            instance->callback_lights_ctrl.callback = &characteristic_lights_ctrl_callback;
            instance->callback_lights_ctrl.context = (void*) instance;
            att_server_register_can_send_now_callback(&instance->callback_lights_ctrl, instance->con_handle);
        }

    }
    if(attribute_handle == service_object.characteristic_fan_ctrl_override_handle)
    {
        printf("Inside fan handler\n");

        custom_service * instance = &service_object;
        buffer[buffer_size] = 0;
        memset(service_object.characteristic_fan_ctrl_override_value, 0, strlen(service_object.characteristic_fan_ctrl_override_value));
        memcpy(service_object.characteristic_fan_ctrl_override_value, buffer, strlen(buffer));

        if(instance->characteristic_fan_ctrl_override_configuration_handle)
        {
            instance->callback_fan_ctrl_override.callback = &characteristic_fan_ctrl_override_callback;
            instance->callback_fan_ctrl_override.context = (void*) instance;
            att_server_register_can_send_now_callback(&instance->callback_fan_ctrl_override, instance->con_handle);
        }
    }
    
        
       
    return 0;
}


static unsigned char characteristic_current_rx[100];
static unsigned char characteristic_fan_ctrl_tx[100];
static unsigned char characteristic_lights_ctrl_tx[100];
static unsigned char characteristic_lights_ctrl_override_tx[100];
static unsigned char characteristic_fan_ctrl_override_tx[100];

static unsigned char characteristic_schedule_tx[100];


// static uint8_t adv_data[] = {
//     // Flags general discoverable
//     0x02, BLUETOOTH_DATA_TYPE_FLAGS, APP_AD_FLAGS,
//     // Name
//     0x17, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'P', 'i', 'c', 'o', ' ', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0',
//     0x03, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x1a, 0x18,
// };

int le_notification_enabled;
hci_con_handle_t con_handle;
uint16_t current_light_reading = 0;
uint16_t current_fan_reading = 0;
static btstack_packet_callback_registration_t hci_event_callback_registration;



static uint8_t adv_data[] = {
    // Flags general discoverable
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, APP_AD_FLAGS,
    // Name
    0x06, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'P', 'i', 'c', 'o', 'W',
    // Custom Service UUID
    0x03, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x10, 0xFF,
};
static const uint8_t adv_data_len = sizeof(adv_data);



static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    UNUSED(size);
    UNUSED(channel);

    bd_addr_t local_addr;

    if(packet_type != HCI_EVENT_PACKET) return;

    //retreive information about packet

    unsigned char event_type = hci_event_packet_get_type(packet);

    switch(event_type){
        case BTSTACK_EVENT_STATE:
            if(btstack_event_state_get_state(packet) != HCI_STATE_WORKING) return;
            gap_local_bd_addr(local_addr);
            printf("BTstack is running on address %s",bd_addr_to_str(local_addr));

            // //setup advertisements
            unsigned short adv_int_min = 800;
            unsigned short adv_int_max = 800;

            unsigned char adv_type = 0;

            bd_addr_t null_addr;

            memset(null_addr,0,6);
            gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
            assert(adv_data_len <= 31); // ble limitation
            gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
            gap_advertisements_enable(1);
            // printf("Sending Advetisement");
            break;
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            // le_notification_enabled = 0;
            break;
        case ATT_EVENT_CAN_SEND_NOW:
            // service_current
            att_server_notify(service_object.con_handle, service_object.characteristic_current_handle, (uint8_t*)&current_light_reading, sizeof(current_light_reading));
        default:
            break;
    }
}
unsigned char bluetooth_tx       = 0;
short current_reading = 0;



// ------ BLUETOOTH RELATED STATE MACHINE INFORMATION


int main()
{
    
    //initialize standard in/out
    stdio_init_all();


    gpio_init(5);
    gpio_set_dir(PICO_GPIO_5,GPIO_OUT);
    // gpio_set_dir(PICO_GPIO_5,GPIO_OUT);

    // tasks[0].state = BL0;
    // tasks[0].period = periodBlinkLED;
    // tasks[0].elapsedTime = tasks[0].period;
    // tasks[0].Function = &BlinkLED;
    // tasks[0].running = 0;
    // tasks[0].priority = 2;

    // tasks[1].state = TL0;
    // tasks[1].period = periodThreeLED;
    // tasks[1].elapsedTime = tasks[1].period;
    // tasks[1].Function = &ThreeLED;
    // tasks[1].running = 0;
    // tasks[1].priority = 1;

    tasks[0].state = SM1_INIT;
    tasks[0].period = periodTickFct_BLE_Poll;
    tasks[0].elapsedTime = tasks[0].period;
    tasks[0].Function = &TickFct_BLE_Poll;
    tasks[0].running = 0;
    tasks[0].priority = 4;

    tasks[1].state = ReadQueue_Init;
    tasks[1].period = periodReadQueue;
    tasks[1].elapsedTime = tasks[1].period;
    tasks[1].Function = &TickFct_Read_Override_Queue;
    tasks[1].running = 0;
    tasks[1].priority = 3;

    tasks[2].state = WriteQueue_Init;
    tasks[2].period = periodWriteQueue;
    tasks[2].elapsedTime = tasks[2].period;
    tasks[2].Function = &TickFct_Write_Override_Queue;
    tasks[2].running = 0;
    tasks[2].priority = 2;

    tasks[3].state = IDLE;
    tasks[3].period = periodCurrent;
    tasks[3].elapsedTime = tasks[3].period;
    tasks[3].Function = &current_sense_tickFct;
    tasks[3].running = 0;
    tasks[3].priority = 1;



    if(cyw43_arch_init())
             {
                 printf("Failed to initialize Bluetooth");
             }
        //initialize L2CAP, security manager
    l2cap_init();
    sm_init();
        // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    
    
    att_server_init(profile_data,att_read_callback,att_write_callback);
    
    custom_service_server_init(characteristic_current_rx,characteristic_fan_ctrl_tx,characteristic_lights_ctrl_tx,characteristic_fan_ctrl_override_tx,characteristic_lights_ctrl_override_tx,characteristic_schedule_tx);
    
    hci_event_callback_registration.callback = &packet_handler;
        
    hci_add_event_handler(&hci_event_callback_registration);
    //
        // register for ATT event
    att_server_register_packet_handler(packet_handler);
    
        // set one-shot btstack timer
        // heartbeat.process = &heartbeat_handler;
        // btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
        // btstack_run_loop_add_timer(&heartbeat);
        
    // turn on bluetooth!
    hci_power_control(HCI_POWER_ON);
    set_characteristic_current(10);
    //status
    set_characteristic_fan_ctrl(1);
    set_characteristic_lights_ctrl(1);

    set_characteristic_fan_ctrl_override(1);
    set_characteristic_lights_ctrl_override(1);

    set_characteristic_schedule(1);
    adc_init();
    adc_gpio_init(26);  // Enable GP26/ADC0
    adc_select_input(0);
    
    add_repeating_timer_us(1, repeating_timer_callback, NULL, &timer); 



    

    while (true) {}//forever loop
}

//task testing code
// Task implementations
/*
int BlinkLED (int state)
{
    switch (state)
    {
        case (BL0):
            B = B & 0xE0;
            printf("BLINK: OFF\n");

            state = BL1;
            break;
        case (BL1):
            B = B | 0x01;
            printf("BLINK: ON\n");

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
            printf("THREE BLINK: 1\n");

            state = TL1;
            break;
        case (TL1):
            B = (B & 0x01) | 0x40;
            printf("THREE BLINK: 2\n");

            state = TL2;
            break;
        case (TL2):
            B = (B & 0x01) | 0x20;
            printf("THREE BLINK: 3\n");

            state = TL0;
            break;
    }
    return state;
}

*/
int TickFct_BLE_Poll(int state) {
    switch(SM1_State) { // Transitions
        case SM1_INIT: 
            if (1) {
                SM1_State = SM1_POLL;

            }
            break;
        case SM1_POLL: 
            if (!reset) {
                SM1_State = SM1_POLL;
            }
            else if (reset) {
                SM1_State = SM1_INIT;
            }
            break;
        default:
            SM1_State = SM1_INIT;
   } // Transitions

    switch(SM1_State) { // State actions
        case SM1_INIT:
        // BLUETOOTH RELATED INITILIZATION CODE

         
        break;
        case SM1_POLL:
            led_on = ~led_on;

            // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);

            async_context_poll(cyw43_arch_async_context());
         
        break;
        default: // ADD default behaviour below
        break;
   } // State actions
   return state;
}
//END BLUETOOTH RELATED CODE

int TickFct_Read_Override_Queue(int state) {
   switch(ReadQueue_State) { // Transitions
        case ReadQueue_Init: 
        if (1) {
            ReadQueue_State = ReadQueue_READ;
        }
        break;
        case ReadQueue_READ: 
        if (1) {
            // printf("READ\n");
            ReadQueue_State = ReadQueue_READ;
        }
        break;
      default:
        // printf("DEFAULT\n");
         ReadQueue_State = ReadQueue_Init;
   } // Transitions

    switch(ReadQueue_State) { // State actions
        case ReadQueue_Init:
            initilize_queue(&my_queue);
            break;
        case ReadQueue_READ:
            unsigned char value =0;
            value = pop_queue(&my_queue);
            // printf("\n\nvalue = %d\n\n",value);
            if(value==49) //since this is treated as a char, '1' is 49
            {
                // printf("ON\n");
                // cyw43_arch_gpio_put(CYW43_L_GPIO_LED_PIN, 1);
                gpio_put(PICO_GPIO_5,1);

                
            }
            else
            {
                // printf("OFF\n");
                // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
                gpio_put(PICO_GPIO_5,0);



            }
            break;
        default: // ADD default behaviour below
        break;
   } // State actions
   return state;
}

int TickFct_Write_Override_Queue(int state) {
   switch(WriteQueue_State) { // Transitions
        case WriteQueue_Init: 
        if (1) {
            WriteQueue_State = WriteQueue_PUSH;
        }
        break;
        case WriteQueue_PUSH: 
        if (1) {
            // print_queue(my_queue);
            // printf("PUSH\n");
            WriteQueue_State = WriteQueue_PUSH;
        }
         break;
      default:
        // printf("DEFAULT\n");
         WriteQueue_State = WriteQueue_Init;
   } // Transitions

    switch(WriteQueue_State) { // State actions
        case WriteQueue_Init:
            initilize_queue(&my_queue);
            break;
        case WriteQueue_PUSH:
            unsigned char override_value = *service_object.characteristic_fan_ctrl_override_value;
            push_queue(&my_queue,override_value);
            break;
        default: // ADD default behaviour below
        break;
   } // State actions
   return state;
}

volatile static unsigned long cnt = 0;

//FOR SAMPLING OF 2kHz, a cumulativ
volatile static long double cumulative_sum;
volatile static float avg_current; //mili amps

float get_current(char adc_channel)
{
    cnt+=1;
    adc_select_input(adc_channel);
    uint16_t raw = adc_read();
    const float conversion_factor = 5.0f / (1 << 12);
    // (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.066

    // printf("Raw: %x\r\n",raw);
    uint16_t result = ((uint32_t) raw);
    // float converted = OFFSET_VOLTAGE - ((result * 5.0) / ADC_RESOLUTION)/SENSITIVITY;
    float converted = ((uint32_t) raw * V_REF)/ ADC_RESOLUTION;
    cumulative_sum+=converted;

    if(cnt%15000==0)
    {
        printf("cnt: %d\n",cnt);
        avg_current = (cumulative_sum/15000)*100;
        // printf("Raw value: 0x%03d, current: %f A\n", result, converted);
        printf("Avg Current: %f A\n", avg_current);
        cumulative_sum=0;

    }
    uint32_t voltage = ((uint32_t) raw * V_REF)/ ADC_RESOLUTION;
    // printf("Voltage: %d\r\n",voltage);
    // printf("Voltage (Hex): %x\r\n",raw);


    uint32_t current = (long)(voltage - OFFSET_VOLTAGE) / SENSITIVITY;
    // signed long result = (voltage - OFFSET_VOLTAGE) / SENSITIVITY;
    // printf("voltage: %d A\r\n",voltage);
    // printf("SENSITIVITY: %d A\r\n",SENSITIVITY);

    // printf("result: %d A\r\n",result);
    // printf("Current: %d A\r\n",current);

    return result;
    // printf("Getting Current\n");
}

int current_sense_tickFct(int state) {
    if (light_state == IDLE && fan_state == IDLE) {
        // Prioritize light reading first
        light_state = READING;
    }

    if (light_state == READING) {
        light_current = get_current(0);  // Channel 0 for light
        // printf("Current: %.2f A\r\n",light_current);
;
        light_state = PROCESSING;
    }

    if (fan_state == IDLE && light_state != READING) {
        fan_state = READING;
    }

    if (fan_state == READING) {
        // fan_current = get_current(1);  // Channel 1 for fan
        // printf("Fan Curr: %d\n\r",fan_current);

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
    return state;
}



