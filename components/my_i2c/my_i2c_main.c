#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "my_i2c_main.h"

static const char *TAG = "my_i2c";

/**
 * @brief my i2c master init
 */
esp_err_t my_i2c_init(void)
{
    esp_err_t err;
/* 安装驱动程序，并检查是否成功（err == ESP_OK） */
    
    i2c_config_t conf;
    memset(&conf, 0, sizeof conf);
    conf.clk_flags=0;//4.3 新增,I2C_SCLK_SRC_FLAG_FOR_NOMA 0,仅根据所需频率进行自动选择时钟。（不支持特殊功能，如 APB 等）;
    //I2C_SCLK_SRC_FLAG_AWARE_DFS 1,当 APB 时钟改变时，时钟的波特率不会改变;I2C_SCLK_SRC_FLAG_LIGHT_SLEEP 2 ,用于轻度睡眠模式. 
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = my_sda_gpio_num;
    conf.sda_pullup_en = true;
    conf.scl_io_num = my_scl_gpio_num;
    conf.scl_pullup_en = true;
    conf.master.clk_speed = 100000;
    err = i2c_param_config(my_i2c_mum_0, &conf);
    err = i2c_driver_install(my_i2c_mum_0 , conf.mode, 0, 0, 0);
    if(err != ESP_OK)
    {
        return err;
    }
    return err;
    
}

/*
 * @brief my i2c sendByte
 *参数说明：
    1、i2c_port_mum 是i2c的端口号;
    2、slaveAddr 是从机地址;
    3、byte 是写入的一个字节;
    返回值：
    如果发送成功则返回ESP_OK
    1、ESP_OK：成功
	2、ESP_ERR_INVALID_ARG : 参数错误
	3、ESP_FAIL：发送错误
	4、ESP_ERR_INVALID_STATE：I2C设备未初始化
	5、ESP_ERR_TIMEOUT：超时

   发送1个字节流程说明
  ___________________________________________________________________
 | start | slave_addr + wr_bit + ack | write 1 bytes + ack  | stop |
 --------|---------------------------|----------------------|------|

*/
esp_err_t my_i2c_sendByte(i2c_port_t i2c_port_mum,uint8_t slaveAddr, uint8_t byte)
{

    esp_err_t err;

    i2c_cmd_handle_t my_cmd = i2c_cmd_link_create();
    i2c_master_start(my_cmd);
    i2c_master_write_byte(my_cmd, (slaveAddr << 1) | my_i2c_master_write_bit, my_i2c_ack_check_en);
    i2c_master_write_byte(my_cmd, byte, my_i2c_ack_check_en);
    i2c_master_stop(my_cmd);
    err = i2c_master_cmd_begin(i2c_port_mum, my_cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(my_cmd);
    
    return err;
}
/*
 * @brief my i2c sendData 
 * 参数说明：
    1、i2c_port_mum 是i2c的端口号;
    2、slaveAddr 是从机地址;
    3、data 是写入的数据指针;
    4、dataLen 是写入数据的字节数
    返回值：
    如果发送成功则返回ESP_OK
    1、ESP_OK：成功
	2、ESP_ERR_INVALID_ARG : 参数错误
	3、ESP_FAIL：发送错误
	4、ESP_ERR_INVALID_STATE：I2C设备未初始化
	5、ESP_ERR_TIMEOUT：超时
    发送多字节流程说明
  ___________________________________________________________________
 | start | slave_addr + wr_bit + ack | write n bytes + ack  | stop |
 --------|---------------------------|----------------------|------|

*/
 esp_err_t my_i2c_sendData(i2c_port_t i2c_port_mum , uint8_t slaveAddr, uint8_t *data, size_t dataLen) 
{
   
    esp_err_t err;

    i2c_cmd_handle_t my_cmd = i2c_cmd_link_create();
    i2c_master_start(my_cmd );
    i2c_master_write_byte(my_cmd , (slaveAddr << 1) | my_i2c_master_write_bit, my_i2c_ack_check_en);
    i2c_master_write(my_cmd , data, dataLen, my_i2c_ack_check_en);
    i2c_master_stop(my_cmd );
    err = i2c_master_cmd_begin(i2c_port_mum, my_cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(my_cmd);
    ESP_LOGI(TAG, "send data ok");

    return err;
}
/**
 * @brief my i2c readByte
 */
esp_err_t my_i2c_readData(i2c_port_t i2c_port_num , uint8_t slaveAddr,uint8_t* data_rd, size_t dataLen)
{
    esp_err_t err;
    i2c_cmd_handle_t my_cmd = i2c_cmd_link_create();
    i2c_master_start(my_cmd);
    i2c_master_write_byte(my_cmd,(slaveAddr<<1)|my_i2c_master_read_bit,my_i2c_ack_check_en);
    if (dataLen > 1) 
    {
        i2c_master_read(my_cmd, data_rd, dataLen - 1, my_i2c_ack_value);
    }
    i2c_master_read_byte(my_cmd, data_rd + dataLen - 1, my_i2c_no_ack_value);
    i2c_master_stop(my_cmd);
    err = i2c_master_cmd_begin(i2c_port_num, my_cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(my_cmd);
    return err;



}
