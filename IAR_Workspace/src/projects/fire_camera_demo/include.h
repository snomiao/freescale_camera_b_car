#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include  "common.h"

/*
 * Include �û��Զ����ͷ�ļ�
 */

#include  "gpio.h"          //IO�ڲ���
#include  "uart.h"          //����
#include  "lptmr.h"         //�͹��Ķ�ʱ��(��ʱ)
#include  "PIT.h"
#include  "LED.h"
#include  "dma.h"           //����ͷdma��
#include  "FTM.h"

//#include  "spi.h"         //��������ͨѶ��
#include  "init.h"          //��ʼ��
#include  "MENU.h"          //menu�˵��Ŀ�
#include  "BL144002.h"      //lcd��
#include  "LCD.h"           //lcd��
#include  "OV7725.h"        //����ͷ��
#include  "OV7725_REG.h"    // ??
#include  "SCCB.h"          //i2c������ͷͨѶ��

#include  "ASCII.h"// ??

#include  "CAMERA_FUNC.h"   //����ͷ���⹦�ܺ�����
#include  "IMG_FUNC.h"      //ͼ�����
#include  "STEERING_GEAR.h" //�����
#include  "MOTOR.h"         //�����
#include  "COMMUNICATION.h" //ͨ�ſ�

extern volatile u8 img_flag;      //ͼ��״̬


#endif  //__INCLUDE_H__
