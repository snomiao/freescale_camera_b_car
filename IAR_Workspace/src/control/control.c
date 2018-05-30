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

extern sGPID*  pSGPID;			//定义一个PID结构体指针
extern int keyOperation;		//按键变量
extern int SG_Pid_Set[6];		//舵机5个pid
extern int middlePoint;								//根据中线得到的一个偏差点
extern int pointNum;									//有效点数量
int outProMark = FUNC_CLOSE;	//打死开关
int runState = 40; // ??

extern motorPID*  pMotorPID;				//PID结构体指针
extern int keyOperation;					//按键信号
extern int SG_Pid_Set[6];					//舵机5个pid+电机pid
extern int middlePoint;						//得到的中点值
extern RoadBoundary roadFB[60];				//取得图像

int motorPWMSet1 = INITSPEED1;				//电机pwm值
int motorPWMSet2 = INITSPEED2;				//电机pwm值
int carSpeed = 0;		    		//车速
int minPWM = STOP_PWM;						//最低速度保护
int stopMark = FUNC_CLOSE;					//停车记号
int slowMark = FUNC_OPEN;					//减速记号

#include "common.h"
#include "INIT.h"
#include "MENU.h"
#include "LCD.h"
#include "FTM.h"
#include "uart.h"
#include "BL144002.h"
#include "ASCII.h"  
#include "IMG_FUNC.h"

extern int keyOperation;                    // 按键信号
extern int SG_Pid_Set[6];                   // 舵机5个pid+电机pid
extern int middlePoint;                     // 得到的中点值
extern RoadBoundary roadFB[60];             // 取得图像

extern int pointNum;                        // 有效点数量

int outProMark   = FUNC_CLOSE;                      // 打死开关
int runState     = 40;                              //  ??


int motorPWMSet1 = INITSPEED1;                      // 电机pwm值
int motorPWMSet2 = INITSPEED2;                      // 电机pwm值
int carSpeed     = 0;                               // 车速
int minPWM       = STOP_PWM;                        // 最低速度保护
int stopMark     = FUNC_CLOSE;                      // 停车记号
int slowMark     = FUNC_OPEN;                       // 减速记号








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












    
    