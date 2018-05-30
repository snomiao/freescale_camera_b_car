#include "common.h"
#include "gpio.h"
#include "dma.h"
#include "delay.h"
#include "SCCB.h"
#include "OV7725.h"
#include "OV7725_REG.h"


#define OV7725_EAGLE_Delay_ms(time)  DELAY_MS(time)


uint8   *ov7725_eagle_img_buff = NULL;  				   //����һ�����ͼ���ָ��

volatile IMG_STATUS_e      ov7725_eagle_img_flag = IMG_FINISH;   //ͼ��״̬

//�ڲ���������
static uint8 ov7725_eagle_reg_init(void);
static void ov7725_eagle_port_init();


/******************************************************
 * ��������exti_init
 * ����  ����ʼ��VSYN���ж϶˿ڣ��ڲ����ã�
 * ����  ���˿��飬�˿ںţ������ж�����
 * ���  ��void
 * ����  ��
 * ע��  ���ڲ�����
*********************************************************/  
void  exti_init(PORTx portx, u8 n, exti_cfg cfg)
{
    SIM_SCGC5 |= (SIM_SCGC5_PORTC_MASK << portx);    //����PORTC�˿�
    PORT_PCR_REG(PORTX[portx], n) = PORT_PCR_MUX(1) | PORT_PCR_IRQC(cfg & 0x7f ) | PORT_PCR_PE_MASK | ((cfg & 0x80 ) >> 7); // ����GPIO , ȷ������ģʽ ,������������������
    GPIO_PDDR_REG(GPIOx[portx]) &= ~(1 << n);       //����ģʽ
    //enable_irq(portx + 87);                         //ʹ��PORT�жϣ�PORTC��ISR�жϺ�Ϊ89
}


/******************************************************
 * ��������ov7725_eagle_init
 * ����  ������ov7725_eagle_port_init��ʼ��dma�����������ַ����ָ�루�ⲿ���ã�
 * ����  ��ͼ���С������
 * ���  ��0
 * ����  ��
 * ע��  ��
*********************************************************/  
uint8 try_ov7725_eagle_init(uint8 *imgaddr)
{
    ov7725_eagle_img_buff = imgaddr;			//���������������ָ�븳��ָ��
    if(ov7725_eagle_reg_init() == 0)    		//��ʼ��dma
      return 1;
    ov7725_eagle_port_init();                           //���ó�ʼ��dma����
    return 0;
}

uint8 ov7725_eagle_init(uint8 *imgaddr)
{
    ov7725_eagle_img_buff = imgaddr;			//���������������ָ�븳��ָ��
    while(ov7725_eagle_reg_init() == 0);		//��ʼ��dmaֱ���ɹ�Ϊֹ
    ov7725_eagle_port_init();                           //���ó�ʼ��dma����
    return 0;
}


/******************************************************
 * ��������ov7725_eagle_port_init
 * ����  ����ʼ��dma���ڲ����ã�
 * ����  ��void
 * ���  ��void
 * ����  ��
 * ע��  ��������VSTN���ж϶˿ڣ�exti_initΪ�ڲ�����
*********************************************************/ 
void ov7725_eagle_port_init()
{
    //DMAͨ��0��ʼ����PTA27����Դ(Ĭ��������)��Դ��ַΪPTB_B0_IN��Ŀ�ĵ�ַΪ��IMG_BUFF��ÿ�δ���1Byte
    DMA_PORTx2BUFF_Init(CAMERA_DMA_CH, (void *)&PTD_BYTE0_IN, (void *)ov7725_eagle_img_buff, PTB8, DMA_BYTE1, CAMERA_DMA_NUM, DMA_falling_keepon);

    DMA_DIS(DMA_CH0);
    disable_irq(89);                                //�ر�PTA���ж�
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);                   //���ͨ�������жϱ�־λ
    DMA_IRQ_EN(CAMERA_DMA_CH);

    //port_init(PTA27, ALT1 | DMA_FALLING | PULLDOWN );         //PCLK
    
    exti_init(PORTC,19,falling_down);	//���ж�VSYN���ڲ������������ش����ж� rising_down

}


/*!
 *  @brief      ӥ��ov7725���жϷ�����
 *  @since      v5.0
 */
/******************************************************
 * ��������ov7725_eagle_vsync
 * ����  �����жϷ��������ⲿ���ã�
 * ����  ��void
 * ���  ��void
 * ����  ��
 * ע��  ���ɼ�ͼ��
*********************************************************/ 
void ov7725_eagle_vsync(void)
{

    //���ж���Ҫ�ж��ǳ��������ǳ���ʼ
    if(ov7725_eagle_img_flag == IMG_START)                   //��Ҫ��ʼ�ɼ�ͼ��
    {
        ov7725_eagle_img_flag = IMG_GATHER;                  //���ͼ��ɼ���
        disable_irq(89);

#if 1

        //PORTB_ISFR  = ~0;   //PORTA_ISFR = 1 <<  PT27;            //���PCLK��־λ

        DMA_EN(CAMERA_DMA_CH);                  //ʹ��ͨ��CHn Ӳ������
        //PORTB_ISFR  = ~0;   //PORTA_ISFR = 1 <<  PT27;            //���PCLK��־λ
        DMA_DADDR(CAMERA_DMA_CH) = (uint32)ov7725_eagle_img_buff;    //�ָ���ַ

#else
        PORTB_ISFR  = ~0;   //PORTA_ISFR = 1 <<  PT27;            //���PCLK��־λ
        dma_repeat(CAMERA_DMA_CH, (void *)&PTB_B0_IN, (void *)ov7725_eagle_img_buff,CAMERA_DMA_NUM);
#endif
    }
    else                                        //ͼ��ɼ�����
    {
        disable_irq(89);                        //�ر�PTA���ж�
        ov7725_eagle_img_flag = IMG_FAIL;                    //���ͼ��ɼ�ʧ��
    }
}

/*!
 *  @brief      ӥ��ov7725 DMA�жϷ�����
 *  @since      v5.0
 */
/******************************************************
 * ��������ov7725_eagle_dma
 * ����  ��DMA�жϷ�����
 * ����  ��void
 * ���  ��void
 * ����  ��
 * ע��  ��
*********************************************************/ 
void ov7725_eagle_dma()
{
    ov7725_eagle_img_flag = IMG_FINISH ;
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //���ͨ�������жϱ�־λ
}



/******************************************************
 * ��������ov7725_eagle_get_img
 * ����  ���ɼ�ͼ��ÿ�λ��ͼ����Ҫ����
		���ɼ��������ݴ洢�� ��ʼ��ʱ���õĵ�ַ�ϣ����ⲿ���ã�
 * ����  ��void 
 * ���  ��void
 * ����  ��
 * ע��  ��
*********************************************************/ 
void ov7725_eagle_get_img()
{
    ov7725_eagle_img_flag = IMG_START;                   //��ʼ�ɼ�ͼ��
    PORTC_ISFR = ~0;                        //д1���жϱ�־λ(����ģ���Ȼ�ص���һ���жϾ����ϴ����ж�)
    enable_irq(89);                         //����PTA���ж�
    while(ov7725_eagle_img_flag != IMG_FINISH)           //�ȴ�ͼ��ɼ����
    {
        if(ov7725_eagle_img_flag == IMG_FAIL)            //����ͼ��ɼ����������¿�ʼ�ɼ�
        {
            ov7725_eagle_img_flag = IMG_START;           //��ʼ�ɼ�ͼ��
            PORTC_ISFR = ~0;                //д1���жϱ�־λ(����ģ���Ȼ�ص���һ���жϾ����ϴ����ж�)
            enable_irq(89);                 //����PTA���ж�
        }
    }
}


/*OV7725��ʼ�����ñ�*/
reg_s ov7725_eagle_reg[] =
{
    //�Ĵ������Ĵ���ֵ��
    {OV7725_COM4         , 0xC1},
    {OV7725_CLKRC        , 0x00},
    {OV7725_COM2         , 0x03},
    {OV7725_COM3         , 0xD0},
    {OV7725_COM7         , 0x40},
    {OV7725_HSTART       , 0x3F},
    {OV7725_HSIZE        , 0x50},
    {OV7725_VSTRT        , 0x03},
    {OV7725_VSIZE        , 0x78},
    {OV7725_HREF         , 0x00},
    {OV7725_SCAL0        , 0x0A},
    {OV7725_AWB_Ctrl0    , 0xE0},
    {OV7725_DSPAuto      , 0xff},
    {OV7725_DSP_Ctrl2    , 0x0C},
    {OV7725_DSP_Ctrl3    , 0x00},
    {OV7725_DSP_Ctrl4    , 0x00},

#if (CAMERA_W == 80)
    {OV7725_HOutSize     , 0x14},
#elif (CAMERA_W == 160)
    {OV7725_HOutSize     , 0x28},
#elif (CAMERA_W == 240)
    {OV7725_HOutSize     , 0x3c},
#elif (CAMERA_W == 320)
    {OV7725_HOutSize     , 0x50},
#else

#endif

#if (CAMERA_H == 60 )
    {OV7725_VOutSize     , 0x1E},
#elif (CAMERA_H == 120 )
    {OV7725_VOutSize     , 0x3c},
#elif (CAMERA_H == 180 )
    {OV7725_VOutSize     , 0x5a},
#elif (CAMERA_H == 240 )
    {OV7725_VOutSize     , 0x78},
#else

#endif

    {OV7725_EXHCH        , 0x00},
    {OV7725_GAM1         , 0x0c},
    {OV7725_GAM2         , 0x16},
    {OV7725_GAM3         , 0x2a},
    {OV7725_GAM4         , 0x4e},
    {OV7725_GAM5         , 0x61},
    {OV7725_GAM6         , 0x6f},
    {OV7725_GAM7         , 0x7b},
    {OV7725_GAM8         , 0x86},
    {OV7725_GAM9         , 0x8e},
    {OV7725_GAM10        , 0x97},
    {OV7725_GAM11        , 0xa4},
    {OV7725_GAM12        , 0xaf},
    {OV7725_GAM13        , 0xc5},
    {OV7725_GAM14        , 0xd7},
    {OV7725_GAM15        , 0xe8},
    {OV7725_SLOP         , 0x20},
    {OV7725_LC_RADI      , 0x00},
    {OV7725_LC_COEF      , 0x13},
    {OV7725_LC_XC        , 0x08},
    {OV7725_LC_COEFB     , 0x14},
    {OV7725_LC_COEFR     , 0x17},
    {OV7725_LC_CTR       , 0x05},
    {OV7725_BDBase       , 0x99},
    {OV7725_BDMStep      , 0x03},
    {OV7725_SDE          , 0x04},
    {OV7725_BRIGHT       , 0x00},
    {OV7725_CNST         , 0xFF},
    {OV7725_SIGN         , 0x06},
    {OV7725_UVADJ0       , 0x11},
    {OV7725_UVADJ1       , 0x02},

};

uint8 ov7725_eagle_cfgnum = ARR_SIZE( ov7725_eagle_reg ) ; /*�ṹ�������Ա��Ŀ*/


/******************************************************
 * ��������ov7725_eagle_reg_init
 * ����  ��ӥ��ov7725�Ĵ��� ��ʼ��
 * ����  ��void
 * ���  ����ʼ�������0��ʾʧ�ܣ�1��ʾ�ɹ���
 * ����  ��
 * ע��  ��
*********************************************************/ 
uint8 ov7725_eagle_reg_init(void)
{
    uint16 i = 0;
    uint8 Sensor_IDCode = 0;
    
    OV7725_EAGLE_Delay_ms(50);
    
    SCCB_GPIO_init();

    OV7725_EAGLE_Delay_ms(50);
    
    if( 0 == SCCB_WriteByte( OV7725_COM7, 0x80 ) ) /*��λsensor */
    {
        DEBUG_OUT("\n����:SCCBд���ݴ���");
        return 0 ;
    }

    OV7725_EAGLE_Delay_ms(50);

    if( 0 == SCCB_ReadByte( &Sensor_IDCode, 1, OV7725_VER ) )    /* ��ȡsensor ID��*/
    {
        DEBUG_OUT("\n����:��ȡIDʧ��");
        return 0;
    }
    DEBUG_OUT("\nGet ID success��SENSOR ID is 0x%x", Sensor_IDCode);
    DEBUG_OUT("\nConfig Register Number is %d ", ov7725_eagle_cfgnum);
    if(Sensor_IDCode == OV7725_ID)
    {
        for( i = 0 ; i < ov7725_eagle_cfgnum ; i++ )
        {
            if( 0 == SCCB_WriteByte(ov7725_eagle_reg[i].addr, ov7725_eagle_reg[i].val) )
            {
                DEBUG_OUT("\n����:д�Ĵ���0x%xʧ��", ov7725_eagle_reg[i].addr);
                return 0;
            }
        }
    }
    else
    {
        return 0;
    }
    DEBUG_OUT("\nOV7725 Register Config Success!");
    return 1;
}


