#include "common.h"
#include "INIT.h"
#include "MENU.h"
#include "LCD.h"
#include "FTM.h"
#include "STEERING_GEAR.h"
#include "MOTOR.h"
#include "uart.h"
#include "IMG_FUNC.h"

extern motorPID*  pMotorPID;				//PID�ṹ��ָ��
extern int keyOperation;					//�����ź�
extern int SG_Pid_Set[6];					//���5��pid+���pid
extern int middlePoint;						//�õ����е�ֵ
extern RoadBoundary roadFB[60];				//ȡ��ͼ��
int motorPWMSet1 = INITSPEED1;				//���pwmֵ
int motorPWMSet2 = INITSPEED2;				//���pwmֵ
float carSpeed = 0;		    		//����
int minPWM = STOP_PWM;						//����ٶȱ���
int stopMark = FUNC_CLOSE;					//ͣ���Ǻ�
int slowMark = FUNC_OPEN;					//���ټǺ�
/******************************************************
 * ��������motorStart
 * ����  �����õ���ڲ�����(�ⲿ����)
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/  
void motorStart(int turn){
	LCD_Num_8x16_X (70,74,carSpeed,BLACK,WHITE);
  	speedChange(turn);
}


/******************************************************
 * ��������speedChange
 * ����  �����õ���ٶ�(�ڲ�����)
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/  

void speedChange(int turn){
	int tempD,temp2;
	static int speedSet = 0;
	
	tempD = ABS(middlePoint - 40);
	
        speedSet = MAX(0, 120 - 3 * tempD + 0.02 * tempD * tempD);
	
	if(slowMark == FUNC_OPEN){
	  	speedSet = MIN(speedSet, 80); //���120
	}
	
	if(stopMark == FUNC_OPEN){
		speedSet = 0;
	}
	
	LCD_Num_8x16_X(50, 108, speedSet ,WHITE,BLACK);
	temp2 = motorPID_realize(carSpeed, CONTROLED_MOTOR(speedSet),pMotorPID);
	
        
        
        
	motorPWMSet1 = motorPWMSet1 - temp2;
	motorPWMSet2 = motorPWMSet2 + temp2;
        
        
        //motorPWMSet2 = LIMITED(motorPWMSet2, FTM_PRECISON / 2 - FTM_PRECISON / 6, FTM_PRECISON / 2 + FTM_PRECISON / 6);
        
	//FTM_PWM_Duty(FTM1, CH0, motorPWMSet2); //�ս�
	FTM_PWM_Duty(FTM1, CH1, LIMITED(motorPWMSet2, 0, FTM_PRECISON)); //������
}

/******************************************************
 * ��������motorPID_init
 * ����  ����ʼ�����PID(�ڲ�����)
 * ����  : sGPIDָ��
 * ���  ��void
 * ע��  ��	��������
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
 * ��������motorPID_realize
 * ����  ��ʵ�ֵ��PID(�ڲ�����)
 * ����  : Ŀ��Xֵ����ǰXֵ��pidָ��
 * ���  ��PWM��
 * ע��  ��	��������
*********************************************************/  
int motorPID_realize(int X,int toX, motorPID *pid){
	register int iErr, motor_PWM; 			//��ǰ���
	pid->SetX = toX;
	pid->ActualX = X;
	iErr = pid->SetX - pid->ActualX; 		//��������
	
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
	pid->err_last = iErr;					//�洢�������´μ���
	
//	if(motor_PWM > 50000)
//	  	motor_PWM = 50000;
//	else if(motor_PWM < -50000)
//	  	motor_PWM = -50000;
	
	return(motor_PWM);							//��������ֵ
}



