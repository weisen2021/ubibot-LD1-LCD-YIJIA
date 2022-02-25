#ifndef _HT9B95B_H_
#define _HT9B95B_H_

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#define ACK_CHECK_EN 0x1  	/*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0 	/*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0       	/*!< I2C ack value */
#define NACK_VAL 0x1      	/*!< I2C nack value */

#define HT9B95B_ERROR_CODE              0xffff
#define HT9B95B_DISPLAY_NULL            0xff
#define HT9B95B_Write_Addr 0x7c 	//HT9B95B被写操作的地址
#define HT9B95B_Read_Addr 0x7d 	//HT9B95B被读操作的地址

#define HT9B95B_MAX_RAM_ADDR 0x15 //LCD的RAM最大有效地址

#define Driver_reset_CMD 0xf6 //驱动模式设置和软件复位命令,B型反转，执行软件复位
#define Duty_Bias_CMD 0xfd //Duty 和 Bias 设置命令
#define LCD_Voltage_CMD 0xc0 //设置电压为电源电压3.3V
#define Current_Frame_CMD  0xe2 //设置电流为正常电流，频率为80hz
#define Pixels_Set_CMD 0xf8 // 正常显示时所有像素
#define Driver_Display_CMD 0xf5 //B型反转，显示开启

#define HT9B95B_DISPLAY_ALL 0xFF
#define HT9B95B_DISPLAY_NONE 0x00
#define HT9B95B_DISPLAY_OFF 0xf6

#define RAMaddr0  0x00
#define RAMaddr1  0x01
#define RAMaddr2  0x02
#define RAMaddr3  0x03
#define RAMaddr4  0x04
#define RAMaddr5  0x05
#define RAMaddr6  0x06
#define RAMaddr7  0x07
#define RAMaddr8  0x08
#define RAMaddr9  0x09
#define RAMaddrA  0x0A
#define RAMaddrB  0x0B
#define RAMaddrC  0x0C
#define RAMaddrD  0x0D
#define RAMaddrE  0x0E
#define RAMaddrF  0x0F
#define RAMaddr10  0x10
#define RAMaddr11  0x11
#define RAMaddr12  0x12
#define RAMaddr13  0x13
#define RAMaddr14  0x14
#define RAMaddr15  0x15



extern void HT9B95B_init(bool lcd_reset);

esp_err_t HT9B95B_readCMD(i2c_port_t i2c_port_mum,uint8_t slaveAddr, uint8_t HT9B95B_CMD);
esp_err_t HT9B95B_Multiple_readCMD(i2c_port_t i2c_port_mum,uint8_t slaveAddr, uint8_t HT9B95B_CMD);

esp_err_t HT9B95B_sendData(i2c_port_t i2c_port_mum, uint8_t slavewriteAddr,uint8_t slavereadAddr,uint8_t RAMaddr, uint8_t *sendbyte);
esp_err_t HT9B95B_Multiple_sendData(i2c_port_t i2c_port_mum, uint8_t slavewriteAddr,uint8_t slavereadAddr,uint8_t RAMaddr, uint8_t *sendbyte);

esp_err_t HT9B95B_RAM_readData(i2c_port_t i2c_port_mum,uint8_t slaveAddr, uint8_t RAMaddr, uint8_t RAMbyte);
esp_err_t HT9B95B_Multiple_RAM_readData(i2c_port_t i2c_port_mum,uint8_t slaveAddr, uint8_t RAMaddr, uint8_t RAMbyte );

extern esp_err_t HT9B95B_Clear(void);
extern void HT9B95B_clear_up_area(void);
extern void HT9B95B_clear_down_area(void);
extern esp_err_t HT9B95B_DisplayAll(void);
extern esp_err_t HT9B95B_OFF(void);

extern void HT9B95B_Display_Ble_Open(int Ble_val);

extern void HT9B95B_Display_Wifi_Rssi_val(int Rssi_val);  //Display the Rssi value

extern void HT9B95B_Display_WiFi_Status(bool connect);  //Display the WiFi status

extern void HT9B95B_Display_Internet_Connect(bool net_status,bool net_all);  //Display the Internet Status

extern void HT9B95B_Display_Data_Post(bool post_status);  //Display the Data Post Status

extern void HT9B95B_Display_AP_Mode(bool ap_mode);  //Display the AP Mode Status

extern void HT9B95B_Display_SimCard_Status(bool sim_status);  //Display the SIM Card status

extern void HT9B95B_Display_SimCard_Rssi(float SimCard_rssi);  //Display the SimCard Rssi

extern void HT9B95B_Display_Ethernet_Status(bool lan_status);  //Display the Ethernet conneted status

extern void HT9B95B_Display_ErrCode(uint8_t err_code);  //Display the error code

extern void HT9B95B_Display_A_Point(bool point_show);

extern void HT9B95B_Display_A_Totallength_Data(uint32_t lengthdata); //显示A接口的总绳长

extern void HT9B95B_Display_B_Totallength_Data(uint32_t lengthdata);//显示B接口的总绳长

extern void HT9B95B_Display_A_Leaklength_Data(uint32_t lengthdata);//显示A接口的漏水位置

extern void HT9B95B_Display_B_Leaklength_Data(uint32_t lengthdata);//显示B接口的漏水位置

extern void HT9B95B_Display_A_Breaklength_Data(void);//显示A接口断线绳长为0

extern void HT9B95B_Display_B_Breaklength_Data(void);//显示B接口断线绳长为0

extern void HT9B95B_Display_A_Totallength(bool line_show);
extern void HT9B95B_Display_B_Totallength(bool line_show);

extern void HT9B95B_Display_A_Leak(bool leak_show);
extern void HT9B95B_Display_B_Leak(bool leak_show);

extern void HT9B95B_Display_A_Break(bool break_show);
extern void HT9B95B_Display_B_Break(bool break_show);

extern void HT9B95B_Display_A_Point(bool point_show);
extern void HT9B95B_Display_B_Point(bool point_show);

extern void HT9B95B_Display_Ext1(bool ext1_show);
extern void HT9B95B_Display_Ext2(bool ext2_show);

extern void HT9B95B_Display_A_Warning(bool warning_show);
extern void HT9B95B_Display_B_Warning(bool warning_show);

extern void  HT9B95B_Display_A_Unit(bool unit_show);
extern void  HT9B95B_Display_B_Unit(bool unit_show);

extern void HT9B95B_Display_CrossLine(bool cross_show);//显示中间的横线

#endif
