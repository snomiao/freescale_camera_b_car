#include "common.h"
#include "INIT.h"
#include "MENU.h"
#include "LCD.h"
#include "FTM.h"
#include "STEERING_GEAR.h"
#include "MOTOR.h"
#include "uart.h"
#include "IMG_FUNC.h"
#include  "common.h"
#include  "STEERING_GEAR.h"
#include  "FTM.h"
#include  "MENU.h"
#include  "LCD.h"
#include  "BL144002.h"
#include  "ASCII.h"	
#include  "IMG_FUNC.h"
#include  "MOTOR.h"
#include  "uart.h"

extern sGPID*  pSGPID;			//����һ��PID�ṹ��ָ��
extern int keyOperation;		//��������
extern int SG_Pid_Set[6];		//���5��pid
extern int middlePoint;								//�������ߵõ���һ��ƫ���
extern int pointNum;									//��Ч������
int outProMark = FUNC_CLOSE;	//��������
int runState = 40; // ??

extern motorPID*  pMotorPID;				//PID�ṹ��ָ��
extern int keyOperation;					//�����ź�
extern int SG_Pid_Set[6];					//���5��pid+���pid
extern int middlePoint;						//�õ����е�ֵ
extern RoadBoundary roadFB[60];				//ȡ��ͼ��

int motorPWMSet1 = INITSPEED1;				//���pwmֵ
int motorPWMSet2 = INITSPEED2;				//���pwmֵ
int carSpeed = 0;		    		//����
int minPWM = STOP_PWM;						//����ٶȱ���
int stopMark = FUNC_CLOSE;					//ͣ���Ǻ�
int slowMark = FUNC_OPEN;					//���ټǺ�

#include "common.h"
#include "INIT.h"
#include "MENU.h"
#include "LCD.h"
#include "FTM.h"
#include "uart.h"
#include "BL144002.h"
#include "ASCII.h"  
#include "IMG_FUNC.h"

extern int keyOperation;                    // �����ź�
extern int SG_Pid_Set[6];                   // ���5��pid+���pid
extern int middlePoint;                     // �õ����е�ֵ
extern RoadBoundary roadFB[60];             // ȡ��ͼ��

extern int pointNum;                        // ��Ч������

int outProMark   = FUNC_CLOSE;                      // ��������
int runState     = 40;                              //  ??


int motorPWMSet1 = INITSPEED1;                      // ���pwmֵ
int motorPWMSet2 = INITSPEED2;                      // ���pwmֵ
int carSpeed     = 0;                               // ����
int minPWM       = STOP_PWM;                        // ����ٶȱ���
int stopMark     = FUNC_CLOSE;                      // ͣ���Ǻ�
int slowMark     = FUNC_OPEN;                       // ���ټǺ�








#define PID_FUNC(TYPE, NAME)                           \
    TYPE PID_##NAME(TYPE now, target, kp, ki, kd){     \
        static TYPE error_sum = 0, error_last = 0;     \
        TYPE error, controled_out, error_difference;   \
                                                       \
        /* PID calculate                            */ \
        error = target - now;                          \
        error_sum += error;                            \
        error_difference = error - error_last;         \
                                                       \
        /* PID control                              */ \
        controled_out = (                              \
            kp * error +                               \
            ki * error_sum +                           \
            kd * error_difference                      \
            );                                         \
                                                       \
        /* Record value for Integral and Difference */ \
        value_last = VALUE;                            \
        error_last = error;                            \
        return VALUE_controled_out;                    \
    }

PID_FUNC(float, SPEED);
//PID_FUNC(float, target);


output = PID_SPEED(speed, speed_target, speed_kp);
PID_SPEED(speed, v4, v5, v6)












    
    