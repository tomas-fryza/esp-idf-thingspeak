/*
   Blink a LED.
   Xtensa dual-core 32-bit LX6 (ESP32-CAM), 240 MHz
   PlatformIO, ESP-IDF framework

   Copyright (c) 2022 Tomas Fryza
   Dept. of Radio Electronics, Brno University of Technology, Czechia
   This work is licensed under the terms of the GNU GENERAL PUBLIC LICENSE.
 */


/*-----------------------------------------------------------*/
#include <freertos/FreeRTOS.h>  // FreeRTOS
#include <freertos/task.h>      // vTaskDelay, portTICK_PERIOD_MS
#include <driver/gpio.h>        // GPIO pins


/*-----------------------------------------------------------*/
// ESP32-CAM on-board LED(s): #33 (red, bottom side), #4 (Flash, top side)
#define BUILT_IN_LED 33


/*-----------------------------------------------------------*/
/* In ESP-IDF instead of "main", we use "app_main" function
   where the program execution begins */
void app_main()
{
    // Pin(s) configuration
    gpio_reset_pin(BUILT_IN_LED);
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);

    // Forever loop
    while (1) {
        gpio_set_level(BUILT_IN_LED, 1);        // Set high level
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay 1 second

        gpio_set_level(BUILT_IN_LED, 0);        // Set low level
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay 1 second
    }

    // Will never reach this
}
