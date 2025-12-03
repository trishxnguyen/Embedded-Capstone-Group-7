//Custom Header
#include "bluetooth_setup.h"
#include "btstack_config.h"
#include <stdio.h>
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
    

        //Alert application of bluetooth rx

    }
    
        
       
    return 0;
}


static unsigned char characteristic_current_rx[100];
static unsigned char characteristic_fan_ctrl_tx[100];
static unsigned char characteristic_lights_ctrl_tx[100];
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
uint16_t current_temp = 50;
static btstack_timer_source_t heartbeat;
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
            att_server_notify(service_object.con_handle, service_object.characteristic_current_handle, (uint8_t*)&current_temp, sizeof(current_temp));
        default:
            break;
    }
}
unsigned char bluetooth_tx       = 0;
short current_reading = 0;


unsigned char writeCharacteristic()
{
    //This should be every 1 milisecond to obtain 1Khz


    return(1);
}
unsigned char readControlSignals()
{
    // write to the bluetooth characteristics:
    //- WRITE CURRENT READING
    return(1);
}
short readCurrent()
{
    current_reading = 5;
}

// ------ BLUETOOTH RELATED STATE MACHINE INFORMATION
enum BLE_Poll { SM1_INIT, SM1_POLL } SM1_State;

TickFct_State_machine_1() {
   switch(SM1_State) { // Transitions
      case -1:
         SM1_State = SM1_INIT;
         break;
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

         if(cyw43_arch_init())
             {
                 printf("Failed to initialize Bluetooth");
             }
             //initialize L2CAP, security manager
            l2cap_init();
            sm_init();
             // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
         
         
            att_server_init(profile_data,att_read_callback,att_write_callback);
         
            custom_service_server_init(characteristic_current_rx,characteristic_fan_ctrl_tx,characteristic_lights_ctrl_tx,characteristic_schedule_tx);
         
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
            set_characteristic_current(0);
            set_characteristic_fan_ctrl(0);
            set_characteristic_lights_ctrl(0);
            set_characteristic_schedule(100);
         
         break;
      case SM1_POLL:
             async_context_poll(cyw43_arch_async_context());
         
         break;
      default: // ADD default behaviour below
      break;
   } // State actions

//END BLUETOOTH RELATED CODE

//------TIMER RELATED CODE
short test_cnt = 0;
char led_on = 0;

bool repeating_timer_callback(__unused struct repeating_timer *t) {
    // printf("Repeat at %lld\n", time_us_64());
    test_cnt++;
    if(test_cnt % 1000 == 0)
    {
        led_on = ~led_on;

        // set_characteristic_current(led_on);

        // printf("\n");
        // printf("One second \'Samples\': %d\n",test_cnt);
        // printf("Current Value: %d\n",*service_object.characteristic_current_value);
        // printf("Fan Control: %s\n",service_object.characteristic_fan_ctrl_value);
        // printf("Light Control: %s\n",service_object.characteristic_lights_ctrl_value);
        // printf("Schedule: %s\n",service_object.characteristic_schedule_value);
        // printf("\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);

    }
    async_context_poll(cyw43_arch_async_context());
    // async_context_wait_for_work_until(cyw43_arch_async_context(), at_the_end_of_time);

    return true;
}
int main()
{
    
    //initialize standard in/out
    stdio_init_all();


    struct repeating_timer timer;
    add_repeating_timer_ms(1, repeating_timer_callback, NULL, &timer);
    sleep_ms(3000);
    
    while (true) {
        
    }
}
