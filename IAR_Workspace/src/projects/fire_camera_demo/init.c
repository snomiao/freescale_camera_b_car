#include "common.h"
#include "include.h"

extern u16 LEFT_MAX;
extern u16 MIDDLE_DEGREE;
extern u16 RIGHT_MAX;

u8        imgbuff[CAMERA_SIZE];                       //定义一个摄像头图像大小数组
sGPID     steering_gear_pid;                      //定义一个PID结构体变量
sGPID*    pSGPID = &steering_gear_pid;            //定义一个PID结构体指针
motorPID  motor_pid;                           //定义一个PID结构体变量
motorPID* pMotorPID = &motor_pid;              //定义一个PID结构体指针

#define INTERVAL_TICK 4

extern float motor_idle;
extern float steero_middle;


void LCD_DEBUG_ITER(u8* str){
    static Site_type pos = {0, 0}; //左上角
    LCD_Str_8x16(pos, str, BLACK, WHITE);
    pos.y+=16; pos.y %= (16 * 8);
}

void myInit()
{  
    DisableInterrupts;						//禁止总中断
    
    // PIT初始化要放 LCD 前面，否则编码器值不正常
    pit_init_ms(PIT0, 200);             //初始化PIT0，定时时间为： 1000ms
    pit_init_ms(PIT1, 4);                   //初始化PIT1，定时时间为： 2ms
    
    LCD_Init(BLACK);                                     //lcd初始化
    LCD_DEBUG_ITER("INITIALING...");
    
    
    
    LCD_DEBUG_ITER("GPIO");
    gpio_init  (PORTE, 1, GPO, 1);          //编码器
    gpio_init  (PORTE, 2, GPO, 0);
    gpio_init  (PORTE, 3, GPO, 0);
    gpio_init  (PORTB, 7, GPO, LOW);                    //电机使能口（已接线）
    gpio_init  (PORTA, 17, GPO, HIGH);      //单片机led口, LOW表示亮
    //FTM_PWM_init(FTM1, CH0, 20000, motor_middle);         //电机初始化
    
    //串口的初始化一定要放在电机前面
    LCD_DEBUG_ITER("UART");
    UART_IRQ_DIS(UART4);                          //串口 关接收中断
    uart_init(UART4, 19200);
    UART_IRQ_EN(UART4);                           //串口 开接收中断
    
    LCD_DEBUG_ITER("FTM");
    FTM_PWM_init(FTM1, CH1, 20000, (int)motor_idle);         //电机初始化
    FTM_PWM_init(FTM2, CH0, 120, (int)steero_middle);        //舵机初始化

    

    //  pit_init_ms(PIT2, 200);                 //初始化PIT2，定时时间为： 200ms
    //  pit_init_ms(PIT3, 200);                 //初始化PIT3，定时时间为： 200ms

    
    
    LCD_DEBUG_ITER("CAMERA");
    
    camera_init(imgbuff);                               //摄像头初始化
    
    LCD_DEBUG_ITER("LPTMR");
    
    lptmr_counter_init(LPT0_ALT1, 10, 8, LPT_Rising);   //编码器初始化（端口）
    
    
    
    
    sGPID_init(pSGPID);                                 //舵机pid初始化
    motorPID_init(pMotorPID);                           //电机pid初始化
    LCD_Clear(BLACK);
    
    EnableInterrupts;						//开总中断
}