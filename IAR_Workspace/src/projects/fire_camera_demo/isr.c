/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名       ：isr.c
 * 描述         ：摄像头小车中断都在这
 * 作者         ：沈泽皓
**********************************************************************************/

#include "include.h"



extern int middlePoint;

u32 time_stamp = 0;

//volatile u16 fps_camera_count = 0;
volatile u32 LPT_INT_count = 0;
volatile u32 time_stack;                      //运动计时栈
extern u8 *ov7725_eagle_img_buff;           //扩大二进制图像指针变量的作用域
extern int keyOperation;
extern int turn;                //舵机PWM占空比
extern float carSpeed;
extern int fps_motion;                         //帧数
extern int fps_imgproc;                         //帧数
extern int fps_camera;                         //帧数
extern int fps_camera_count;
extern int fps_motion_count;                         //帧数
extern int fps_imgproc_count;                         //帧数

extern int motorFunc;                       //电机开关
extern int stopMark;                        //停车开关
extern int delaytime;                       //延迟时间
extern int runState;                        //舵机中点

extern u16 LEFT_MAX;
extern u16 MIDDLE_DEGREE;
extern u16 RIGHT_MAX;


extern int motorPWMSet1;                        //舵机中点
extern int motorPWMSet2;                        //舵机中点

extern float motor_kp;
extern float motor_ki;
extern float motor_kd;

extern float steero_pid_x;
extern float steero_pid_dx;

extern float steero_kp;
extern float steero_ki;
extern float steero_kd;
extern float steero_middle;
extern float steero_min;
extern float steero_max;
extern float steero_pid_rate;
extern float speed_max;
extern float speed_min;

extern float motor_idle;
extern float motor_max;
extern float motor_min;
/*************************************************************************
*  函数名称：PORTC_IRQHandler
*  功能说明：PORTC端口中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2017-4-2    已测试
*  备    注：摄像头场中断
*************************************************************************/
void PORTC_IRQHandler()
{
    u8  n;                          //引脚号 Cn
    u32 flag;

    while(!PORTC_ISFR);
    flag = PORTC_ISFR;              //清中断标志位
    n = 19;                         //场中断
    if(flag & (1 << n))             //PTA29触发中断
    {
        camera_vsync();             //场中断服务函数
    }
    PORTC_ISFR  = ~0; 
}

/*************************************************************************
*  函数名称：DMA0_IRQHandler
*  功能说明：dma中断服务函数，读取摄像头信号
*  参数说明：无
*  函数返回：无
*  修改时间：2017-4-2    已测试
*  备    注：摄像头dma中断
*************************************************************************/
void DMA0_IRQHandler()
{
    camera_dma();
    fps_camera_count++;
}

/*************************************************************************
*  函数名称：LPT_Handler
*  功能说明：lpt中断服务函数，读取编码器值
*  参数说明：无
*  函数返回：无
*  修改时间：2017-4-2    已测试
*  备    注：lpt中断
*************************************************************************/
void  LPT_Handler(void)
{
    LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;   //清除LPTMR比较标志
    LPT_INT_count++;                    //中断溢出加1
}

/*************************************************************************
*  函数名称：PIT0_IRQHandler
*  功能说明：PIT0定时中断服务函数，用于读取帧数
*  参数说明：无
*  函数返回：无
*  修改时间：
*  备    注：定时中断 200ms
*************************************************************************/
void PIT0_IRQHandler(void)
{
    static int num = 0;
    PIT_Flag_Clear(PIT0);       // 清中断标志位
    keyOperation = getKey();    // 得到按键
//  LCD_Num_8x16_X(70,8,Vnum ,RED,BLUE);
    if(num == 5){
        
        fps_camera  = fps_camera_count;

        show_fps();
        fps_imgproc = fps_imgproc_count;
        fps_camera  = fps_camera_count;
        fps_motion  = fps_motion_count;
        fps_imgproc_count = 0;
        fps_camera_count  = 0;
        fps_motion_count = 0;
        num = 0;
    }
    num++;
}

/*************************************************************************
*  函数名称：PIT1_IRQHandler
*  功能说明：PIT1定时中断服务函数，用于精准计时
*  参数说明：无
*  函数返回：无
*  修改时间：
*  备    注：定时中断 4ms
*************************************************************************/
void PIT1_IRQHandler(void)
{
    time_stamp++;
    
    time_stack++;

    s16 count = 0;
    s16 speed = 0;
    
    /*
    
    char c;
    c = uart_myGetChar(UART4);
    if(c != 0){
          uart_putchar(UART4,c);
    }
    if(c == 's'){
        stopMark = FUNC_OPEN;
//      LCD_Num_8x16_X (50,100,1,BLACK,WHITE);
    }else if(c == 'g'){
        stopMark = FUNC_CLOSE;
    }
    
    */
    
    //if(motorFunc == FUNC_OPEN)
    if(1)
    {
        count = LPTMR0_CNR;                     //保存脉冲计数器计算值
        lptmr_counter_clean();                  //清空脉冲计数器计算值（马上清空，这样才能保证计数值准确）
        speed = LPT_INT_count * 10 + count;    //得到编码器数值
        //LCD_Num_8x16_X(70,24,speed,BLACK, WHITE);   //打印编码器示数
        //sendValue(UART4, speed);
        
        //printf("%d\n", speed);
        
        carSpeed = speed;
        // speedChange(turn);
        LPT_INT_count   =   0; 
    }
    /*
    delaytime++;
    if(delaytime == 100){
        runState = 40;
    }
    */
    PIT_Flag_Clear(PIT1);       //清中断标志位
}

/*************************************************************************
*  函数名称：PIT2_IRQHandler
*  功能说明：PIT2定时中断服务函数，用于回传图像
*  参数说明：无
*  函数返回：无
*  修改时间：
*  备    注：定时中断
*************************************************************************/
void PIT2_IRQHandler(void)
{
    uart_putchar(UART4,0x01);
    uart_putchar(UART4,0xFE);
    uart_sendN(UART4,ov7725_eagle_img_buff,600);    //发送n个字节
    uart_putchar(UART4,0xFE);
    uart_putchar(UART4,0x01);
    PIT_Flag_Clear(PIT2);       //清中断标志位
}

/*************************************************************************
*  函数名称：USART3_IRQHandler
*  功能说明：串口指令中断
*  参数说明：无
*  函数返回：无
*  修改时间：
*  备    注：
*************************************************************************/
void USART4_IRQHandler(void)
{
 
    uint8 ch;

    
    //DisableInterrupts;          //关总中断

    //接收一个字节数据并回发
    ch = uart_myGetChar (UART4);              //接收到一个数据
    
    static char command_open = 0;
    
    if( ch == '/' ) {
      command_open = 1;
    };
    if( ch == '\n' ) {
      command_open = 0;
    };
      
    if(command_open){
        switch(ch){
            case '1': printf("steero_kp: %d\n"    , (int)(++steero_kp)    ); break;
            case 'Z': printf("steero_kp: %d\n"    , (int)(--steero_kp)    ); break;
        //  case ' ': printf("steero_ki: %d\n"    , (int)(++steero_ki)    ); break;
        //  case ' ': printf("steero_ki: %d\n"    , (int)(--steero_ki)    ); break;
            case '2': printf("steero_kd: %d\n"    , (int)(++steero_kd)    ); break;
            case 'X': printf("steero_kd: %d\n"    , (int)(--steero_kd)    ); break;
            case '3': printf("steero_pid_x: %d\n" , (int)(++steero_pid_x) ); break;
            case 'C': printf("steero_pid_x: %d\n" , (int)(--steero_pid_x) ); break;
            //case '4': printf("steero_pid_dx: %d\n" , (int)(++steero_pid_dx) ); break;
            //case 'V': printf("steero_pid_dx: %d\n" , (int)(--steero_pid_dx) ); break;
            case '5': printf("speed_max: %d\n"    , (int)(++speed_max)    ); break;
            case 'B': printf("speed_max: %d\n"    , (int)(--speed_max)    ); break;
            case '4': printf("speed_min: %d\n"    , (int)(++speed_min)    ); break;
            case 'V': printf("speed_min: %d\n"    , (int)(--speed_min)    ); break;
            case '6': printf("motor_kp: %d\n"     , (int)(++motor_kp)     ); break;
            case 'N': printf("motor_kp: %d\n"     , (int)(--motor_kp)     ); break;
            case '7': printf("motor_ki: %d\n"     , (int)(++motor_ki)     ); break;
            case 'M': printf("motor_ki: %d\n"     , (int)(--motor_ki)     ); break;
        //  case '': printf("motor_kd: %d\n"     , (int)(++motor_kd)     ); break;
        //  case '': printf("motor_kd: %d\n"     , (int)(--motor_kd)     ); break;
        //  case ' ': printf("steero_min: %d\n"   , (int)(++steero_min)   ); break;
        //  case ' ': printf("steero_min: %d\n"   , (int)(--steero_min)   ); break;
            case 'Q': printf("steero_middle: %d\n", (int)(++steero_middle)); break;
            case 'E': printf("steero_middle: %d\n", (int)(--steero_middle)); break;
        //  case ' ': printf("steero_max: %d\n"   , (int)(++steero_max)   ); break;
        //  case ' ': printf("steero_max: %d\n"   , (int)(--steero_max)   ); break;
        //  case ' ': printf("motor_max: %d\n"    , (int)(++motor_max)    ); break;
        //  case ' ': printf("motor_max: %d\n"    , (int)(--motor_max)    ); break;
        //  case ' ': printf("motor_idle: %d\n"   , (int)(++motor_idle)   ); break;
        //  case ' ': printf("motor_idle: %d\n"   , (int)(--motor_idle)   ); break;
        //  case ' ': printf("motor_min: %d\n"    , (int)(++motor_min)    ); break;
        //  case ' ': printf("motor_min: %d\n"    , (int)(--motor_min)    ); break;

            case 'J': keyOperation = KEY_DOWN;   break;
            case 'K': keyOperation = KEY_UP;     break;
            case 'H': keyOperation = KEY_LEFT;   break;
            case 'L': keyOperation = KEY_RIGHT;  break;
            case 'G': keyOperation = KEY_MIDDLE; break;
          
            case 'W': CONTROL_MOTOR(85000, 20); break;
            case 'S': CONTROL_MOTOR(25000, 20); break;
            case 'A': CONTROL_TURN((int)steero_max, 10); break; //  leftmax
            case 'D': CONTROL_TURN((int)steero_min, 10); break; // rightmax

            case 'U': //获取各项参数2
                printf(
                    "volatile float steero_kp     = %d;\n"
                    "volatile float steero_ki     = %d;\n"
                    "volatile float steero_kd     = %d;\n"
                    "volatile float steero_pid_x  = %d;\n"
                    "volatile float steero_pid_dx = %d;\n"
                    "volatile float speed_max     = %d;\n"
                    "volatile float speed_min     = %d;\n"
                    "volatile float motor_kp      = %d;\n"
                    "volatile float motor_ki      = %d;\n"
                    "volatile float motor_kd      = %d;\n"
                    "volatile float steero_min    = %d;\n"
                    "volatile float steero_middle = %d;\n"
                    "volatile float steero_max    = %d;\n"
                    "volatile float motor_max     = %d;\n"
                    "volatile float motor_idle    = %d;\n"
                    "volatile float motor_min     = %d;\n",
                    (int)steero_kp,
                    (int)steero_ki,
                    (int)steero_kd,
                    (int)steero_pid_x,
                    (int)steero_pid_dx,
                    (int)speed_max,
                    (int)speed_min,
                    (int)motor_kp,
                    (int)motor_ki,
                    (int)motor_kd,
                    (int)steero_min,
                    (int)steero_middle,
                    (int)steero_max,
                    (int)motor_max,
                    (int)motor_idle,
                    (int)motor_min);
                    break;
               case 'Y': //获取传感器的值
                 printf("middlePoint: %d\n", (int)middlePoint);
                 printf("carSpeed: %d\n", (int)carSpeed);
                  break;
            case 'T': //获取图像
                uart_putchar(UART4,0x01);
                uart_putchar(UART4,0xFE);
                uart_sendN(UART4,ov7725_eagle_img_buff,600);    //发送n个字节
                uart_putchar(UART4,0xFE);
                uart_putchar(UART4,0x01);
                break;
            default:
                printf("%c", ch);     //指令回显
                break;
      }
      return;
    };
    printf("%c", ch);     //指令回显

    //EnableInterrupts;           //开总中断

}

