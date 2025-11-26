//Custom Header
#include "bluetooth_setup.h"

// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "btstack.h"
// #include "pico/cyw43_arch.h"
// #include "pico/btstack_cyw43.h"
// #include "pico/sync.h"
// #include <stdint.h>

// Pico W devices use a GPIO on the WIFI chip for the LED
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

#define APP_AD_FLAGS 0x06




static uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    UNUSED(con_handle);
    //Current callback
    printf("Read Callback");
    //CURRENT
    if(attribute_handle == service_object.characteristic_current_handle)
    {
        return att_read_callback_handle_blob(service_object.characteristic_current_value, strlen(service_object.characteristic_current_value), offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_current_configuration_handle)
    {
        return att_read_callback_handle_little_endian_16(service_object.characteristic_current_client_configuration, offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_current_user_description_handle)
    {
        return att_read_callback_handle_blob(service_object.characteristic_current_user_description,strlen(service_object.characteristic_current_user_description), offset, buffer, buffer_size);
    }
    //FAN CONTROL
    if(attribute_handle == service_object.characteristic_fan_ctrl_handle)
    {
        return att_read_callback_handle_blob(service_object.characteristic_fan_ctrl_value, strlen(service_object.characteristic_fan_ctrl_value), offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_fan_ctrl_configuration_handle)
    {
        return att_read_callback_handle_little_endian_16(service_object.characteristic_fan_ctrl_client_configuration, offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_fan_ctrl_user_description_handle)
    {
        return att_read_callback_handle_blob(service_object.characteristic_fan_ctrl_user_description,strlen(service_object.characteristic_fan_ctrl_user_description), offset, buffer, buffer_size);
    }
    // LIGHT CONTROL
    if(attribute_handle == service_object.characteristic_lights_ctrl_handle)
    {
        return att_read_callback_handle_blob(service_object.characteristic_lights_ctrl_value, strlen(service_object.characteristic_lights_ctrl_value), offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_lights_ctrl_configuration_handle)
    {
        return att_read_callback_handle_little_endian_16(service_object.characteristic_lights_ctrl_client_configuration, offset, buffer, buffer_size);
    }
    if(attribute_handle == service_object.characteristic_lights_ctrl_user_description_handle)
    {
        return att_read_callback_handle_blob(service_object.characteristic_lights_ctrl_user_description,strlen(service_object.characteristic_lights_ctrl_user_description), offset, buffer, buffer_size);
    }

    // SCHEDULE
    if(attribute_handle == service_object.characteristic_schedule_handle)
    {
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
    UNUSED(transaction_mode);
    UNUSED(offset);
    UNUSED(buffer_size);
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

        //Alert application of bluetooth rx

    }
    

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


static void heartbeat_handler(struct btstack_timer_source *ts) {
    static uint32_t counter = 0;
    counter++;
    
    // Update the temp every 10s
    if(counter%10==0)
    {
        att_server_request_can_send_now_event(service_object.con_handle);
        printf("Current       :%s\n",service_object.characteristic_current_value);
        printf("Fan Control   :%s\n",service_object.characteristic_fan_ctrl_value);
        printf("Light Control :%s\n",service_object.characteristic_lights_ctrl_value);
        printf("Schedule      :%s\n",service_object.characteristic_schedule_value);
    }
    

    // Invert the led
    static int led_on = true;
    led_on = !led_on;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
    // Restart timer
    btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
    btstack_run_loop_add_timer(ts);
}

/*
int main()
{
    
    //initialize standard in/out
    stdio_init_all();

    //initilize CYW43
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
    heartbeat.process = &heartbeat_handler;
    btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
    btstack_run_loop_add_timer(&heartbeat);
    
      // turn on bluetooth!
    hci_power_control(HCI_POWER_ON);
    set_characteristic_current('5');
    set_characteristic_fan_ctrl('1');
    set_characteristic_lights_ctrl('1');
    set_characteristic_schedule('F');

    printf("%s",service_object.characteristic_current_value);
    while (true) {
        // printf("Hello, world!\n");
        // sleep_ms(100);
        // PACKET_HANDLER_FLAG = ~PACKET_HANDLER_FLAG;
        async_context_poll(cyw43_arch_async_context());
        // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, PACKET_HANDLER_FLAG);
        async_context_wait_for_work_until(cyw43_arch_async_context(), at_the_end_of_time);
    }
}
    */
