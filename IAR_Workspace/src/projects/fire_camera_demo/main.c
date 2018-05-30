/**********************************************************************************
 * 文件名       ：main.c
 * 描述         ：摄像头小车主函数
 * 作者         ：沈泽皓

 * 接口说明		
1、摄像头
D0-D7				PTD0-PTD7
DMA,PCLK			PTB8
HRER				PTC16			//暂无代码支持
VSYN，场中断			PTC19
SDA					PTC14
SCL					PTC13

2、LCD
D0-D7				PTC0-PTC7
wr					PTC8
rd					PTC9
cs					PTC10
rs					PTC11
reset				PTC12
左					PTE10
右					PTE11
上					PTE8
下					PTE9
中					PTE4

**********************************************************************************/

/**********************************************************************************
*主要外部函数调用介绍
1、	void	menuStart		(void)			调用菜单
	tip:自带while循环

2、	void	printCamera		(void)			打印二进制图像
	tip:自带while循环

3、	void	printCamera2	(void)			打印处理后图像并控制舵机电机旋转
	tip:自带while循环，内部调用4,5,6函数

4、	void	imageProcessing	(void)			处理图像（在CAMERA_FUNC函数中调用）
	tip:

5、	int		sGStart			(int* Point)	舵机转动（在CAMERA_FUNC函数中调用）
	tip:

6、	void	motorStart		(int turn)		电机转动（在CAMERA_FUNC函数中调用）
	tip:
**********************************************************************************/

#include "common.h"
#include "include.h"



//u16 CNST_=0x3C;					//SCCB寄存器地址,调整灰度
u16 CNST_ = 100;					//SCCB寄存器地址,调整灰度w
 void main(void)
{	
	myInit();  						//初始化
        DisableInterrupts;
        SCCB_WriteByte( 0x9C, CNST_);			//i2c协议，调整摄像头灰度
        EnableInterrupts;
        
	menuStart();
	
//	printCamera();
	//printCamera2();

}
