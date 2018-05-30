#ifndef	_BL14002_H_
#define _BL14002_H_

#define	LCD_H	129		//��
#define LCD_W	129		//��

#define	wr		PTC8_OUT
#define	rd		PTC9_OUT
#define	cs		PTC10_OUT
#define	rs		PTC11_OUT
#define	reset	        PTC12_OUT

//lcd�ϰ����궨��
#define middleKey          PTE4_IN
#define downKey            PTE8_IN
#define upKey              PTE9_IN
#define rightKey           PTE10_IN
#define leftKey            PTE11_IN

#define P0			PTC_BYTE0_OUT
#define	PIN			PTC_BYTE0_IN
#define PDDR_OUT()	(DDRC_BYTE0 = 0xff)
#define PDDR_IN()	(DDRC_BYTE0 = 0)

#define	BL14002_DELAY()		do{asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");}while(0)

typedef	struct
{
	u16 x;
	u16 y;
}Site_type;

typedef	struct
{
	u16 W;		//��
	u16 H;		//��
}Size_type;

#define LCD_RAMWR()		LCD_WR_CMD(0x2C)				        //���� Memory Write 
#define LCD_RAMRD()     do{LCD_WR_CMD(0x2E);rd_data();}while(0) //���� Memory Read 

//void 	wr_cmd(u8 command_data_8);
//void 	LCD_WR_Data(u8 low_8_data);
u8  	rd_data(void);
void 	lcd_initialize();
void 	LCD_PTLON(Site_type site,Size_type size);		//����

/*******************����Ϊд���д����*********************/
/*****���ں����϶̣�ִ�д���Ƶ��������д�ɺ궨�壬�ӿ��ٶ�*****/
#define LCD_WR_CMD(command_data_8)		do\
										{\
										rd=1;\
										BL14002_DELAY();\
										rs=0;\
										BL14002_DELAY();\
										cs=0;\
										BL14002_DELAY();\
										P0=(u8)(command_data_8);\
										wr=0;\
										BL14002_DELAY();\
										wr=1;\
										BL14002_DELAY();\
										cs=1;\
									}while(0)	//wr=0;wr=1;����һ��������
									
#define LCD_WR_Data(low_8_data)		do\
								{\
									rd=1;\
									rs=1;\
									cs=0;\
									BL14002_DELAY();\
									P0=(u8)(low_8_data);\
									wr=0;\
									wr=1;\
									cs=1;\
								}while(0) 	//wr=0;������д�����ݵ�RAM 

#define LCD_WR_16Data(DATA)        LCD_WR_Data((u8)((DATA)>>8) );LCD_WR_Data( (u8)(DATA) ) //д16λ����

#define LCD_WR_16Data_BIG(DATA)    LCD_WR_Data((u8)(DATA) );LCD_WR_Data( (u8)((DATA)>>8) ) //д16λ���ݣ����

#endif	//_BL14002_H_