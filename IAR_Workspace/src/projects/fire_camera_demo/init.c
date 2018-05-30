#include "common.h"
#include "include.h"

extern u16 LEFT_MAX;
extern u16 MIDDLE_DEGREE;
extern u16 RIGHT_MAX;

u8        imgbuff[CAMERA_SIZE];                       //����һ������ͷͼ���С����
sGPID     steering_gear_pid;                      //����һ��PID�ṹ�����
sGPID*    pSGPID = &steering_gear_pid;            //����һ��PID�ṹ��ָ��
motorPID  motor_pid;                           //����һ��PID�ṹ�����
motorPID* pMotorPID = &motor_pid;              //����һ��PID�ṹ��ָ��

#define INTERVAL_TICK 4

extern float motor_idle;
extern float steero_middle;


void LCD_DEBUG_ITER(u8* str){
    static Site_type pos = {0, 0}; //���Ͻ�
    LCD_Str_8x16(pos, str, BLACK, WHITE);
    pos.y+=16; pos.y %= (16 * 8);
}

void myInit()
{  
    DisableInterrupts;						//��ֹ���ж�
    
    // PIT��ʼ��Ҫ�� LCD ǰ�棬���������ֵ������
    pit_init_ms(PIT0, 200);             //��ʼ��PIT0����ʱʱ��Ϊ�� 1000ms
    pit_init_ms(PIT1, 4);                   //��ʼ��PIT1����ʱʱ��Ϊ�� 2ms
    
    LCD_Init(BLACK);                                     //lcd��ʼ��
    LCD_DEBUG_ITER("INITIALING...");
    
    
    
    LCD_DEBUG_ITER("GPIO");
    gpio_init  (PORTE, 1, GPO, 1);          //������
    gpio_init  (PORTE, 2, GPO, 0);
    gpio_init  (PORTE, 3, GPO, 0);
    gpio_init  (PORTB, 7, GPO, LOW);                    //���ʹ�ܿڣ��ѽ��ߣ�
    gpio_init  (PORTA, 17, GPO, HIGH);      //��Ƭ��led��, LOW��ʾ��
    //FTM_PWM_init(FTM1, CH0, 20000, motor_middle);         //�����ʼ��
    
    //���ڵĳ�ʼ��һ��Ҫ���ڵ��ǰ��
    LCD_DEBUG_ITER("UART");
    UART_IRQ_DIS(UART4);                          //���� �ؽ����ж�
    uart_init(UART4, 19200);
    UART_IRQ_EN(UART4);                           //���� �������ж�
    
    LCD_DEBUG_ITER("FTM");
    FTM_PWM_init(FTM1, CH1, 20000, (int)motor_idle);         //�����ʼ��
    FTM_PWM_init(FTM2, CH0, 120, (int)steero_middle);        //�����ʼ��

    

    //  pit_init_ms(PIT2, 200);                 //��ʼ��PIT2����ʱʱ��Ϊ�� 200ms
    //  pit_init_ms(PIT3, 200);                 //��ʼ��PIT3����ʱʱ��Ϊ�� 200ms

    
    
    LCD_DEBUG_ITER("CAMERA");
    
    camera_init(imgbuff);                               //����ͷ��ʼ��
    
    LCD_DEBUG_ITER("LPTMR");
    
    lptmr_counter_init(LPT0_ALT1, 10, 8, LPT_Rising);   //��������ʼ�����˿ڣ�
    
    
    
    
    sGPID_init(pSGPID);                                 //���pid��ʼ��
    motorPID_init(pMotorPID);                           //���pid��ʼ��
    LCD_Clear(BLACK);
    
    EnableInterrupts;						//�����ж�
}