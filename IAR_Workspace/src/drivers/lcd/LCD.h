#ifndef	_LCD_H_
#define	_LCD_H_

#include "BL144002.h"

/******������ɫ*****/
#define RED   0XF800
#define GREEN 0X07E0
#define BLUE  0X001F  
#define BRED  0XF81F
#define GRED  0XFFE0
#define GBLUE 0X07FF
#define BLACK 0X0000
#define WHITE 0XFFFF

#define imgSiteX 10   //����ͷͼƬ��ʾλ�ã����Ͻ�x����
#define imgSiteY 10   //����ͷͼƬ��ʾλ�ã����Ͻ�y����



#define RGB565_R(rgb565)				((u8)(((rgb565)>>11) &0x1F))	                        //��ȡRGB��3Ԫ�أ�rgb565������16λ							
#define RGB565_G(rgb565)				((u8)(((rgb565)>> 5) &0x3F))
#define RGB565_B(rgb565)				((u8)( (rgb565)      &0x1F))
#define GRAY_2_RGB565(gray)				((u16)((((u8)(gray)>>3)<<11)&(((u8)(gray)>>2)<<5)&((u8)(gray)>>3)))
#define RGB565_2_GRAY(rgb565)   		        ((u8)(((RGB565_R(rgb565)*235+RGB565_G(rgb565)*613+RGB565_B(rgb565)*625)+1)>>8))	//31*235+63*613+31*625+1  = 255*256
#define RGB565_H(rgb565)        		        ((u8)(((u16)(rgb565))>>8))
#define RGB565_L(rgb565)        		        (u8)(rgb565))

#define	BINARY_BGCOLOR	WHITE		                //�����ֵ��ͼ�񱳾���ɫ
#define	BINARY_COLOR	BLACK		                //�����ֵ��ͼ��ǰ����ɫ
#define MAX_NUM_BIT     5                               //���ֵ����λ������������������Ļ�������֣�

#define Range(x,max,min)				((u8)((x)<(min) ? (min) : ( (x)>(max) ? (max):(x) )))	//ȷ��x�ķ�ΧΪ min~max		



extern  void LCD_Init(u16 colour);
extern  void LCD_Img_Binary     (u8 *img);                                                 //��ʾ��ֵ��ͼ��
extern  void LCD_Img_Binary_Z   (Site_type site,Size_type size,u16 *img,Size_type imgsize); //��ʾ��ֵ��ͼ��(������)
void LCD_Img(u8 img[80][60]);
void LCD_Clear(u16 colour);

/***************  LCD�滭  ***************/
u8   LCD_Char       (Site_type site,const u8 ascii , u16 Color ,u16 bkColor);			//��ʾ�ַ�
void LCD_Point      (Site_type site                             ,u16 rgb565);   	    //����
void LCD_A_Point    (u8 sit                              ,u8 sie,u16 rgb565);
void LCD_Rectangle  (Site_type site,Size_type size              ,u16 rgb565);   	    //������
void LCD_Str_8x16   (Site_type site,u8 *Str, u16 Color, u16 bkColor);  	            	//��ʾ�ַ���
void LCD_Str_8x16_X (u8 sit,u8 sie,u8 *Str, u16 Color, u16 bkColor);
void LCD_Str_R      (Site_type site,const u8 *str  , u16 Color ,u16 bkColor);			//��ʾ�ַ������Ҷ��룩
void LCD_Num_8x16   (Site_type site,u32 num        , u16 Color ,u16 bkColor);			//��ʾ����
void LCD_Num_8x16_C (Site_type site,u32 num        , u16 Color ,u16 bkColor);			//��ʾ����(���� MAX_NUM_BIT ������������Ļ�������֣������������λ���������õ����λ��ֻ��ʾ��λ������Ҫ���ú�Ŷ)
void LCD_Num_8x16_X (u8 sit         ,u8 sie,u32 num, u16 Color, u16 bkColor);
void LCD_Img_RGB565 (Site_type site                ,Size_type size,u16 *img);		    //ͼ����ʾ
void LCD_Img_RGB565_Z	(Site_type site,Size_type size,u16 *img,Size_type imgsize);		//ͼ����ʾ����zoom���Ź���


#endif