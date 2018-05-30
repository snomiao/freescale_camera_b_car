#include "include.h"

u8 img_buff[80][60] = {0};                  //�洢��ѹͼ��

extern u16 LEFT_MAX;
extern u16 MIDDLE_DEGREE;
extern u16 RIGHT_MAX;

extern u32 time_stamp;

extern u8 *ov7725_eagle_img_buff;           //���������ͼ��ָ�������������
extern int middleBuff[BOTTOM_EDGE];     //����һ������洢�����е�x����
extern int keyOperation;                    //�����ź�
extern u16 CNST_;                           //�Ҷ�ֵ
extern int stopMark;                        //ͣ������
extern int middlePoint;

int speedSet;
extern float carSpeed;
extern motorPID*  pMotorPID;
extern int motorPWMSet1;
extern int motorPWMSet2 ;

//int funcMark = FUNC_CLOSE;                    //���ܿ���
int motorFunc = FUNC_CLOSE;                 //������أ����ڸ����ļ�
int turn;                   //ȫ�ֱ�������¼���ת��pwmֵ������isr�ļ�
int fps_camera = 0;                            //֡��������isr�ļ�
int fps_imgproc = 0;                            //֡��������isr�ļ�
int fps_motion = 0;                            //֡��������isr�ļ�
int fps_camera_count = 0;                            //֡��������isr�ļ�
int fps_imgproc_count = 0;                            //֡��������isr�ļ�
int fps_motion_count = 0;                            //֡��������isr�ļ�
int turnSet;

u32 car_flags = 0;
#define CAR_IDLE        (    0)
#define CAR_RUN_CAMERA  (1<< 1)
#define CAR_RUN_IMGPROC (1<< 2)
#define CAR_RUN_TURN    (1<< 3)
#define CAR_RUN_SPEED   (1<< 4)
#define CAR_RUN_STEERO  (1<< 5)
#define CAR_RUN_MOTOR   (1<< 6)
#define CAR_SEE_CAMERA  (1<< 7)
#define CAR_SEE_IMGPROC (1<< 8)
#define CAR_SEE_TURN    (1<< 9)
#define CAR_SEE_SPEED   (1<<10)
#define CAR_SEE_STEERO  (1<<11)
#define CAR_SEE_MOTOR   (1<<12)

#define CAR_SEE_FPS     (1<<13)

#define CAR_START          
#define CAR_START_NOSPEED  CAR_START & (~CAR_RUN_SPEED)
#define CAR_DEALED_VIEW    CAR_START_NOSPEED | CAR_SEE_IMGPROC | CAR_SEE_MOTOR | CAR_SEE_TURN
#define CAR_CAMERA_VIEW    CAR_START_NOSPEED | CAR_CAMERA_VIEW
#define CAR_CAMERA_VIEW    CAR_START_NOSPEED | CAR_CAMERA_VIEW


// 1 |   | 
// 1 |   | 
// 1 |   | 
// 1 |   | 
// 1 | 1 | 1
// 1 | 1 | 1
// 1 | 1 | 1
// 1 | 1 | 1




// ת���趨
// �ٶ��趨
// Ӳ������

float steero_kp     = 43;
float steero_ki     = 0;
float steero_kd     = -2;
float steero_pid_x  = 8;
float steero_pid_dx = 0;
float speed_max     = 42;
float speed_min     = 25;
float motor_kp      = 1000;
float motor_ki      = 48;
float motor_kd      = 0;
float steero_min    = 14379;
float steero_middle = 22325;
float steero_max    = 30679;
float motor_max     = 100000;
float motor_idle    = 50000;
float motor_min     = 25000;

// volatile float ss_var[17]
// #define steero_kp     (ss_var[1])
// #define steero_ki     (ss_var[2])
// #define steero_kd     (ss_var[3])
// #define steero_pid_x  (ss_var[4])
// #define steero_pid_dx (ss_var[5])
// #define speed_max     (ss_var[6])
// #define speed_min     (ss_var[7])
// #define motor_kp      (ss_var[8])
// #define motor_ki      (ss_var[9])
// #define motor_kd      (ss_var[10])
// #define steero_min    (ss_var[11])
// #define steero_middle (ss_var[12])
// #define steero_max    (ss_var[13])
// #define motor_max     (ss_var[14])
// #define motor_idle    (ss_var[15])
// #define motor_min     (ss_var[16])

/*
steero_kp: 178
steero_ki: 0
steero_kd: 0
steero_pid_x: 6
steero_pid_dx: -1
speed_max: 55
speed_min: 10
motor_kp: 787
motor_ki: 5
motor_kd: 0
steero_min: 12600
steero_middle: 18976
steero_max: 29000
motor_max: 85000
motor_idle: 50000
motor_min: 25000
*/


// 10ms һ�ζ���, �湻200ms�Ķ���
#define STACK_COUNT   20
extern int   motion_list_length = 0;
extern u32   motion_list_time [STACK_COUNT];
extern float motion_list_motor[STACK_COUNT];
extern float motion_list_turn [STACK_COUNT];
extern int ov7725_eagle_img_flag;
extern u32 time_stack;








#define FUNC_I(NAME)                    \
    float NAME(float x){                \
        static float sum = 0;           \
        sum += x;                       \
        return sum;                     \
    }

#define FUNC_D(NAME)                    \
    float NAME(float x){                \
        static float x0;                \
        static char init = 0 ;          \
        if(!init){                      \
            x0 = x;                     \
            init = 1;                   \
        }                               \
        float d = x - x0;               \
        x0 = x;                         \
        return d;                       \
    }

#define FUNC_PID(NAME, KP, KI, KD)      \
    FUNC_I(NAME##_I);                   \
        FUNC_D(NAME##_D);               \
    float NAME(float set, float val){   \
        float err = set - val;          \
                                        \
        float y = 0;                    \
            y += (KP) *          err;   \
        y += (KI) * NAME##_I(err);      \
        y += (KD) * NAME##_D(err);      \
                                        \
        return y;                       \
    }


FUNC_PID(motor_PID , motor_kp , motor_ki , motor_kd );
FUNC_PID(steero_PID, steero_kp, steero_ki, steero_kd);
FUNC_D(middlePoint_D);

// ����΢�ֺ���
FUNC_D(x0_D);
FUNC_D(x1_D);
FUNC_D(x2_D);
FUNC_D(x3_D);

// ������ֺ���
FUNC_I(x0_I);
FUNC_I(x1_I);
FUNC_I(x2_I);
FUNC_I(x3_I);



// ԭ��ͣ������
void PARK(){
    gpio_set(PORTB, 7, HIGH); //�򿪵��ʹ��
    FTM_PWM_Duty(FTM1, CH0, (int)motor_idle);
    FTM_PWM_Duty(FTM1, CH1, (int)motor_idle);
    FTM_PWM_Duty(FTM2, CH0, (int)steero_middle);
    motorFunc = FUNC_CLOSE;
    stopMark = FUNC_CLOSE;
    // ����ʱ���õ��ͣת
    delayms(100);
    gpio_set(PORTB, 7, LOW); //�رյ��ʹ��
}





Site_type LCD_NUM_POS[] = {
    {0,   0},
    {0,  16},
    {0,  32},
    {0,  48},
    {0,  64},
    {0,  80},
    {0,  96},
    {0, 112},
    {40,  64},
    {40,  80},
    {40,  96},
    {40, 112},
    {80,  64},
    {80,  80},
    {80,  96},
    {80, 112},
};
#define LCD_OUT(VAL)       LCD_Num_8x16_X(0,16*(k++), (int)ABS(VAL), VAL < 0 ? BLUE : RED, BLACK);
#define LCD_NUM(POS, VAL)  LCD_Num_8x16_X(LCD_NUM_POS[POS].x,LCD_NUM_POS[POS].y, (int)ABS(VAL), VAL < 0 ? BLUE : RED, BLACK)




/******************************************************
 * ��������img_extract
 * ����  ����ѹͼƬ(��ʱ�ڲ�����)
 * ����  : 
 * ���  ��
 * ע��  ��    ��������
*********************************************************/  
void img_extract_baks(u8 * img_bin)
{
    u8 imgtemp;
    u8 bitindex;
    u8 *pimg=(u8 *)img_bin;
    u8 i,j;
    // 80 * 60
    for(i=59;i>0;i--){
        for(j=0;j<10;j++){
            imgtemp     = *(pimg++);
            bitindex    = 8;
            while(bitindex--){
                if( imgtemp & (0x01<<bitindex) ){
                    img_buff[j*8+7-bitindex][59-i]=1;           //дͼ������BLACK
                }else{
                    img_buff[j*8+7-bitindex][59-i]=0;            //дͼ������WHITE
                }
            }
        }
    }
}

void img_extract(u8 * img_bin)
{
    u8 imgtemp;
    u8 bitindex;
    u8 *pimg=(u8 *)img_bin;
    u8 i, j;
    // 80 * 60
    for(i=59;i>0;i--){
        for(j=0;j<10;j++){
            imgtemp     = *(pimg++);
            bitindex    = 8;
            while(bitindex--){
                if( imgtemp & (0x01<<bitindex) ){
                    img_buff[j*8+7-bitindex][59-i]=1;           //дͼ������BLACK
                }else{
                    img_buff[j*8+7-bitindex][59-i]=0;            //дͼ������WHITE
                }
            }
        }
    }
}

void request_img(){
    ov7725_eagle_img_flag = IMG_START;       //��ʼ�ɼ�ͼ������һ�غϴ���ͼ��
    PORTC_ISFR = ~0;                         //д1���жϱ�־λ(����ģ���Ȼ�ص���һ���жϾ����ϴ����ж�)
    enable_irq(89);                          //����PTA���ж�
}
u8 try_get_img(){
    // �յ�ͼ��
    u8 ret = 0;
    switch(ov7725_eagle_img_flag){
        case IMG_FINISH:
            ret = 1;
        case IMG_FAIL:                               //ע������û��break;
        default:                                    //ע������Ҳû��break;
            request_img();
    }
    return ret;
}


void show_fps(){
    if(car_flags & CAR_SEE_CAMERA){
        LCD_NUM(15, fps_camera);
    }
    if(car_flags & CAR_SEE_IMGPROC){
        LCD_NUM(14, fps_imgproc);
    }
    if((car_flags & CAR_SEE_STEERO) | (car_flags & CAR_SEE_MOTOR)){
        LCD_NUM(13, fps_motion);
    }
}
// LCD_NUM(2, steero - steero_middle);
// LCD_NUM(3, steero);
// LCD_NUM(4, setSpeed);
// LCD_NUM(5, carSpeed);
// LCD_NUM(6, steero);
// LCD_NUM(7, motor);


void car_motion(){
    // ��ʵӦ�÷��ж���������Ϳ�����imageProcessing��ʱ���˶���
    if(0 == time_stack) return;
    time_stack--;
    
    fps_motion_count++;
    // �����ɼ�������
    float  x0 = middlePoint; // ������ĵ�·�е�(-40~-40)
    float dx0 = x0_D(x0);     // ��·�е��΢�֣�ע��΢�ֲ���ֱ���ã���ʱ���й�
    float ix0 = x0_I(x0);     // ��·�е��΢��
    
    float  x1 =  ABS(middlePoint); // ��·�е�ľ���ֵ
    float ix1 = x1_I(x1); // ��·�е�
    float dx1 = x1_D(x1); // ��·�е�

    float  x2 = carSpeed;    // �ٶ�
    float ix2 = x2_I(x2);    // λ��
    float dx2 = x2_D(x2);    // ���ٶ�

    float setSpeed = MAX(0, speed_max - ABS(x0 / 40.0) * MAX(0, speed_max - speed_min));
    float  x3 = setSpeed - carSpeed; // �ٶ����
    float ix3 = x3_I(x3); // λ�����
    float dx3 = x3_D(x3); // ���ٶ����
    

    // ���
    float steero = steero_middle;
    float motor  = motor_idle;

    if(car_flags & CAR_RUN_TURN){
        steero -=   ( steero_kp     *  x0
                    + steero_ki     * ix0
                    + steero_kd     * dx0 )
                    *
                    ( 10 +
                      steero_pid_x  *  x1
                    + steero_pid_dx * dx1 ) / 10; // �������޸�
        if(car_flags & CAR_SEE_TURN){
            LCD_NUM(1, middlePoint);
            LCD_NUM(2, steero - steero_middle);
        }
        steero = LIMITED(steero, steero_min, steero_max);
    }

    if(car_flags & CAR_RUN_STEERO){
        steero = CONTROLED_TURN((int)steero);
        if(car_flags & CAR_SEE_STEERO) LCD_NUM(3, steero);
        FTM_PWM_Duty(FTM2, CH0, (int)steero);        //���ת������
    }

    if(car_flags & CAR_RUN_SPEED){
        if(ix3 > 25000){
            printf("����ͣ�����ٶ������ֹ���: %d\n", ix3);
            keyOperation = KEY_LEFT;
            return;
        }
        motor += ( motor_kp *  x3
                 + motor_ki * ix3
                 + motor_kd * dx3);

      
        if(car_flags & CAR_SEE_SPEED){
            LCD_NUM(4, setSpeed);
            LCD_NUM(5, carSpeed);
        }
        motor  = LIMITED(motor, motor_min, motor_max);
        // ��ת����
        if(carSpeed < 10){
            motor  = LIMITED(motor, motor_idle, motor_max);
        }
        LCD_NUM(6, steero);
    }
    if(car_flags & CAR_RUN_MOTOR){
        motor = CONTROLED_MOTOR((int)motor);
        if(car_flags & CAR_SEE_MOTOR) LCD_NUM(7, motor);
        FTM_PWM_Duty(FTM1, CH1, (int)motor);    //������
    }
}

u8 car_loop(){
    if(car_flags & CAR_RUN_CAMERA){
        if(try_get_img()){
            if(car_flags & CAR_RUN_IMGPROC){
                img_extract((u8*)ov7725_eagle_img_buff); //��ѹ
                imageProcessing();                       //����ͼ����, �������Ӧ�û�ռ��ѭ���Ĵ󲿷�ʱ�䡣
                
                
                if(car_flags & CAR_SEE_IMGPROC){
                    LCD_Img(img_buff);                        //��ӡ�������ͼ��
                }
                fps_imgproc_count++;
            }
        }
    }

    if(car_flags & CAR_SEE_CAMERA){
        LCD_Img_Binary((u8 *)ov7725_eagle_img_buff); //��ӡ����ͷͼ��
    }

    // TODO: ���������sei�ж�����
    car_motion();
    if(keyOperation == KEY_LEFT){                      //�������м��ʱ��                  
        keyOperation = KEY_NULL;
        PARK();
        return 0;
    }
    return 1;
}

void car_run_init(){
    if(car_flags & CAR_RUN_MOTOR){
        gpio_set(PORTB, 7, HIGH); // �򿪵��ʹ��
        motorFunc = FUNC_OPEN;
    }

    // ��ջ��۵� I
    x0_I(-x0_I(0));
    x1_I(-x1_I(0));
    x2_I(-x2_I(0));
    x3_I(-x3_I(0));
    
    LCD_Clear(BLACK);
    time_stack = 0; //��ռ�ʱջ
}






void START(){
    car_flags = ( CAR_RUN_CAMERA | CAR_RUN_IMGPROC |
                  CAR_RUN_TURN   | CAR_RUN_STEERO  |
                  CAR_RUN_MOTOR  | CAR_RUN_SPEED   );
    car_run_init();
    while(car_loop());
    car_flags = CAR_IDLE;
}

void LEARN(){
    car_flags = (
        CAR_RUN_STEERO | CAR_RUN_MOTOR   |
        CAR_RUN_TURN   | CAR_RUN_SPEED   |
        CAR_RUN_CAMERA | CAR_RUN_IMGPROC |
        CAR_SEE_FPS    |
        0 );
    car_run_init();
    while(car_loop());
    car_flags = CAR_IDLE;
}


void STEERO_DEBUG(){
    car_flags = (
        CAR_RUN_STEERO | CAR_RUN_MOTOR   |
        CAR_RUN_TURN   |
        CAR_RUN_CAMERA | CAR_RUN_IMGPROC |
        0 );
    car_run_init();
    while(car_loop());
    car_flags = CAR_IDLE;
}

void CONTROL(){
    car_flags = ( CAR_RUN_STEERO | CAR_RUN_MOTOR );
    car_run_init();
    while(car_loop());
    car_flags = CAR_IDLE;
}

void CameraView(){
    car_flags = (
        CAR_RUN_CAMERA  |
        CAR_RUN_STEERO  | CAR_RUN_MOTOR |
        CAR_SEE_CAMERA  |
        0 );
    car_run_init();
    while(car_loop()){
        if(keyOperation == KEY_UP){
            keyOperation = KEY_NULL;
            CNST_++;
        }
        if(keyOperation == KEY_DOWN){
            keyOperation = KEY_NULL;
            CNST_--;
        }
        SCCB_WriteByte( 0x9C, CNST_);			//i2cЭ�飬��������ͷ�Ҷ�
        LCD_NUM(4, (int)CNST_);
    }
    car_flags = CAR_IDLE;
}

void DealedView(){
    car_flags = (
        CAR_RUN_CAMERA  | CAR_RUN_IMGPROC |
        CAR_RUN_STEERO  | CAR_RUN_MOTOR   |
        CAR_RUN_TURN    |
        CAR_SEE_STEERO  |
        CAR_SEE_TURN    |
        CAR_SEE_IMGPROC |
        0 );
    car_run_init();
    while(car_loop());
    car_flags = CAR_IDLE;
}