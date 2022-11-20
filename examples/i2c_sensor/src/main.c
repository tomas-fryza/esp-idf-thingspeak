/*
   Read temperature and humidity from DHT12 sensor, SLA = 0x5c.
   Xtensa dual-core 32-bit LX6 (ESP32-CAM), 240 MHz
   PlatformIO, ESP-IDF framework

   Copyright (c) 2022 Tomas Fryza
   Dept. of Radio Electronics, Brno University of Technology, Czechia
   This work is licensed under the terms of the GNU GENERAL PUBLIC LICENSE.
 */


/*-----------------------------------------------------------*/
#include <freertos/FreeRTOS.h>  // FreeRTOS
#include <freertos/task.h>
#include <esp_log.h>            // ESP_LOG/E/W/I functions
#include <driver/i2c.h>         // Inter-Integrated Circuit driver


/*-----------------------------------------------------------*/
//        ESP32-CAM
//   * 5V  Arduino     * 3.3V VCC DHT
//   * GND GND DHT     *
//   * 12              * 0    --+ Programming jumper
//   * 13  SDA DHT     * GND  --+
//   * 15  SCL DHT     * VCC
//   * 14              * U0R  Arduino Rx
//   * 2               * U0T  Arduino Tx
//   * 4               * GND  Arduino
//
//        Arduino Uno
//   Reset  --+ Jumper
//   GND    --+
#define I2C_MASTER_SDA_IO 13  // ESP32-CAM
#define I2C_MASTER_SCL_IO 15
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_ACK 0x0
#define I2C_NACK 0x1
#define I2C_DHT_ADDRESS 0x5c  // DHT12 temp & humidity sensor
#define I2C_DHT_HUMID 0x00    // 0x00 @ Humidity
#define I2C_DHT_TEMP 0x02     // 0x02 @ Temperature


/*-----------------------------------------------------------*/
// Used function(s)
void vTaskLoop();
void getTemp();
void getHumid();


/*-----------------------------------------------------------*/
/* In ESP-IDF instead of "main", we use "app_main" function
   where the program execution begins */
void app_main(void)
{
    ESP_LOGI("setup", "i2c sensor");

    // Configure i2c controller in master mode
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_NUM_0, &conf);
    ESP_LOGI("i2c", "i2c controller configured");

    // Install i2c driver
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    ESP_LOGI("i2c", "i2c driver installed");

    // Start the loop task
    xTaskCreate(vTaskLoop, "forever_loop", 2048, NULL, 5, NULL);
}


/*-----------------------------------------------------------*/
void vTaskLoop()
{
    ESP_LOGI("task", "loop task started");

    // Forever loop
    while (1) {
        getTemp();
        getHumid();
        // Delay 1 second
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // Delete this task if it exits from the loop above
    vTaskDelete(NULL);
}


/*-----------------------------------------------------------*/
void getTemp()
{
    uint8_t tempInt;
    uint8_t tempDec;

    // Create and execute i2c commands list
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DHT_ADDRESS<<1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, I2C_DHT_TEMP, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DHT_ADDRESS<<1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &tempInt, I2C_ACK);
    i2c_master_read_byte(cmd, &tempDec, I2C_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, (1000/portTICK_RATE_MS));
    i2c_cmd_link_delete(cmd);

    ESP_LOGI("i2c", "temperature: %d.%d °C", tempInt, tempDec);
}


/*-----------------------------------------------------------*/
void getHumid()
{
    uint8_t humidInt;
    uint8_t humidDec;

    // Create and execute i2c commands list
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DHT_ADDRESS<<1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, I2C_DHT_HUMID, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DHT_ADDRESS<<1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &humidInt, I2C_ACK);
    i2c_master_read_byte(cmd, &humidDec, I2C_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, (1000/portTICK_RATE_MS));
    i2c_cmd_link_delete(cmd);

    ESP_LOGI("i2c", "humidity: %d.%d", humidInt, humidDec);
}
