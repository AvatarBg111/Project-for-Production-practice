#include "test_pwm.h"

void example_ledc_init(void){
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel1 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL1,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO1,
        .duty           = 200, // Set duty to 0%
        .hpoint         = 0
    };

    ledc_channel_config_t ledc_channel2 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL2,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO2,
        .duty           = 300, // Set duty to 0%
        .hpoint         = 0
    };

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel1));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel2));
}