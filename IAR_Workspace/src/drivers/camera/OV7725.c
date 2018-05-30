#include "common.h"
#include "gpio.h"
#include "dma.h"
#include "delay.h"
#include "SCCB.h"
#include "OV7725.h"
#include "OV7725_REG.h"


#define OV7725_EAGLE_Delay_ms(time)  DELAY_MS(time)


uint8   *ov7725_eagle_img_buff = NULL;  				   //定义一个存放图像的指针

volatile IMG_STATUS_e      ov7725_eagle_img_flag = IMG_FINISH;   //图像状态

//内部函数声明
static uint8 ov7725_eagle_reg_init(void);
static void ov7725_eagle_port_init();


/******************************************************
 * 函数名：exti_init
 * 描述  ：初始化VSYN场中断端口（内部调用）
 * 输入  ：端口组，端口号，触发中断条件
 * 输出  ：void
 * 举例  ：
 * 注意  ：内部函数
*********************************************************/  
void  exti_init(PORTx portx, u8 n, exti_cfg cfg)
{
    SIM_SCGC5 |= (SIM_SCGC5_PORTC_MASK << portx);    //开启PORTC端口
    PORT_PCR_REG(PORTX[portx], n) = PORT_PCR_MUX(1) | PORT_PCR_IRQC(cfg & 0x7f ) | PORT_PCR_PE_MASK | ((cfg & 0x80 ) >> 7); // 复用GPIO , 确定触发模式 ,开启上拉或下拉电阻
    GPIO_PDDR_REG(GPIOx[portx]) &= ~(1 << n);       //输入模式
    //enable_irq(portx + 87);                         //使能PORT中断，PORTC的ISR中断号为89
}


/******************************************************
 * 函数名：ov7725_eagle_init
 * 描述  ：调用ov7725_eagle_port_init初始化dma，并把数组地址赋给指针（外部调用）
 * 输入  ：图像大小的数组
 * 输出  ：0
 * 举例  ：
 * 注意  ：
*********************************************************/  
uint8 try_ov7725_eagle_init(uint8 *imgaddr)
{
    ov7725_eagle_img_buff = imgaddr;			//将传进来的数组的指针赋给指针
    if(ov7725_eagle_reg_init() == 0)    		//初始化dma
      return 1;
    ov7725_eagle_port_init();                           //调用初始化dma函数
    return 0;
}

uint8 ov7725_eagle_init(uint8 *imgaddr)
{
    ov7725_eagle_img_buff = imgaddr;			//将传进来的数组的指针赋给指针
    while(ov7725_eagle_reg_init() == 0);		//初始化dma直到成功为止
    ov7725_eagle_port_init();                           //调用初始化dma函数
    return 0;
}


/******************************************************
 * 函数名：ov7725_eagle_port_init
 * 描述  ：初始化dma（内部调用）
 * 输入  ：void
 * 输出  ：void
 * 举例  ：
 * 注意  ：定义了VSTN场中断端口，exti_init为内部函数
*********************************************************/ 
void ov7725_eagle_port_init()
{
    //DMA通道0初始化，PTA27触发源(默认上升沿)，源地址为PTB_B0_IN，目的地址为：IMG_BUFF，每次传输1Byte
    DMA_PORTx2BUFF_Init(CAMERA_DMA_CH, (void *)&PTD_BYTE0_IN, (void *)ov7725_eagle_img_buff, PTB8, DMA_BYTE1, CAMERA_DMA_NUM, DMA_falling_keepon);

    DMA_DIS(DMA_CH0);
    disable_irq(89);                                //关闭PTA的中断
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);                   //清除通道传输中断标志位
    DMA_IRQ_EN(CAMERA_DMA_CH);

    //port_init(PTA27, ALT1 | DMA_FALLING | PULLDOWN );         //PCLK
    
    exti_init(PORTC,19,falling_down);	//场中断VSYN，内部下拉，上升沿触发中断 rising_down

}


/*!
 *  @brief      鹰眼ov7725场中断服务函数
 *  @since      v5.0
 */
/******************************************************
 * 函数名：ov7725_eagle_vsync
 * 描述  ：场中断服务函数（外部调用）
 * 输入  ：void
 * 输出  ：void
 * 举例  ：
 * 注意  ：采集图像
*********************************************************/ 
void ov7725_eagle_vsync(void)
{

    //场中断需要判断是场结束还是场开始
    if(ov7725_eagle_img_flag == IMG_START)                   //需要开始采集图像
    {
        ov7725_eagle_img_flag = IMG_GATHER;                  //标记图像采集中
        disable_irq(89);

#if 1

        //PORTB_ISFR  = ~0;   //PORTA_ISFR = 1 <<  PT27;            //清空PCLK标志位

        DMA_EN(CAMERA_DMA_CH);                  //使能通道CHn 硬件请求
        //PORTB_ISFR  = ~0;   //PORTA_ISFR = 1 <<  PT27;            //清空PCLK标志位
        DMA_DADDR(CAMERA_DMA_CH) = (uint32)ov7725_eagle_img_buff;    //恢复地址

#else
        PORTB_ISFR  = ~0;   //PORTA_ISFR = 1 <<  PT27;            //清空PCLK标志位
        dma_repeat(CAMERA_DMA_CH, (void *)&PTB_B0_IN, (void *)ov7725_eagle_img_buff,CAMERA_DMA_NUM);
#endif
    }
    else                                        //图像采集错误
    {
        disable_irq(89);                        //关闭PTA的中断
        ov7725_eagle_img_flag = IMG_FAIL;                    //标记图像采集失败
    }
}

/*!
 *  @brief      鹰眼ov7725 DMA中断服务函数
 *  @since      v5.0
 */
/******************************************************
 * 函数名：ov7725_eagle_dma
 * 描述  ：DMA中断服务函数
 * 输入  ：void
 * 输出  ：void
 * 举例  ：
 * 注意  ：
*********************************************************/ 
void ov7725_eagle_dma()
{
    ov7725_eagle_img_flag = IMG_FINISH ;
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //清除通道传输中断标志位
}



/******************************************************
 * 函数名：ov7725_eagle_get_img
 * 描述  ：采集图像，每次获得图像都需要调用
		（采集到的数据存储在 初始化时配置的地址上）（外部调用）
 * 输入  ：void 
 * 输出  ：void
 * 举例  ：
 * 注意  ：
*********************************************************/ 
void ov7725_eagle_get_img()
{
    ov7725_eagle_img_flag = IMG_START;                   //开始采集图像
    PORTC_ISFR = ~0;                        //写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
    enable_irq(89);                         //允许PTA的中断
    while(ov7725_eagle_img_flag != IMG_FINISH)           //等待图像采集完毕
    {
        if(ov7725_eagle_img_flag == IMG_FAIL)            //假如图像采集错误，则重新开始采集
        {
            ov7725_eagle_img_flag = IMG_START;           //开始采集图像
            PORTC_ISFR = ~0;                //写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
            enable_irq(89);                 //允许PTA的中断
        }
    }
}


/*OV7725初始化配置表*/
reg_s ov7725_eagle_reg[] =
{
    //寄存器，寄存器值次
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

uint8 ov7725_eagle_cfgnum = ARR_SIZE( ov7725_eagle_reg ) ; /*结构体数组成员数目*/


/******************************************************
 * 函数名：ov7725_eagle_reg_init
 * 描述  ：鹰眼ov7725寄存器 初始化
 * 输入  ：void
 * 输出  ：初始化结果（0表示失败，1表示成功）
 * 举例  ：
 * 注意  ：
*********************************************************/ 
uint8 ov7725_eagle_reg_init(void)
{
    uint16 i = 0;
    uint8 Sensor_IDCode = 0;
    
    OV7725_EAGLE_Delay_ms(50);
    
    SCCB_GPIO_init();

    OV7725_EAGLE_Delay_ms(50);
    
    if( 0 == SCCB_WriteByte( OV7725_COM7, 0x80 ) ) /*复位sensor */
    {
        DEBUG_OUT("\n警告:SCCB写数据错误");
        return 0 ;
    }

    OV7725_EAGLE_Delay_ms(50);

    if( 0 == SCCB_ReadByte( &Sensor_IDCode, 1, OV7725_VER ) )    /* 读取sensor ID号*/
    {
        DEBUG_OUT("\n警告:读取ID失败");
        return 0;
    }
    DEBUG_OUT("\nGet ID success，SENSOR ID is 0x%x", Sensor_IDCode);
    DEBUG_OUT("\nConfig Register Number is %d ", ov7725_eagle_cfgnum);
    if(Sensor_IDCode == OV7725_ID)
    {
        for( i = 0 ; i < ov7725_eagle_cfgnum ; i++ )
        {
            if( 0 == SCCB_WriteByte(ov7725_eagle_reg[i].addr, ov7725_eagle_reg[i].val) )
            {
                DEBUG_OUT("\n警告:写寄存器0x%x失败", ov7725_eagle_reg[i].addr);
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


