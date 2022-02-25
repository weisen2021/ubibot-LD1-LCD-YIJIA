#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c.h"


#include "my_i2c_main.h"
// #include "E2prom.h"
#include "HT9B95B.h"


static const char *TAG = "HT9B95B";

#define ESP_OK 0
#define ESP_FAIL -1


/**
 * @brief 主机向HT9B95B发送命令，写入
 * 
 * @param i2c_port_mum 
 * @param slaveAddr 
 * @param HT9B95B_CMD 
 * @return esp_err_t 
 */
esp_err_t HT9B95B_readCMD(i2c_port_t i2c_port_mum,uint8_t slaveAddr, uint8_t HT9B95B_CMD)
{
    esp_err_t err;

    i2c_cmd_handle_t my_cmd = i2c_cmd_link_create();
    i2c_master_start(my_cmd);
    i2c_master_write_byte(my_cmd , slaveAddr,ACK_CHECK_EN);
    i2c_master_write_byte(my_cmd,HT9B95B_CMD,ACK_CHECK_EN);
    i2c_master_stop(my_cmd);
    err = i2c_master_cmd_begin(i2c_port_mum, my_cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(my_cmd);
    
    return err;

}

/**
 * @brief 主机向HT9B95B多次（3）发送多次命令，写入
 * 
 * @param i2c_port_mum 
 * @param slaveAddr 
 * @param HT9B95B_CMD 
 * @return esp_err_t 
 */
esp_err_t HT9B95B_Multiple_readCMD(i2c_port_t i2c_port_mum,uint8_t slaveAddr, uint8_t HT9B95B_CMD)
{
    esp_err_t err;

    for (size_t i = 0; i < 3; i++)
    {
        if (HT9B95B_readCMD(i2c_port_mum,slaveAddr,HT9B95B_CMD) == ESP_OK)
        {
            err= ESP_OK ;
            break;
        } 
        else
        {
            err= ESP_FAIL ;
        }
        vTaskDelay(15/portTICK_RATE_MS);
    }
    return err ;
}

/**
 * @brief 主机从HT9B95B中读取数据
 * 
 * @param i2c_port_mum 
 * @param slavewriteAddr 
 * @param slavereadAddr 
 * @param RAMaddr 
 * @param sendbyte 
 * @param size 
 * @return esp_err_t 
 */
esp_err_t HT9B95B_sendData(i2c_port_t i2c_port_mum, uint8_t slavewriteAddr,uint8_t slavereadAddr,uint8_t RAMaddr, uint8_t *sendbyte)
{
    
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, slavewriteAddr, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, RAMaddr, ACK_CHECK_EN);
    // i2c_master_stop(cmd);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, slavereadAddr, ACK_CHECK_EN);

    i2c_master_read_byte(cmd, sendbyte , NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port_mum, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;

}

/**
 * @brief 主机从HT9B95B中多次（3）读取数据
 * 
 * @param i2c_port_mum 
 * @param slavewriteAddr 
 * @param slavereadAddr 
 * @param RAMaddr 
 * @param sendbyte 
 * @param size 
 * @return esp_err_t 
 */
esp_err_t HT9B95B_Multiple_sendData(i2c_port_t i2c_port_mum, uint8_t slavewriteAddr,uint8_t slavereadAddr,uint8_t RAMaddr, uint8_t *sendbyte)
{
    uint8_t send_val1,send_val2;
    esp_err_t err;
    for (size_t i = 0; i < 2; i++)
    {
        // vTaskDelay(1/portTICK_RATE_MS);
        HT9B95B_sendData(i2c_port_mum,slavewriteAddr,slavereadAddr,RAMaddr, &send_val1);
        ESP_LOGI(TAG,"%d,RAMaddr:%d",__LINE__,RAMaddr);
        // vTaskDelay(5/portTICK_RATE_MS);
        HT9B95B_sendData(i2c_port_mum,slavewriteAddr,slavereadAddr,RAMaddr, &send_val2);
        ESP_LOGI(TAG,"%d,send_val1:%x,send_val2:%x,RAMaddr:%d",__LINE__,send_val1,send_val2,RAMaddr);
        if (send_val1==send_val2)
        {
            *sendbyte = send_val1;
            err =ESP_OK;
            break;
        }
        else
        {
            err =ESP_FAIL;
            // break;
        }
        
    }
    return err;
}

/**
 * @brief 主机向HT9B95B发送显示RAM数据，写入
 * 
 * @param i2c_port_mum 
 * @param slaveAddr 
 * @param RAMaddr 
 * @param RAMbyte 
 * @return esp_err_t 
 */
esp_err_t HT9B95B_RAM_readData(i2c_port_t i2c_port_mum,uint8_t slaveAddr, uint8_t RAMaddr, uint8_t RAMbyte )
{
    esp_err_t err;

    // if (size == 0) 
    // {
    //     return ESP_OK;
    // }
    i2c_cmd_handle_t my_cmd = i2c_cmd_link_create();
    i2c_master_start(my_cmd);
    i2c_master_write_byte(my_cmd,slaveAddr,ACK_CHECK_EN);
    i2c_master_write_byte(my_cmd,RAMaddr,ACK_CHECK_EN);
    i2c_master_write_byte(my_cmd,RAMbyte,ACK_CHECK_EN);
    i2c_master_stop(my_cmd);
    err = i2c_master_cmd_begin(i2c_port_mum, my_cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(my_cmd);
    
    return err;
    
}

/**
 * @brief 主机向HT9B95B多次（3）发送显示RAM数据,写入
 * 
 * @param i2c_port_mum 
 * @param slaveAddr 
 * @param RAMaddr 
 * @param RAMbyte 
 * @return esp_err_t 
 */
esp_err_t HT9B95B_Multiple_RAM_readData(i2c_port_t i2c_port_mum,uint8_t slaveAddr, uint8_t RAMaddr, uint8_t RAMbyte )
{
    uint8_t send_val;
    esp_err_t err;
    for (size_t i = 0; i < 2; i++)
    {
       err=HT9B95B_RAM_readData( i2c_port_mum, slaveAddr, RAMaddr, RAMbyte );
    //    vTaskDelay(3/portTICK_RATE_MS);
       ESP_LOGI(TAG,"%d,RAMbyte:%x,RAMaddr:%d",__LINE__,RAMbyte,RAMaddr);
    //    if (err==ESP_OK)
    //    {
    //        break;
    //    }

       HT9B95B_Multiple_sendData(i2c_port_mum,0x7C,0x7D,RAMaddr, &send_val);
       ESP_LOGI(TAG,"%d,send_val:%x,RAMaddr:%d",__LINE__,send_val,RAMaddr);
       if (send_val==RAMbyte)
       {
           err=ESP_OK;
           break;
       }
       else
       {
           err=ESP_FAIL;
        //    break;
       } 
    }
    return err;
}

/**
 * @brief 清空显示中的RAM
 * 
 * @return esp_err_t 
 */
esp_err_t HT9B95B_Clear(void)
{
  uint8_t ram_addr;
  esp_err_t err;
  
  for(ram_addr=0;ram_addr<=HT9B95B_MAX_RAM_ADDR;ram_addr++)
  {
    
    err=HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,ram_addr,HT9B95B_DISPLAY_NONE);
    ESP_LOGI(TAG,"%d,HT9B95Bclear:%d",__LINE__,err);
    vTaskDelay(3/portTICK_PERIOD_MS);   
  }
  return err;
}

/**
 * @brief 显示所有的段码
 * 
 * @return esp_err_t 
 */
esp_err_t HT9B95B_DisplayAll(void)
{
  uint8_t ram_addr;
  esp_err_t err;
  
  for(ram_addr=0;ram_addr<=HT9B95B_MAX_RAM_ADDR;ram_addr++)
  {
    
    err=HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,ram_addr,HT9B95B_DISPLAY_ALL);
    vTaskDelay(3/portTICK_PERIOD_MS);   
  }
  return err;
}

/**
 * @brief 关闭显示屏幕
 * 
 */
esp_err_t HT9B95B_OFF(void)
{
    esp_err_t  err;

    err=HT9B95B_Multiple_readCMD(my_i2c_mum_0,HT9B95B_Write_Addr, HT9B95B_DISPLAY_OFF);
    
    vTaskDelay(2/portTICK_PERIOD_MS);

    return err;

}

/**
 * @brief 显示蓝牙打开
 * 
 * @param Ble_val  0关闭，1打开
 */
void HT9B95B_Display_Ble_Open(int Ble_val)
{
    uint8_t reg_val;

    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr7,&reg_val);

    reg_val=reg_val&0xbf ;//T4

    if(Ble_val)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr7,reg_val|0x40);
        //display T4
    }
    else
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr7,reg_val);
        //not display T4
    }

}

/**
 * @brief WIFI的连接状态
 * 
 * @param connect 0断开，1连接
 */
void HT9B95B_Display_WiFi_Status(bool connect)
{
    uint8_t reg_val;

    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr6,&reg_val);

    if(connect)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,0x7f&reg_val);
        //not display T17
    }
    else
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,0x80|reg_val);
        //display T17
    }
}

/**
 * @brief 显示wifi的信号值
 * 
 * @param Rssi_val 
 */
void HT9B95B_Display_Wifi_Rssi_val(int Rssi_val)
{

    uint8_t reg_val;
    
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr5,&reg_val);

    reg_val = reg_val&0xf0;  // T13 T14 T15 T16

    if(Rssi_val == HT9B95B_ERROR_CODE)
    {
        //not dispaly T13 T14 T15 T16
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val);

    }
    if((Rssi_val<0)&&(Rssi_val>-120))
    {
        if(Rssi_val<=-85)
        {
            //dispaly T16, not dispaly T13 T14 T15
            HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val|0x08);

        }
        else if(Rssi_val<=-75)
        {
            //dispaly T16 T15, not dispaly T13 T14 
            HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val|0x0C);

        }
        else if(Rssi_val<=-65)
        {
            //dispaly T16 T15 T14 , not dispaly T13 
            HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val|0x0E);

        }
        else
        {
            //dispaly T13 T14 T15 T16
            HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val|0x0F);

        }
    }
    else
    {
        //not dispaly T13 T14 T15 T16
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val);

    }

}

/**
 * @brief 显示网络连接状态
 * (0,0)，（1，0）都不显示，（0，1）都显示，（1，1）显示T11
 * @param net_status 0断开，1连接 0,0
 * @param net_all 0断开，1连接
 */
void HT9B95B_Display_Internet_Connect(bool net_status,bool net_all)
{
    uint8_t reg_val;
    
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr6,&reg_val);
    
    reg_val = reg_val&0x9F;  //T11 T12 

    if(net_all)
    {
        if(net_status)
        {
            HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,reg_val|0x20);
            //display T11
    
        }
        else
        {
            HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,reg_val|0x60);
            //display T11 T12
            
        }
    }
    else
    {
        //not display T11 T12
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,reg_val);
        
    }
}

/**
 * @brief 显示正在发送数据
 * 
 * @param post_status 1post,0 not post
 */
void HT9B95B_Display_Data_Post(bool post_status)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr6,&reg_val);
    
    reg_val = reg_val&0xef;  //T10 

    if(post_status)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,reg_val|0x10);
        //display T10 
    }
    else
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,reg_val);
        //not display T10
    }

}

/**
 * @brief 显示AP模式的图标
 * 
 * @param ap_mode 
 */
void HT9B95B_Display_AP_Mode(bool ap_mode)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr7,&reg_val);
    
    reg_val = reg_val&0xdf;  //T3

    if(ap_mode)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr7,reg_val|0x20);
        //display T3
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr7,reg_val);  
        //not display T3
    }
}

/**
 * @brief 显示sim卡图标
 * 
 * @param sim_status 
 */
void HT9B95B_Display_SimCard_Status(bool sim_status)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr7,&reg_val);
    
    reg_val = reg_val&0x7f;  //T5

    if(sim_status)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr7,reg_val|0x80);
        //display T5
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr7,reg_val);  
        //not display T5
    }

}

/**
 * @brief 显示sim卡信号数值
 * 
 * @param SimCard_rssi 
 */
void HT9B95B_Display_SimCard_Rssi(float SimCard_rssi)
{
    uint8_t reg_val;
    
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr6,&reg_val);

    reg_val = reg_val&0xf0;  // T6 T7 T8 T9

    if(SimCard_rssi == HT9B95B_ERROR_CODE)
    {
        //not dispaly T6 T7 T8 T9
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,reg_val);

    }

    if ((SimCard_rssi>31)||(SimCard_rssi<=0))
    {
        //not dispaly T6 T7 T8 T9
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,reg_val);
    }
    else if(SimCard_rssi<=4)
    {
        //dispaly T6, not dispaly T7 T8 T9
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,reg_val|0x08);

    }
    else if(SimCard_rssi<=9)
    {
        //dispaly T6 T7, not dispaly T8 T9 
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,reg_val|0x0C);

    }
    else if(SimCard_rssi<=14)
    {
        //dispaly T6 T7 T8 , not dispaly T9  
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,reg_val|0x0E);

    }
    else if(SimCard_rssi<=31)
    {
        //dispaly T6 T7 T8 T9
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr6,reg_val|0x0F);

    }

}

/**
 * @brief 显示网线物理连接状态
 * 
 * @param lan_status 
 */
void HT9B95B_Display_Ethernet_Status(bool lan_status)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr7,&reg_val);
    
    reg_val = reg_val&0xef;  //T2

    if(lan_status)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr7,reg_val|0x10);
        //display T2
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr7,reg_val);  
        //not display T2
    }

}

/**
 * @brief 显示屏幕中编号为1和2的图形
 * 
 * @param ram_addr 
 * @param data_val 
 * @param Top_flag
 */
void HT9B95B_Display_NUM_1_2(uint8_t ram_addr,uint8_t data_val,bool Top_flag)
{
    uint8_t reg_val1,reg_val2;

    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,ram_addr-1,&reg_val1);
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,ram_addr,&reg_val2);

    reg_val1 = reg_val1&0xf8;

    reg_val2 = reg_val2&0x0f;

    if(Top_flag)
    {
        reg_val1 = reg_val1|0x08;
    }
    else
    {
        reg_val1 = reg_val1&0xf7;
    }

    switch(data_val)
    {
        case 0:  //A B C D E F
        {
            reg_val1 = reg_val1|0x05;  //B C

            reg_val2 = reg_val2|0xf0;  //D E F A
        }
        break;

        case 1:  //B C
        {
            reg_val1 = reg_val1|0x05;  //B C
        }
        break;

        case 2:  //A B G E D
        {
            reg_val1 = reg_val1|0x03;  //B G 0011

            reg_val2 = reg_val2|0xD0;  //D E A 1101
        }
        break;

        case 3:  //A B G C D 
        {
            reg_val1 = reg_val1|0x07;  // C G B 0111

            reg_val2 = reg_val2|0x90;  // D A 1001
        }
        break;

        case 4:  //F G B C 
        {
            reg_val1 = reg_val1|0x07;  //G C B 0111

            reg_val2 = reg_val2|0x20;  //F  0010
        }
        break;

        case 5:  //A F G C D
        {
            reg_val1 = reg_val1|0x06;  //C G 0110

            reg_val2 = reg_val2|0xB0;  // D F A 1011
        }
        break;

        case 6:  //A F E D C G  
        {
            reg_val1 = reg_val1|0x06;  //C G 0110

            reg_val2 = reg_val2|0xF0;  //D E F A 1111
        }
        break;

        case 7:  //A B C
        {
            reg_val1 = reg_val1|0x05;  //C B 0101
            reg_val2 = reg_val2|0x10;  //A 0001
        }
        break;

        case 8:  //A F G C D E B
        {
            reg_val1 = reg_val1|0x07;  //C G B 0111

            reg_val2 = reg_val2|0xf0;  //D E F A 1111
        }
        break;

        case 9:  //A F G B C D
        {
            reg_val1 = reg_val1|0x07;  //C G B 0111

            reg_val2 = reg_val2|0xB0;  // D F A 1011
        }
        break;

        default:
        break;
    }
    
    HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,ram_addr-1,reg_val1);
    
    HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,ram_addr,reg_val2);

}

/**
 * @brief 显示错误代码
 * 
 * @param err_code 
 */
void HT9B95B_Display_ErrCode(uint8_t err_code)
{

    uint8_t f_val,s_val;

    if(err_code)
    {
        f_val=err_code/10;

        s_val=err_code%10;
        
        HT9B95B_Display_NUM_1_2(RAMaddrD,f_val,1);//Number 1/T1

        HT9B95B_Display_NUM_1_2(RAMaddrC,s_val,1);  //Number 2
        
    }
    else
    {
        HT9B95B_Display_NUM_1_2(RAMaddrD,0,1);//Number 1/T1

        HT9B95B_Display_NUM_1_2(RAMaddrC,0,1);  //Number 2

    }

}

/**
 * @brief 显示屏幕中编号为3、4、5、6、7的图形
 * 
 * @param ram_addr 
 * @param data_val 
 * @param end_flag 
 */
void HT9B95B_Display_NUM_3_4_5_6_7(uint8_t ram_addr,uint8_t data_val,bool end_flag)
{
    uint8_t reg_val1,reg_val2;

    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,ram_addr-1,&reg_val1);
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,ram_addr,&reg_val2);

    reg_val1 = reg_val1&0xf1;

    reg_val2 = reg_val2&0x0f;

    if(end_flag)
    {
        reg_val1 = reg_val1|0x01;
    }
    else
    {
        reg_val1 = reg_val1&0xfe;
    }

    switch(data_val)
    {
        case 0:  //A B C D E F
        {
            reg_val1 = reg_val1|0x0A;  //B C 1010

            reg_val2 = reg_val2|0xf0;  //A F E D 1111
        }
        break;

        case 1:  //B C
        {
            reg_val1 = reg_val1|0x0A;  //B C 1010
        }
        break;

        case 2:  //A B G E D
        {
            reg_val1 = reg_val1|0x0C;  //B G 1100

            reg_val2 = reg_val2|0xB0;  //A E D  1011
        }
        break;

        case 3:  //A B G C D 
        {
            reg_val1 = reg_val1|0x0E;  // B G C 1110

            reg_val2 = reg_val2|0x90;  // A D 1001
        }
        break;

        case 4:  //F G B C 
        {
            reg_val1 = reg_val1|0x0E;  //B G C 1110

            reg_val2 = reg_val2|0x40;  //F  0100
        }
        break;

        case 5:  //A F G C D
        {
            reg_val1 = reg_val1|0x06;  //G C 0110

            reg_val2 = reg_val2|0xD0;  // A F D 1101
        }
        break;

        case 6:  //A F E D C G  
        {
            reg_val1 = reg_val1|0x06;  // G C 0110

            reg_val2 = reg_val2|0xF0;  //A F E D  1111
        }
        break;

        case 7:  //A B C
        {
            reg_val1 = reg_val1|0x0A;  //B C 1010
            reg_val2 = reg_val2|0x80;  //A 1000
        }
        break;

        case 8:  //A F G C D E B
        {
            reg_val1 = reg_val1|0x0E;  //B G C 1110

            reg_val2 = reg_val2|0xf0;  //A F E D 1111
        }
        break;

        case 9:  //A F G B C D
        {
            reg_val1 = reg_val1|0x0E;  //B G C 1110

            reg_val2 = reg_val2|0xD0;  // A F D  1101
        }
        break;

        default:
        break;
    }
    
    HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,ram_addr-1,reg_val1);
    
    HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,ram_addr,reg_val2);

}

/**
 * @brief 显示屏幕中编号为8_9_10_11_12的图形
 * 
 * @param ram_addr 
 * @param data_val 
 * @param Top_flag
 */
void HT9B95B_Display_NUM_8_9_10_11_12(uint8_t ram_addr,uint8_t data_val,bool Top_flag)
{
    uint8_t reg_val;

    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,ram_addr,&reg_val);

    reg_val = reg_val&0x80;

    if(Top_flag)
    {
        reg_val = reg_val|0x80;
    }
    else
    {
        reg_val = reg_val&0x00;
    }

    switch(data_val)
    {
        //C G B D E F A
        case 0:  //A B C D E F
        {
            reg_val = reg_val|0x5F;  // C B D E F A 0101 1111    
        }
        break;

        case 1:  //B C
        {
            reg_val = reg_val|0x50;  //C B 0101 0000
        }
        break;

        case 2:  //A B G E D
        {
            reg_val = reg_val|0x3D;  //G B D E A  0011 1101   
        }
        break;

        case 3:  //A B G C D 
        {
            reg_val = reg_val|0x79;  // C G B D A  0111 1001

            
        }
        break;

        case 4:  //F G B C 
        {
            reg_val = reg_val|0x72;  //C G B F 0111 0010

            
        }
        break;

        case 5:  //A F G C D
        {
            reg_val = reg_val|0x6B;  //C G D F A 0110 1011    
        }
        break;

        case 6:  //A F E D C G  
        {
            reg_val = reg_val|0x6F;  // C G D E F A 0110 1111   
        }
        break;

        case 7:  //A B C
        {
            reg_val = reg_val|0x51;  //C B A  0101 0001
            
        }
        break;

        case 8:  //A F G C D E B
        {
            reg_val = reg_val|0x7F;  //C G B D E F A 0111 1111

            
        }
        break;

        case 9:  //A F G B C D
        {
            reg_val = reg_val|0x7B;  //C G B D F A 0111 1011  
        }
        break;

        default:
        break;
    }
       
    HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,ram_addr,reg_val);

}

/**
 * @brief 显示A接口的总绳长图标
 * 
 * @param line_show
 */
void HT9B95B_Display_A_Totallength(bool line_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr12,&reg_val);
    
    reg_val = reg_val&0xf7;  //T18

    if(line_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr12,reg_val|0x08);
        //display T18
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr12,reg_val);  
        //not display T18
    }

}

/**
 * @brief 显示B接口的总绳长图标
 * 
 * @param line_show
 */
void HT9B95B_Display_B_Totallength(bool line_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr5,&reg_val);
    
    reg_val = reg_val&0xDF;  //T21

    if(line_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val|0x20);
        //display T21
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val);  
        //not display T21
    }

}

/**
 * @brief 显示A接口的漏水图标
 * 
 * @param leak_show
 */
void HT9B95B_Display_A_Leak(bool leak_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr12,&reg_val);
    
    reg_val = reg_val&0xFB;  //T19

    if(leak_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr12,reg_val|0x04);
        //display T19
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr12,reg_val);  
        //not display T19
    }

}

/**
 * @brief 显示B接口的漏水图标
 * 
 * @param leak_show
 */
void HT9B95B_Display_B_Leak(bool leak_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr5,&reg_val);
    
    reg_val = reg_val&0xBF;  //T22

    if(leak_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val|0x40);
        //display T22
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val);  
        //not display T22
    }

}

/**
 * @brief 显示A接口的断线图标
 * 
 * @param  break_show
 */
void HT9B95B_Display_A_Break(bool break_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr12,&reg_val);
    
    reg_val = reg_val&0xFD;  //T20

    if(break_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr12,reg_val|0x02);
        //display T20
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr12,reg_val);  
        //not display T20
    }

}

/**
 * @brief 显示B接口的断线图标
 * 
 * @param break_show
 */
void HT9B95B_Display_B_Break(bool break_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr5,&reg_val);
    
    reg_val = reg_val&0x7F;  //T23

    if(break_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val|0x80);
        //display T23
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val);  
        //not display T23
    }

}

/**
 * @brief 显示A接口的小数点图标
 * 
 * @param point_show
 */
void HT9B95B_Display_A_Point(bool point_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddrF,&reg_val);
    ESP_LOGI("A_Point","%x",reg_val);
    reg_val = reg_val & 0xfe;  //p1
    
    ESP_LOGI("A_Point","%x",reg_val);

    if(point_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddrF,reg_val|0x01);
        // HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddrF,0XFF);
        ESP_LOGI("A_Point","display");
        //display p1
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddrF,reg_val); 
        // HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddrF,0xfe); 
        ESP_LOGI("A_Point","not display");
        //not display p1
    }

}

/**
 * @brief 显示B接口的小数点图标
 * 
 * @param point_show
 */
void HT9B95B_Display_B_Point(bool point_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr2,&reg_val);
    
    reg_val = reg_val&0x7F;  //p2

    if(point_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr2,reg_val|0x80);
        //display p2
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr2,reg_val);  
        //not display p2
    }

}

/**
 * @brief 显示A接口的物理连接图标
 * 
 * @param ext1_show
 */
void HT9B95B_Display_Ext1(bool ext1_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddrD,&reg_val);
    
    reg_val = reg_val&0xfe;  //T24

    if(ext1_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddrD,reg_val|0x01);
        //display T24
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddrD,reg_val);  
        //not display T24
    }

}

/**
 * @brief 显示B接口的物理连接图标
 * 
 * @param ext2_show
 */
void HT9B95B_Display_Ext2(bool ext2_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr0,&reg_val);
    
    reg_val = reg_val&0x7f;  //T28

    if(ext2_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr0,reg_val|0x80);
        //display T28
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr0,reg_val);  
        //not display T28
    }

}

/**
 * @brief 显示A接口的警告图标
 * 
 * @param warning_show
 */
void HT9B95B_Display_A_Warning(bool warning_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddrE,&reg_val);
    
    reg_val = reg_val&0xFE;  //T25

    if(warning_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddrE,reg_val|0x01);
        //display T25
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddrE,reg_val);  
        //not display T25
    }

}

/**
 * @brief 显示B接口的警告图标
 * 
 * @param warning_show
 */
void HT9B95B_Display_B_Warning(bool warning_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr1,&reg_val);
    
    reg_val = reg_val&0x7f;  //T29

    if(warning_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr1,reg_val|0x80);
        //display T29
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr1,reg_val);  
        //not display T29
    }

}

/**
 * @brief 显示A接口的单位图标
 * 
 * @param unit_show
 */
void HT9B95B_Display_A_Unit(bool unit_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr10,&reg_val);
    
    reg_val = reg_val&0xfe;  //T26

    if(unit_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr10,reg_val|0x01);
        //display T26
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr10,reg_val);  
        //not display T26
    }

}

/**
 * @brief 显示B接口的单位图标
 * 
 * @param unit_show
 */
void HT9B95B_Display_B_Unit(bool unit_show)
{
    uint8_t reg_val;
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr3,&reg_val);
    
    reg_val = reg_val&0x7f;  //T30

    if(unit_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr3,reg_val|0x80);
        //display T30
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr3,reg_val);  
        //not display T30
    }

}

/**
 * @brief 显示屏幕中间横线
 * 
 * @param cross_show
 */
void HT9B95B_Display_CrossLine(bool cross_show)
{
    uint8_t reg_val;
    // HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr5,&reg_val);
    HT9B95B_Multiple_sendData(my_i2c_mum_0,HT9B95B_Write_Addr,HT9B95B_Read_Addr,RAMaddr5,&reg_val);
    ESP_LOGI("CrossLine","%x",reg_val);
    reg_val = reg_val&0xef;  //T27
    ESP_LOGI("CrossLine","%x",reg_val);

    if(cross_show)
    {
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val|0x10);
        ESP_LOGI(TAG,"%d,Display_CrossLine",__LINE__);
        //display T27
    }
    else
    {    
        HT9B95B_Multiple_RAM_readData(my_i2c_mum_0,HT9B95B_Write_Addr,RAMaddr5,reg_val);  
        //not display T27
    }

}

/**
 * @brief 清空上半屏幕的显示
 * 
 */
void HT9B95B_clear_up_area(void)
{
    HT9B95B_Display_A_Totallength(0);
    HT9B95B_Display_A_Leak(0);
    HT9B95B_Display_A_Break(0);
    HT9B95B_Display_A_Point(0);
    HT9B95B_Display_Ext1(0);
    HT9B95B_Display_A_Warning(0);
    HT9B95B_Display_A_Unit(0);
    HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr12,HT9B95B_DISPLAY_NULL,0);
    HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr11,HT9B95B_DISPLAY_NULL,0);
    HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr10,HT9B95B_DISPLAY_NULL,0);
    HT9B95B_Display_NUM_3_4_5_6_7(RAMaddrF,HT9B95B_DISPLAY_NULL,0);
    HT9B95B_Display_NUM_3_4_5_6_7(RAMaddrE,HT9B95B_DISPLAY_NULL,0);

}

/**
 * @brief 清空下半屏幕的显示
 * 
 */
void HT9B95B_clear_down_area(void)
{
    HT9B95B_Display_B_Totallength(0);
    HT9B95B_Display_B_Leak(0);
    HT9B95B_Display_B_Break(0);
    HT9B95B_Display_B_Point(0);
    HT9B95B_Display_Ext2(0);
    HT9B95B_Display_B_Warning(0);
    HT9B95B_Display_B_Unit(0);
    HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr4,HT9B95B_DISPLAY_NULL,0);
    HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr3,HT9B95B_DISPLAY_NULL,0);
    HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr2,HT9B95B_DISPLAY_NULL,0);
    HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr1,HT9B95B_DISPLAY_NULL,0);
    HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr0,HT9B95B_DISPLAY_NULL,0);
}


/**
 * @brief 显示A接口的总绳长
 * 
 * @param lengthdata 
 */
void HT9B95B_Display_A_Totallength_Data(uint32_t lengthdata)
{
    uint32_t uints_val,tens_val, hundreds_val;
    uint32_t data_val;


    HT9B95B_clear_up_area();

    HT9B95B_Display_A_Totallength(1);
    HT9B95B_Display_Ext1(1);
    HT9B95B_Display_A_Unit(1);
    HT9B95B_Display_A_Point(1);
    HT9B95B_Display_NUM_3_4_5_6_7(RAMaddrF,0,0);
    HT9B95B_Display_NUM_3_4_5_6_7(RAMaddrE,0,1);
    if (lengthdata<10)
    {
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr12,HT9B95B_DISPLAY_NULL,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr11,HT9B95B_DISPLAY_NULL,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr10,lengthdata,1);
    }
    else if (lengthdata<100)
    {
        uints_val=lengthdata%10;
        tens_val=lengthdata/10;
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr12,HT9B95B_DISPLAY_NULL,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr11,tens_val,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr10,uints_val,1);
    }
    else if (lengthdata<1000)
    {
        hundreds_val=lengthdata/100;
        data_val=lengthdata%100;
        tens_val=data_val/10;
        uints_val=data_val%10;
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr12,hundreds_val,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr11,tens_val,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr10,uints_val,1);
    }
    
}

/**
 * @brief 显示B接口的总绳长
 * 
 * @param lengthdata 
 */
void HT9B95B_Display_B_Totallength_Data(uint32_t lengthdata)
{
    uint32_t uints_val, tens_val, hundreds_val ;
    uint32_t data_val;


    HT9B95B_clear_down_area();

    HT9B95B_Display_B_Totallength(1);
    HT9B95B_Display_Ext2(1);
    HT9B95B_Display_B_Unit(1);
    HT9B95B_Display_B_Point(1);
    HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr1,0,0);
    HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr0,0,1);
    if (lengthdata<10)
    {
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr4,HT9B95B_DISPLAY_NULL,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr3,HT9B95B_DISPLAY_NULL,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr2,lengthdata,1);
    }
    else if (lengthdata<100)
    {
        tens_val=lengthdata/10;
        uints_val=lengthdata%10;
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr4,HT9B95B_DISPLAY_NULL,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr3,tens_val,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr2,uints_val,1);
    }
    else if (lengthdata<1000)
    {
        hundreds_val=lengthdata/100;
        data_val=lengthdata%100;
        tens_val=data_val/10;
        uints_val=data_val%10;
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr4,hundreds_val,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr3,tens_val,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr2,uints_val,1);
    }
    
}

/**
 * @brief 显示A接口的漏水位置
 * 
 * @param lengthdata 
 */
void HT9B95B_Display_A_Leaklength_Data(uint32_t lengthdata)
{
    uint32_t uints_val ,tens_val, hundreds_val ,Two_decimal_val ,one_decimal_val;
    uint32_t data_val;


    HT9B95B_clear_up_area();

    HT9B95B_Display_A_Leak(1);
    HT9B95B_Display_A_Warning(1);
    HT9B95B_Display_Ext1(1);
    HT9B95B_Display_A_Unit(1);
    HT9B95B_Display_A_Point(1);

    if (lengthdata<1000)
    {
        uints_val=lengthdata/100;
        data_val=lengthdata%100;
        one_decimal_val=data_val/10;
        Two_decimal_val=data_val%10;
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr12,HT9B95B_DISPLAY_NULL,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr11,HT9B95B_DISPLAY_NULL,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr10,uints_val,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddrF,one_decimal_val,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddrE,Two_decimal_val,1);
    }
    else if (lengthdata<10000)
    {
        tens_val=lengthdata/1000;
        data_val=lengthdata%1000;
        uints_val=data_val/100;
        data_val=data_val%100;
        one_decimal_val=data_val/10;
        Two_decimal_val=data_val%10;
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr12,HT9B95B_DISPLAY_NULL,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr11,tens_val,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr10,uints_val,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddrF,one_decimal_val,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddrE,Two_decimal_val,1);
    }
    else if (lengthdata<100000)
    {
        hundreds_val=lengthdata/10000;
        data_val=lengthdata%10000;
        tens_val=data_val/1000;
        data_val=data_val%1000;
        uints_val=data_val/100;
        data_val=data_val%100;
        one_decimal_val=data_val/10;
        Two_decimal_val=data_val%10;
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr12,hundreds_val,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr11,tens_val,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr10,uints_val,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddrF,one_decimal_val,1);
        HT9B95B_Display_NUM_3_4_5_6_7(RAMaddrE,Two_decimal_val,1);
    }

}

/**
 * @brief 显示B接口的漏水位置
 * 
 * @param lengthdata 
 */
void HT9B95B_Display_B_Leaklength_Data(uint32_t lengthdata)
{
    uint32_t uints_val,tens_val ,hundreds_val ,Two_decimal_val ,one_decimal_val;
    uint32_t data_val;


    HT9B95B_clear_down_area();

    HT9B95B_Display_B_Leak(1);
    HT9B95B_Display_B_Warning(1);
    HT9B95B_Display_Ext2(1);
    HT9B95B_Display_B_Unit(1);
    HT9B95B_Display_B_Point(1);

    if (lengthdata<1000)
    {
        uints_val=lengthdata/100;
        data_val=lengthdata%100;
        one_decimal_val=data_val/10;
        Two_decimal_val=data_val%10;
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr4,HT9B95B_DISPLAY_NULL,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr3,HT9B95B_DISPLAY_NULL,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr2,uints_val,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr1,one_decimal_val,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr0,Two_decimal_val,1);
    }
    else if (lengthdata<10000)
    {
        tens_val=lengthdata/1000;
        data_val=lengthdata%1000;
        uints_val=data_val/100;
        data_val=data_val%100;
        one_decimal_val=data_val/10;
        Two_decimal_val=data_val%10;
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr4,HT9B95B_DISPLAY_NULL,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr3,tens_val,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr2,uints_val,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr1,one_decimal_val,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr0,Two_decimal_val,1);
    }
    else if (lengthdata<100000)
    {
        hundreds_val=lengthdata/10000;
        data_val=lengthdata%10000;
        tens_val=data_val/1000;
        data_val=data_val%1000;
        uints_val=data_val/100;
        data_val=data_val%100;
        one_decimal_val=data_val/10;
        Two_decimal_val=data_val%10;
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr4,hundreds_val,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr3,tens_val,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr2,uints_val,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr1,one_decimal_val,1);
        HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr0,Two_decimal_val,1);
    }

}

/**
 * @brief 屏幕显示A接口断线的000.00
 * 
 */
void HT9B95B_Display_A_Breaklength_Data(void)
{
    HT9B95B_clear_up_area();

    HT9B95B_Display_A_Break(1);
    HT9B95B_Display_A_Warning(1);
    HT9B95B_Display_Ext1(1);
    HT9B95B_Display_A_Unit(1);
    HT9B95B_Display_A_Point(1);

    HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr12,0,1);
    HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr11,0,1);
    HT9B95B_Display_NUM_3_4_5_6_7(RAMaddr10,0,1);
    HT9B95B_Display_NUM_3_4_5_6_7(RAMaddrF,0,1);
    HT9B95B_Display_NUM_3_4_5_6_7(RAMaddrE,0,1);

}

/**
 * @brief 屏幕显示B接口断线的000.00
 * 
 */
void HT9B95B_Display_B_Breaklength_Data(void)
{
    HT9B95B_clear_down_area();

    HT9B95B_Display_B_Break(1);
    HT9B95B_Display_B_Warning(1);
    HT9B95B_Display_Ext2(1);
    HT9B95B_Display_B_Unit(1);
    HT9B95B_Display_B_Point(1);

    HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr4,0,1);
    HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr3,0,1);
    HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr2,0,1);
    HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr1,0,1);
    HT9B95B_Display_NUM_8_9_10_11_12(RAMaddr0,0,1);

}


/**
 * @brief HT9B95B的初始化函数
 * 1、软件复位；
 * 2、1/4duty，1/3bias
 * 3、LCD驱动电压为3.3V
 * 4、LCD驱动电流为默认x1，刷新频率为80Hz
 * 5、LCD像素正常显示
 * 6、LCD显示模式B型反转，显示开启
 * 设置完成后清空显示界面
 */
void HT9B95B_init(bool lcd_reset)
{

    esp_err_t err;

    for (size_t i = 0; i < 2; i++)
    {
        vTaskDelay(15/portTICK_PERIOD_MS);
        if(lcd_reset)
        {
            err = HT9B95B_Multiple_readCMD(my_i2c_mum_0,HT9B95B_Write_Addr,Driver_reset_CMD);//Software Reset Setting,LCD Output Waveform Setting
            if(err<0)
            {
                ESP_LOGI("Waveform", "ERR");
                continue;
                
            }
            ESP_LOGI("Waveform", "ok");
            vTaskDelay(150/portTICK_PERIOD_MS);              //Delay About 150ms
        }
        
        err = HT9B95B_Multiple_readCMD(my_i2c_mum_0,HT9B95B_Write_Addr,Duty_Bias_CMD); //1/4Duty and 1/3 bias Setting
            if(err<0)
            {
                ESP_LOGI("Duty", "ERR");
                continue;
                
            }
            ESP_LOGI("Duty", "ok");
            vTaskDelay(3/portTICK_PERIOD_MS);    

        err = HT9B95B_Multiple_readCMD(my_i2c_mum_0,HT9B95B_Write_Addr,LCD_Voltage_CMD);//LCD driving voltage adjustment
            if(err<0)
            {
                ESP_LOGI("voltage", "ERR");
                continue;
                
            }
            ESP_LOGI("voltage", "ok");
            vTaskDelay(3/portTICK_PERIOD_MS);    
        
        err = HT9B95B_Multiple_readCMD(my_i2c_mum_0,HT9B95B_Write_Addr,Current_Frame_CMD);//Lcd Current Mode Setting,LCD Frame Frequency Setting
            if(err<0)
            {
                ESP_LOGI("Current", "ERR");
                continue;
                
            }
            ESP_LOGI("Current", "ok");
            vTaskDelay(3/portTICK_PERIOD_MS);   

        err = HT9B95B_Multiple_readCMD(my_i2c_mum_0,HT9B95B_Write_Addr,Pixels_Set_CMD);//nomal display
            if(err<0)
            {
                ESP_LOGI("nomal display Set", "ERR");
                continue;
                
            }
            ESP_LOGI("nomal display Set", "ok");
            vTaskDelay(3/portTICK_PERIOD_MS);     
         
        err = HT9B95B_Multiple_readCMD(my_i2c_mum_0,HT9B95B_Write_Addr,Driver_Display_CMD); //B型反转，显示开启
            if(err<0)
            {
                ESP_LOGI("B型反转 显示开启", "ERR");
                continue;
                
            }
            else
            {
                ESP_LOGI("B型反转 显示开启", "ok");
                break;
                 
            }     
              
            vTaskDelay(3/portTICK_PERIOD_MS);
            
    }

    vTaskDelay(3/portTICK_PERIOD_MS);

    err=HT9B95B_Clear();
    if (err<0)
    {
        // HT9B95B_Clear();
        ESP_LOGI(TAG,"%d,HT9B95B clear fail",__LINE__);
    }
    else
    {
        ESP_LOGI(TAG,"%d,HT9B95B OK",__LINE__);

    }
    vTaskDelay(3/portTICK_PERIOD_MS);
    // HT9B95B_Display_CrossLine(true);
    // HT9B95B_Display_A_Point(true);
    // vTaskDelay(5000/portTICK_PERIOD_MS);
    
        
}

