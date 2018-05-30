#ifndef __SCCB_H
#define __SCCB_H


#define SCL_H()         PTC14_OUT = 1
#define SCL_L()         PTC14_OUT = 0
#define	SCL_DDR_OUT() 	DDRC14 = 1
#define	SCL_DDR_IN() 	DDRC14 = 0

#define SDA_H()         PTC13_OUT = 1
#define SDA_L()         PTC13_OUT = 0
#define SDA_IN()      	PTC13_IN
#define SDA_DDR_OUT()	DDRC13 = 1
#define SDA_DDR_IN()	DDRC13 = 0

#define ADDR_OV7725   0x42

#define SCCB_DELAY()	SCCB_delay(200)	


void SCCB_GPIO_init(void);
int SCCB_WriteByte( u16 WriteAddress , u8 SendByte);
int SCCB_ReadByte(u8* pBuffer,   u16 length,   u8 ReadAddress);

static void SCCB_delay(u16 i);
#endif 
