
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

    // Characteristic FAN_CONTROL information
    char *      characteristic_fan_ctrl_value ;
    uint16_t       characteristic_fan_ctrl_client_configuration ;
    char *      characteristic_fan_ctrl_user_description ;

    // Characteristic LIGHTS_CONTROL information
    char *      characteristic_lights_ctrl_value ;
    uint16_t      characteristic_lights_ctrl_client_configuration ;
    char *      characteristic_lights_ctrl_user_description ;

    // Characteristic LIGHTS_CONTROL_OVERRIDE information
    char *      characteristic_lights_ctrl_override_value ;
    uint16_t      characteristic_lights_ctrl_override_client_configuration ;
    char *      characteristic_lights_ctrl_override_user_description ;

    // Characteristic FAN_CONTROL_OVERRIDE information
    char *      characteristic_fan_ctrl_override_value ;
    uint16_t      characteristic_fan_ctrl_override_client_configuration ;
    char *      characteristic_fan_ctrl_override_user_description ;


    // Characteristic SCHEDULE information
    char *   characteristic_schedule_value ;
    uint16_t      characteristic_schedule_client_configuration ;
    char *      characteristic_schedule_user_description ;



    // Characteristic CURRENT Handles
    uint16_t characteristic_current_handle;
    uint16_t characteristic_current_configuration_handle;
    uint16_t characteristic_current_user_description_handle;
    //Characteristic FAN_CTRL Handles
    uint16_t characteristic_fan_ctrl_handle;
    uint16_t characteristic_fan_ctrl_configuration_handle;
    uint16_t characteristic_fan_ctrl_user_description_handle;
    //Characteristic LIGHT_CTRL Handles
    uint16_t characteristic_lights_ctrl_handle;
    uint16_t characteristic_lights_ctrl_configuration_handle;
    uint16_t characteristic_lights_ctrl_user_description_handle;

    // Characteristic LIGHT_CTRL_OVERRIDE Handles
    uint16_t characteristic_lights_ctrl_override_handle;
    uint16_t characteristic_lights_ctrl_override_configuration_handle;
    uint16_t characteristic_lights_ctrl_override_user_description_handle;
    // Characteristic FAN_CTRL_OVERRIDE Handles
    uint16_t characteristic_fan_ctrl_override_handle;
    uint16_t characteristic_fan_ctrl_override_configuration_handle;
    uint16_t characteristic_fan_ctrl_override_user_description_handle;

    //Characteristic SCHEDULE Handles
    uint16_t characteristic_schedule_handle;
    uint16_t characteristic_schedule_configuration_handle;
    uint16_t characteristic_schedule_user_description_handle;
    

    btstack_context_callback_registration_t callback_current;
    btstack_context_callback_registration_t callback_fan_ctrl;
    btstack_context_callback_registration_t callback_lights_ctrl;
    btstack_context_callback_registration_t callback_lights_ctrl_override;
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

static void characteristic_fan_ctrl_callback(void * context)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    custom_service * instance = (custom_service *) context;
    //send notification
    att_server_notify(instance->con_handle,instance->characteristic_fan_ctrl_handle,instance->characteristic_fan_ctrl_value,strlen(instance->characteristic_fan_ctrl_value));

}

static void characteristic_lights_ctrl_callback(void * context)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    custom_service * instance = (custom_service *) context;
    //send notification
    att_server_notify(instance->con_handle,instance->characteristic_lights_ctrl_handle,instance->characteristic_lights_ctrl_value,strlen(instance->characteristic_lights_ctrl_value));

}
static void characteristic_lights_ctrl_override_callback(void * context)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    custom_service * instance = (custom_service *) context;
    //send notification
    att_server_notify(instance->con_handle,instance->characteristic_lights_ctrl_override_handle,instance->characteristic_lights_ctrl_value,strlen(instance->characteristic_lights_ctrl_value));

}
static void characteristic_fan_ctrl_override_callback(void * context)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    custom_service * instance = (custom_service *) context;
    //send notification
    att_server_notify(instance->con_handle,instance->characteristic_fan_ctrl_override_handle,instance->characteristic_lights_ctrl_value,strlen(instance->characteristic_lights_ctrl_value));

}

static void characteristic_schedule_callback(void * context)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    custom_service * instance = (custom_service *) context;
    //send notification
    att_server_notify(instance->con_handle,instance->characteristic_schedule_handle,instance->characteristic_schedule_value,strlen(instance->characteristic_schedule_value));

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

static void set_characteristic_fan_ctrl(char value)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    custom_service * instance = &service_object;
    printf(instance->characteristic_fan_ctrl_value);
    sprintf(instance->characteristic_fan_ctrl_value,"%d",value);

    if(instance->characteristic_fan_ctrl_client_configuration)
    {
        instance->callback_fan_ctrl.callback = &characteristic_fan_ctrl_callback;
        instance->callback_fan_ctrl.context = (void*) instance;
        // sprintf(instance->characteristic_current_value, "%d", value) ;
        sprintf(instance->characteristic_fan_ctrl_value,"%d",value);
        printf(instance->characteristic_fan_ctrl_value);
        att_server_register_can_send_now_callback(&instance->callback_fan_ctrl,instance->con_handle);
    }
}

static void set_characteristic_lights_ctrl(char value)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    custom_service * instance = &service_object;
    printf(instance->characteristic_lights_ctrl_value);
    sprintf(instance->characteristic_lights_ctrl_value,"%d",value);

    if(instance->characteristic_lights_ctrl_client_configuration)
    {
        instance->callback_lights_ctrl.callback = &characteristic_lights_ctrl_callback;
        instance->callback_lights_ctrl.context = (void*) instance;
        // sprintf(instance->characteristic_current_value, "%d", value) ;
        sprintf(instance->characteristic_lights_ctrl_value,"%d",value);
        printf(instance->characteristic_lights_ctrl_value);
        att_server_register_can_send_now_callback(&instance->callback_lights_ctrl,instance->con_handle);
    }
}

static void set_characteristic_lights_ctrl(char value)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    custom_service * instance = &service_object;
    printf(instance->characteristic_lights_ctrl_value);
    sprintf(instance->characteristic_lights_ctrl_value,"%d",value);

    if(instance->characteristic_lights_ctrl_client_configuration)
    {
        instance->callback_lights_ctrl.callback = &characteristic_lights_ctrl_callback;
        instance->callback_lights_ctrl.context = (void*) instance;
        // sprintf(instance->characteristic_current_value, "%d", value) ;
        sprintf(instance->characteristic_lights_ctrl_value,"%d",value);
        printf(instance->characteristic_lights_ctrl_value);
        att_server_register_can_send_now_callback(&instance->callback_lights_ctrl,instance->con_handle);
    }
}

static void set_characteristic_lights_ctrl_override(char value)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    custom_service * instance = &service_object;
    printf(instance->characteristic_lights_ctrl_override_value);
    sprintf(instance->characteristic_lights_ctrl_override_value,"%d",value);

    if(instance->characteristic_lights_ctrl_override_client_configuration)
    {
        instance->callback_lights_ctrl_override.callback = &characteristic_lights_ctrl_override_callback;
        instance->callback_lights_ctrl_override.context = (void*) instance;
        // sprintf(instance->characteristic_current_value, "%d", value) ;
        sprintf(instance->characteristic_lights_ctrl_override_value,"%d",value);
        printf(instance->characteristic_lights_ctrl_override_value);
        att_server_register_can_send_now_callback(&instance->callback_lights_ctrl_override,instance->con_handle);
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


static void set_characteristic_schedule(char value)
{
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    custom_service * instance = &service_object;
    printf(instance->characteristic_schedule_value);
    sprintf(instance->characteristic_schedule_value,"%d",value);

    if(instance->characteristic_schedule_client_configuration)
    {
        instance->callback_schedule.callback = &characteristic_schedule_callback;
        instance->callback_schedule.context = (void*) instance;
        // sprintf(instance->characteristic_current_value, "%d", value) ;
        sprintf(instance->characteristic_schedule_value,"%d",value);
        printf(instance->characteristic_schedule_value);
        att_server_register_can_send_now_callback(&instance->callback_schedule,instance->con_handle);
    }
}



char characteristic_current[] = "Current Sensor";
char characteristic_fan_ctrl[] = "Fan Control";
char characteristic_lights_ctrl[] = "Light Control";
char characteristic_schedule[] = "Schedule";

void custom_service_server_init(char * ptr_current,char * ptr_fan_ctrl,char * ptr_lights_ctrl,char * ptr_schedule)
{

    custom_service * instance = &service_object;
    instance->characteristic_current_value     = ptr_current;
    instance->characteristic_fan_ctrl_value    = ptr_fan_ctrl;
    instance->characteristic_lights_ctrl_value = ptr_lights_ctrl;
    instance->characteristic_schedule_value    = ptr_schedule;

    instance->characteristic_current_user_description = characteristic_current;
    instance->characteristic_fan_ctrl_user_description = characteristic_fan_ctrl;
    instance->characteristic_lights_ctrl_user_description = characteristic_lights_ctrl;
    instance->characteristic_schedule_user_description = characteristic_schedule;

    //Tying handles from my_server.h to our more readable handles in struct
    instance->characteristic_current_handle = ATT_CHARACTERISTIC_3727f058_69dd_4d78_9b6f_9c2ebbb67d44_01_VALUE_HANDLE;
    instance->characteristic_current_configuration_handle = ATT_CHARACTERISTIC_3727f058_69dd_4d78_9b6f_9c2ebbb67d44_01_CLIENT_CONFIGURATION_HANDLE;
    instance->characteristic_current_user_description_handle = ATT_CHARACTERISTIC_3727f058_69dd_4d78_9b6f_9c2ebbb67d44_01_USER_DESCRIPTION_HANDLE;
    
    instance->characteristic_fan_ctrl_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c7_01_VALUE_HANDLE;
    instance->characteristic_fan_ctrl_configuration_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c7_01_CLIENT_CONFIGURATION_HANDLE;
    instance->characteristic_fan_ctrl_user_description_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c7_01_USER_DESCRIPTION_HANDLE;
    
    instance->characteristic_lights_ctrl_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c6_01_VALUE_HANDLE;
    instance->characteristic_lights_ctrl_configuration_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c6_01_CLIENT_CONFIGURATION_HANDLE;
    instance->characteristic_lights_ctrl_user_description_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c6_01_USER_DESCRIPTION_HANDLE;
//---
    instance->characteristic_fan_ctrl_override_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c7_01_VALUE_HANDLE;
    instance->characteristic_fan_ctrl_override_configuration_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c7_01_CLIENT_CONFIGURATION_HANDLE;
    instance->characteristic_fan_ctrl_override_user_description_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c7_01_USER_DESCRIPTION_HANDLE;
    
    instance->characteristic_lights_ctrl_override_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c6_01_VALUE_HANDLE;
    instance->characteristic_lights_ctrl_override_configuration_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c6_01_CLIENT_CONFIGURATION_HANDLE;
    instance->characteristic_lights_ctrl_override_user_description_handle = ATT_CHARACTERISTIC_74d31fe2_71d3_41c5_aa94_f15f9f61f3c6_01_USER_DESCRIPTION_HANDLE;


//---
    

    instance->characteristic_schedule_handle = ATT_CHARACTERISTIC_989dd69c_1be4_4b29_8bb4_ece912ddb61d_01_VALUE_HANDLE;
    instance->characteristic_schedule_configuration_handle = ATT_CHARACTERISTIC_989dd69c_1be4_4b29_8bb4_ece912ddb61d_01_CLIENT_CONFIGURATION_HANDLE;
    instance->characteristic_schedule_user_description_handle = ATT_CHARACTERISTIC_989dd69c_1be4_4b29_8bb4_ece912ddb61d_01_USER_DESCRIPTION_HANDLE;
    
}