#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>
#include <math.h>
#include <stdint.h>          


// Constants
#define V_REF           3.3     // Pico ADC reference voltage
#define ADC_RESOLUTION  4095.0  // 12-bit ADC (Pico uses 0-4095 range)
#define OFFSET_VOLTAGE  1.65    // 0A output voltage for ACS712 (half of V_REF)
#define SENSITIVITY     0.185   // V/A for ACS712-5A model

float get_current(char adc_channel) {
    adc_select_input(adc_channel);
    short raw = adc_read();

    float voltage = (raw / ADC_RESOLUTION) * V_REF;
    float current = (voltage - OFFSET_VOLTAGE) / SENSITIVITY;
    return current;
}

int main() {
    stdio_init_all();

    adc_init();
    adc_gpio_init(26);  // Enable GP26/ADC0
    adc_select_input(0);

    while (true) {
        float current = get_current(0);
        printf("Current: %.2f A\n", current);
        sleep_ms(500);
    }
}

