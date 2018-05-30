/**********************************************************************************
 * �ļ���       ��main.c
 * ����         ������ͷС��������
 * ����         �������

 * �ӿ�˵��		
1������ͷ
D0-D7				PTD0-PTD7
DMA,PCLK			PTB8
HRER				PTC16			//���޴���֧��
VSYN�����ж�			PTC19
SDA					PTC14
SCL					PTC13

2��LCD
D0-D7				PTC0-PTC7
wr					PTC8
rd					PTC9
cs					PTC10
rs					PTC11
reset				PTC12
��					PTE10
��					PTE11
��					PTE8
��					PTE9
��					PTE4

**********************************************************************************/

/**********************************************************************************
*��Ҫ�ⲿ�������ý���
1��	void	menuStart		(void)			���ò˵�
	tip:�Դ�whileѭ��

2��	void	printCamera		(void)			��ӡ������ͼ��
	tip:�Դ�whileѭ��

3��	void	printCamera2	(void)			��ӡ�����ͼ�񲢿��ƶ�������ת
	tip:�Դ�whileѭ�����ڲ�����4,5,6����

4��	void	imageProcessing	(void)			����ͼ����CAMERA_FUNC�����е��ã�
	tip:

5��	int		sGStart			(int* Point)	���ת������CAMERA_FUNC�����е��ã�
	tip:

6��	void	motorStart		(int turn)		���ת������CAMERA_FUNC�����е��ã�
	tip:
**********************************************************************************/

#include "common.h"
#include "include.h"



//u16 CNST_=0x3C;					//SCCB�Ĵ�����ַ,�����Ҷ�
u16 CNST_ = 100;					//SCCB�Ĵ�����ַ,�����Ҷ�w
 void main(void)
{	
	myInit();  						//��ʼ��
        DisableInterrupts;
        SCCB_WriteByte( 0x9C, CNST_);			//i2cЭ�飬��������ͷ�Ҷ�
        EnableInterrupts;
        
	menuStart();
	
//	printCamera();
	//printCamera2();

}
