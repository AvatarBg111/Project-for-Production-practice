#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO1          (5) // Define the output GPIO
#define LEDC_OUTPUT_IO2          (4) // Define the output GPIO
#define LEDC_CHANNEL1           LEDC_CHANNEL_0
#define LEDC_CHANNEL2           LEDC_CHANNEL_1
#define LEDC_DUTY_RES           LEDC_TIMER_12_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (250) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY          (50) // Frequency in Hertz. Set frequency at 5 kHz

void example_ledc_init(void);