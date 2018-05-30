#include "common.h"
#include "INIT.h"
#include "MENU.h"
#include "LCD.h"
#include "FTM.h"
#include "STEERING_GEAR.h"
#include "MOTOR.h"
#include "uart.h"
#include "IMG_FUNC.h"

extern motorPID*  pMotorPID;				//PID结构体指针
extern int keyOperation;					//按键信号
extern int SG_Pid_Set[6];					//舵机5个pid+电机pid
extern int middlePoint;						//得到的中点值
extern RoadBoundary roadFB[60];				//取得图像
int motorPWMSet1 = INITSPEED1;				//电机pwm值
int motorPWMSet2 = INITSPEED2;				//电机pwm值
float carSpeed = 0;		    		//车速
int minPWM = STOP_PWM;						//最低速度保护
int stopMark = FUNC_CLOSE;					//停车记号
int slowMark = FUNC_OPEN;					//减速记号
/******************************************************
 * 函数名：motorStart
 * 描述  ：调用电机内部函数(外部调用)
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/  
void motorStart(int turn){
	LCD_Num_8x16_X (70,74,carSpeed,BLACK,WHITE);
  	speedChange(turn);
}


/******************************************************
 * 函数名：speedChange
 * 描述  ：调用电机速度(内部调用)
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/  

void speedChange(int turn){
	int tempD,temp2;
	static int speedSet = 0;
	
	tempD = ABS(middlePoint - 40);
	
        speedSet = MAX(0, 120 - 3 * tempD + 0.02 * tempD * tempD);
	
	if(slowMark == FUNC_OPEN){
	  	speedSet = MIN(speedSet, 80); //最高120
	}
	
	if(stopMark == FUNC_OPEN){
		speedSet = 0;
	}
	
	LCD_Num_8x16_X(50, 108, speedSet ,WHITE,BLACK);
	temp2 = motorPID_realize(carSpeed, CONTROLED_MOTOR(speedSet),pMotorPID);
	
        
        
        
	motorPWMSet1 = motorPWMSet1 - temp2;
	motorPWMSet2 = motorPWMSet2 + temp2;
        
        
        //motorPWMSet2 = LIMITED(motorPWMSet2, FTM_PRECISON / 2 - FTM_PRECISON / 6, FTM_PRECISON / 2 + FTM_PRECISON / 6);
        
	//FTM_PWM_Duty(FTM1, CH0, motorPWMSet2); //空接
	FTM_PWM_Duty(FTM1, CH1, LIMITED(motorPWMSet2, 0, FTM_PRECISON)); //主动力
}

/******************************************************
 * 函数名：motorPID_init
 * 描述  ：初始化舵机PID(内部调用)
 * 输入  : sGPID指针
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/  
void motorPID_init(motorPID *pid){ 
	pid->err=0;      
	pid->err_last=0;     
	pid->err_next=0;     
	pid->Kp=30;     
	pid->Ki=10;     
	pid->Kd=100; 
}

/********************************************************
 * 函数名：motorPID_realize
 * 描述  ：实现电机PID(内部调用)
 * 输入  : 目标X值，当前X值，pid指针
 * 输出  ：PWM波
 * 注意  ：	（）测试
*********************************************************/  
int motorPID_realize(int X,int toX, motorPID *pid){
	register int iErr, motor_PWM; 			//当前误差
	pid->SetX = toX;
	pid->ActualX = X;
	iErr = pid->SetX - pid->ActualX; 		//增量计算
	
	if(ABS(iErr-pid->err_last)>15 || pid->ActualX >10){
	   minPWM= STOP_PWM - 35000;
	}else{
	   minPWM= STOP_PWM;
	}
	
//	if(ABS(iErr)>10){
//	   pid->Kp=50; 
//	   pid->Kd=0;  
//	}else{
//	   pid->Kp=25; 
//	   pid->Kd=150; 
//	}
	
	if(ABS(iErr)>10){
	   pid->Kp=100; 
	   pid->Kd=0;  
	}else{
	   pid->Kp=25; 
	   pid->Kd=130; 
	}
	
	motor_PWM  = pid->Kp * (iErr )//- pid->err_last)							//p
			   + pid->Ki * iErr 											//i		
			   + pid->Kd * (iErr + pid->err_next - 2 * pid->err_last); 		//d
	pid->err_next = pid->err_last;			
	pid->err_last = iErr;					//存储误差，用于下次计算
	
//	if(motor_PWM > 50000)
//	  	motor_PWM = 50000;
//	else if(motor_PWM < -50000)
//	  	motor_PWM = -50000;
	
	return(motor_PWM);							//返回增量值
}



