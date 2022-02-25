#ifndef _MY_I2C_MAIN_H_
#define _MY_I2C_MAIN_H_


#include "driver/gpio.h"
#include "driver/i2c.h"

#define my_scl_gpio_num    (GPIO_NUM_27)
#define my_sda_gpio_num    (GPIO_NUM_14)
#define my_i2c_mum_0            I2C_NUM_0    //I2C的端口号0
#define my_i2c_mum_1             1UL    //I2C的端口号1
#define my_i2c_master_write_bit  0UL    //写入位置0
#define my_i2c_master_read_bit   1UL    //读取位置1
#define my_master_clk_speed_100k  100000UL  //时钟速率100k
#define my_master_clk_speed_400k  400000UL  //时钟速率400k
#define my_i2c_ack_check_en  1UL    //主机检查从机的ACK
#define my_i2c_ack_check_dis 0UL    //主机不检查从机的ACK
#define my_i2c_ack_value     0UL    //应答
#define my_i2c_no_ack_value  1UL    //不应答

esp_err_t my_i2c_init(void);    //I2C的初始化函数
esp_err_t my_i2c_sendByte(i2c_port_t i2c_port_mum,uint8_t slaveAddr, uint8_t byte); //I2C发送一个字节的函数
esp_err_t my_i2c_sendData(i2c_port_t i2c_port_mum,uint8_t slaveAddr, uint8_t *data, size_t dataLen);    //I2C发送多个字节的函数，字节大小取决有data
esp_err_t my_i2c_readData(i2c_port_t i2c_port_mum,uint8_t slaveAddr, uint8_t* data_rd, size_t dataLen);

#endif
