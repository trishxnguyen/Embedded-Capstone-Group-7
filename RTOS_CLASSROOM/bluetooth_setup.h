
#include "btstack.h"
#include "pico/btstack_cyw43.h"
#include "my_server.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/btstack_cyw43.h"
#include "pico/sync.h"

#define HEARTBEAT_PERIOD_MS 1000


typedef struct {
    hci_con_handle_t con_handle;

    // Characteristic CURRENT information
    char *      characteristic_current_value;
    uint16_t    characteristic_current_client_configuration ;
    char *      characteristic_current_user_description ;

    // Characteristic CONTROL_OVERRIDE information
    char *      characteristic_fan_ctrl_override_value ;
    uint16_t      characteristic_fan_ctrl_override_client_configuration ;
    char *      characteristic_fan_ctrl_override_user_description ;



    // Characteristic CURRENT Handles
    uint16_t characteristic_current_handle;
    uint16_t characteristic_current_configuration_handle;
    uint16_t characteristic_current_user_description_handle;

    // Characteristic FAN_CTRL_OVERRIDE Handles
    uint16_t characteristic_fan_ctrl_override_handle;
    uint16_t characteristic_fan_ctrl_override_configuration_handle;
    uint16_t characteristic_fan_ctrl_override_user_description_handle;

    //Characteristic SCHEDULE Handles
    uint16_t characteristic_schedule_handle;
    uint16_t characteristic_schedule_configuration_handle;
    uint16_t characteristic_schedule_user_description_handle;
    

    btstack_context_callback_registration_t callback_current;

    btstack_context_callback_registration_t callback_fan_ctrl_override;

    btstack_context_callback_registration_t callback_schedule;


}custom_service;

//callback registration object
static att_service_handler_t service_handler;
static custom_service service_object;

static void characteristic_current_callback(void * context)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    custom_service * instance = (custom_service *) context;
    //send notification
    att_server_notify(instance->con_handle,instance->characteristic_current_handle,instance->characteristic_current_value,strlen(instance->characteristic_current_value));

}
static void characteristic_fan_ctrl_override_callback(void * context)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    custom_service * instance = (custom_service *) context;
    //send notification
    att_server_notify(instance->con_handle,instance->characteristic_fan_ctrl_override_handle,instance->characteristic_fan_ctrl_override_value,strlen(instance->characteristic_fan_ctrl_override_value));

}

static void set_characteristic_current(char value)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    custom_service * instance = &service_object;
    printf(instance->characteristic_current_value);
    sprintf(instance->characteristic_current_value,"%d",value);

    if(instance->characteristic_current_client_configuration)
    {
        instance->callback_current.callback = &characteristic_current_callback;
        instance->callback_current.context = (void*) instance;
        // sprintf(instance->characteristic_current_value, "%d", value) ;
        sprintf(instance->characteristic_current_value,"%d",value);
        // printf(instance->characteristic_current_value);
        att_server_register_can_send_now_callback(&instance->callback_current,instance->con_handle);
    }
}

static void set_characteristic_fan_ctrl_override(char value)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    custom_service * instance = &service_object;
    printf(instance->characteristic_fan_ctrl_override_value);
    sprintf(instance->characteristic_fan_ctrl_override_value,"%d",value);

    if(instance->characteristic_fan_ctrl_override_client_configuration)
    {
        instance->callback_fan_ctrl_override.callback = &characteristic_fan_ctrl_override_callback;
        instance->callback_fan_ctrl_override.context = (void*) instance;
        // sprintf(instance->characteristic_current_value, "%d", value) ;
        sprintf(instance->characteristic_fan_ctrl_override_value,"%d",value);
        printf(instance->characteristic_fan_ctrl_override_value);
        att_server_register_can_send_now_callback(&instance->callback_fan_ctrl_override,instance->con_handle);
    }
}


char characteristic_current[] = "Current Sensor";
char characteristic_fan_ctrl[] = "Fan Control";
char characteristic_lights_ctrl[] = "Light Control";
char characteristic_schedule[] = "Schedule";

void custom_service_server_init(char * ptr_current,char * ptr_fan_override_ctrl)
{

    custom_service * instance = &service_object;
    instance->characteristic_current_value     = ptr_current;
    instance->characteristic_fan_ctrl_override_value = ptr_fan_override_ctrl;


    instance->characteristic_current_user_description = characteristic_current;

    //Tying handles from my_server.h to our more readable handles in struct
    instance->characteristic_current_handle = ATT_CHARACTERISTIC_3727f058_69dd_4d78_9b6f_9c2ebbb67d44_01_VALUE_HANDLE;
    instance->characteristic_current_configuration_handle = ATT_CHARACTERISTIC_3727f058_69dd_4d78_9b6f_9c2ebbb67d44_01_CLIENT_CONFIGURATION_HANDLE;
    instance->characteristic_current_user_description_handle = ATT_CHARACTERISTIC_3727f058_69dd_4d78_9b6f_9c2ebbb67d44_01_USER_DESCRIPTION_HANDLE;
    
    
//---
    instance->characteristic_fan_ctrl_override_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c5_01_VALUE_HANDLE;
    instance->characteristic_fan_ctrl_override_configuration_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c5_01_USER_DESCRIPTION_HANDLE;
    instance->characteristic_fan_ctrl_override_user_description_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c5_01_USER_DESCRIPTION_HANDLE;
  
//---
 
}