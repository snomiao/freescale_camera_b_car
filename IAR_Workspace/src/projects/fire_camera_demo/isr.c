/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���       ��isr.c
 * ����         ������ͷС���ж϶�����
 * ����         �������
**********************************************************************************/

#include "include.h"



extern int middlePoint;

u32 time_stamp = 0;

//volatile u16 fps_camera_count = 0;
volatile u32 LPT_INT_count = 0;
volatile u32 time_stack;                      //�˶���ʱջ
extern u8 *ov7725_eagle_img_buff;           //���������ͼ��ָ�������������
extern int keyOperation;
extern int turn;                //���PWMռ�ձ�
extern float carSpeed;
extern int fps_motion;                         //֡��
extern int fps_imgproc;                         //֡��
extern int fps_camera;                         //֡��
extern int fps_camera_count;
extern int fps_motion_count;                         //֡��
extern int fps_imgproc_count;                         //֡��

extern int motorFunc;                       //�������
extern int stopMark;                        //ͣ������
extern int delaytime;                       //�ӳ�ʱ��
extern int runState;                        //����е�

extern u16 LEFT_MAX;
extern u16 MIDDLE_DEGREE;
extern u16 RIGHT_MAX;


extern int motorPWMSet1;                        //����е�
extern int motorPWMSet2;                        //����е�

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
*  �������ƣ�PORTC_IRQHandler
*  ����˵����PORTC�˿��жϷ�����
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2017-4-2    �Ѳ���
*  ��    ע������ͷ���ж�
*************************************************************************/
void PORTC_IRQHandler()
{
    u8  n;                          //���ź� Cn
    u32 flag;

    while(!PORTC_ISFR);
    flag = PORTC_ISFR;              //���жϱ�־λ
    n = 19;                         //���ж�
    if(flag & (1 << n))             //PTA29�����ж�
    {
        camera_vsync();             //���жϷ�����
    }
    PORTC_ISFR  = ~0; 
}

/*************************************************************************
*  �������ƣ�DMA0_IRQHandler
*  ����˵����dma�жϷ���������ȡ����ͷ�ź�
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2017-4-2    �Ѳ���
*  ��    ע������ͷdma�ж�
*************************************************************************/
void DMA0_IRQHandler()
{
    camera_dma();
    fps_camera_count++;
}

/*************************************************************************
*  �������ƣ�LPT_Handler
*  ����˵����lpt�жϷ���������ȡ������ֵ
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2017-4-2    �Ѳ���
*  ��    ע��lpt�ж�
*************************************************************************/
void  LPT_Handler(void)
{
    LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;   //���LPTMR�Ƚϱ�־
    LPT_INT_count++;                    //�ж������1
}

/*************************************************************************
*  �������ƣ�PIT0_IRQHandler
*  ����˵����PIT0��ʱ�жϷ����������ڶ�ȡ֡��
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺
*  ��    ע����ʱ�ж� 200ms
*************************************************************************/
void PIT0_IRQHandler(void)
{
    static int num = 0;
    PIT_Flag_Clear(PIT0);       // ���жϱ�־λ
    keyOperation = getKey();    // �õ�����
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
*  �������ƣ�PIT1_IRQHandler
*  ����˵����PIT1��ʱ�жϷ����������ھ�׼��ʱ
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺
*  ��    ע����ʱ�ж� 4ms
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
        count = LPTMR0_CNR;                     //�����������������ֵ
        lptmr_counter_clean();                  //����������������ֵ��������գ��������ܱ�֤����ֵ׼ȷ��
        speed = LPT_INT_count * 10 + count;    //�õ���������ֵ
        //LCD_Num_8x16_X(70,24,speed,BLACK, WHITE);   //��ӡ������ʾ��
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
    PIT_Flag_Clear(PIT1);       //���жϱ�־λ
}

/*************************************************************************
*  �������ƣ�PIT2_IRQHandler
*  ����˵����PIT2��ʱ�жϷ����������ڻش�ͼ��
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺
*  ��    ע����ʱ�ж�
*************************************************************************/
void PIT2_IRQHandler(void)
{
    uart_putchar(UART4,0x01);
    uart_putchar(UART4,0xFE);
    uart_sendN(UART4,ov7725_eagle_img_buff,600);    //����n���ֽ�
    uart_putchar(UART4,0xFE);
    uart_putchar(UART4,0x01);
    PIT_Flag_Clear(PIT2);       //���жϱ�־λ
}

/*************************************************************************
*  �������ƣ�USART3_IRQHandler
*  ����˵��������ָ���ж�
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺
*  ��    ע��
*************************************************************************/
void USART4_IRQHandler(void)
{
 
    uint8 ch;

    
    //DisableInterrupts;          //�����ж�

    //����һ���ֽ����ݲ��ط�
    ch = uart_myGetChar (UART4);              //���յ�һ������
    
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

            case 'U': //��ȡ�������2
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
               case 'Y': //��ȡ��������ֵ
                 printf("middlePoint: %d\n", (int)middlePoint);
                 printf("carSpeed: %d\n", (int)carSpeed);
                  break;
            case 'T': //��ȡͼ��
                uart_putchar(UART4,0x01);
                uart_putchar(UART4,0xFE);
                uart_sendN(UART4,ov7725_eagle_img_buff,600);    //����n���ֽ�
                uart_putchar(UART4,0xFE);
                uart_putchar(UART4,0x01);
                break;
            default:
                printf("%c", ch);     //ָ�����
                break;
      }
      return;
    };
    printf("%c", ch);     //ָ�����

    //EnableInterrupts;           //�����ж�

}

