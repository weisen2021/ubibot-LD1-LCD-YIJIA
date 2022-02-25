#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "nvs_flash.h"

#include "Uart0.h"
#include "HT9B95B.h"
#include "my_i2c_main.h"

void app_main(void)
{

    esp_err_t err;
    Uart_Init();
    ESP_LOGI("1", "ubibot-LD1-A2L!");
    my_i2c_init();
    ESP_LOGI("2", "ubibot-LD1-A2L!");
    HT9B95B_init(true);
    ESP_LOGI("3", "ubibot-LD1-A2L!");
    // esp_err_t ret;
    // ret = nvs_flash_init();
    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    // {
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     ret = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK(ret);
    while (1)
    {
        // err=HT9B95B_DisplayAll();
        // if (err<0)
        // {
            
        //     ESP_LOGI("Init", "HT9B95B DisplayAll fail");
        // }
        // else
        // {
        //     ESP_LOGI("Init", "HT9B95B DisplayAll OK");

        // }
        
        HT9B95B_Display_A_Leaklength_Data(107);
        HT9B95B_Display_B_Leaklength_Data(101);
        vTaskDelay(5000/portTICK_PERIOD_MS);
        HT9B95B_Display_A_Leaklength_Data(1135);

        HT9B95B_Display_B_Leaklength_Data(1256);
        vTaskDelay(5000/portTICK_PERIOD_MS);
        HT9B95B_Display_A_Leaklength_Data(17135);

        HT9B95B_Display_B_Leaklength_Data(51256);
        vTaskDelay(5000/portTICK_PERIOD_MS);

        
        // ESP_LOGI("Init", "ubibot-LD1-A2L!");
    }
       
}
