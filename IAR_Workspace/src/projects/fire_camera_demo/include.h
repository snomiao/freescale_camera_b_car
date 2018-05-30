#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include  "common.h"

/*
 * Include 用户自定义的头文件
 */

#include  "gpio.h"          //IO口操作
#include  "uart.h"          //串口
#include  "lptmr.h"         //低功耗定时器(延时)
#include  "PIT.h"
#include  "LED.h"
#include  "dma.h"           //摄像头dma库
#include  "FTM.h"

//#include  "spi.h"         //理论上是通讯的
#include  "init.h"          //初始化
#include  "MENU.h"          //menu菜单的库
#include  "BL144002.h"      //lcd的
#include  "LCD.h"           //lcd的
#include  "OV7725.h"        //摄像头的
#include  "OV7725_REG.h"    // ??
#include  "SCCB.h"          //i2c，摄像头通讯的

#include  "ASCII.h"// ??

#include  "CAMERA_FUNC.h"   //摄像头特殊功能函数库
#include  "IMG_FUNC.h"      //图像处理库
#include  "STEERING_GEAR.h" //舵机库
#include  "MOTOR.h"         //电机库
#include  "COMMUNICATION.h" //通信库

extern volatile u8 img_flag;      //图像状态


#endif  //__INCLUDE_H__
