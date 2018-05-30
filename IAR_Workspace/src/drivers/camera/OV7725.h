#ifndef __OV7725_H
#define __OV7725_H 

/*
 * 返回数组元素的个数
 */
#define ARR_SIZE( a ) ( sizeof( (a) ) / sizeof( ((a)[0]) ) )

#if ( USE_CAMERA == CAMERA_OV7725_EAGLE )
//配置摄像头的特性
#define CAMERA_USE_HREF     0               //是否使用 行中断 (0 为 不使用，1为使用)
#define CAMERA_COLOR        0               //摄像头输出颜色 ， 0 为 黑白二值化图像 ，1 为 灰度 图像 ，2 为 RGB565 图像
#define CAMERA_POWER        0               //摄像头 电源选择， 0 为 3.3V ,1 为 5V

//配置摄像头顶层接口
#define camera_init(imgaddr)    ov7725_eagle_init(imgaddr)
#define camera_get_img()        ov7725_eagle_get_img()

#define camera_vsync()          ov7725_eagle_vsync()
#define camera_href()           //ov7725_eagle_href()
#define camera_dma()            ov7725_eagle_dma()

//配置 摄像头 参数
#define CAMERA_DMA_CH       OV7725_EAGLE_DMA_CH         //定义摄像头的DMA采集通道
#define CAMERA_W            OV7725_EAGLE_W              //定义摄像头图像宽度
#define CAMERA_H            OV7725_EAGLE_H              //定义摄像头图像高度

#define CAMERA_SIZE         OV7725_EAGLE_SIZE           //图像占用空间大小
#define CAMERA_DMA_NUM      OV7725_EAGLE_DMA_NUM        //DMA采集次数

#endif  //#if ( USE_CAMERA == CAMERA_OV7725_EAGLE )


//配置摄像头 属性
#define OV7725_EAGLE_DMA_CH       DMA_CH0                               //定义摄像头的DMA采集通道
#define OV7725_EAGLE_W            80                                    //定义摄像头图像宽度
#define OV7725_EAGLE_H            60                                    //定义摄像头图像高度
#define OV7725_EAGLE_SIZE         (OV7725_EAGLE_W * OV7725_EAGLE_H/8 )  //图像占用空间大小
#define OV7725_EAGLE_DMA_NUM      (OV7725_EAGLE_SIZE )                  //DMA采集次数

typedef enum exti_cfg
{
    zero_down     = 0x08u,     //低电平触发，内部下拉
    rising_down   = 0x09u,     //上升沿触发，内部下拉
    falling_down  = 0x0Au,     //下降沿触发，内部下拉
    either_down   = 0x0Bu,     //跳变沿触发，内部下拉
    one_down      = 0x0Cu,     //高电平触发，内部下拉

    //用最高位标志上拉和下拉
    zero_up       = 0x88u,     //低电平触发，内部上拉
    rising_up     = 0x89u,     //上升沿触发，内部上拉
    falling_up    = 0x8Au,     //下降沿触发，内部上拉
    either_up     = 0x8Bu,     //跳变沿触发，内部上拉
    one_up        = 0x8Cu      //高电平触发，内部上拉
} exti_cfg;
typedef enum
{
    IMG_NOTINIT = 0,
    IMG_FINISH,             //图像采集完毕
    IMG_FAIL,               //图像采集失败(采集行数少了)
    IMG_GATHER,             //图像采集中
    IMG_START,              //开始采集图像
    IMG_STOP,               //禁止图像采集
} IMG_STATUS_e;
typedef struct
{
    uint8 addr;                 /*寄存器地址*/
    uint8 val;                   /*寄存器值*/
} reg_s;

extern  u8     	*ov7725_eagle_img_buff;					//扩展定义存放图像的指针
extern  uint8	ov7725_eagle_init(uint8 *imgaddr);		//调用内部函数初始化dma，并把数组地址赋给指针
extern  void	ov7725_eagle_get_img(void);				//获得图像，每次打印前要使用

extern  void	ov7725_eagle_vsync(void);				//场中断服务函数
extern  void	ov7725_eagle_dma(void);					//dma服务函数


#endif  //_VCAN_OV7725_EAGLE_H_


