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

//����Ƕ��������
volatile u16 LEFT_MAX 	       = 26500;						//�����ת���PWM
volatile u16 MIDDLE_DEGREE    = 19500;						//����м�PWM
volatile u16 RIGHT_MAX        = 12600;						//�����ת���PWM


extern sGPID*  pSGPID;			//����һ��PID�ṹ��ָ��
extern int keyOperation;		//��������
extern int SG_Pid_Set[6];		//���5��pid
extern int middlePoint;								//�������ߵõ���һ��ƫ���
extern int pointNum;					
				//��Ч������
int outProMark = FUNC_CLOSE;	//��������
int runState = 40; // ??

extern float steero_middle;
/******************************************************
 * ��������sGStart
 * ����  �����ö���ڲ�����(�ⲿ����)
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/  
int sGStart(int* Point){	
	LCD_Num_8x16_X(70,88,pSGPID->Kp ,WHITE,BLACK);		//��ӡSGpֵ	
	return followRoad();
}

/******************************************************
 * ��������followRoad
 * ����  ��Ѱ·����(�ڲ�����)
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/  

#define Polynomial2(x, a, b, c) (a + b * x + c * x * x)

int followRoad(void){
	int tempD = 0;					//���ڵõ�������Ũ���ĵ�
	int turn = MIDDLE_DEGREE;				//���PWMռ�ձ�
	
	tempD = middlePoint - runState;
	// sendValue(UART4,middlePoint);
//	LCD_Num_8x16_X(10,80,tempD + 40,WHITE,BLACK);//��ӡ�õ����м�ֵ
//	LCD_Num_8x16_X(10,96,pointNum,WHITE,BLACK);//��ӡ�õ����м�ֵ
	LCD_Num_8x16_X(50,108,tempD,WHITE,BLACK);//��ӡ�õ����м�ֵ
	

        //pSGPID->Kp = Polynomial2(ABS(tempD), 15, 5, 0.1);
        pSGPID->Kp = (int)Polynomial2(ABS(tempD), 24, 5.5, 0.1);
        
        //24, 5.5, 0.1, // Kp = f( abs( tempD ) )
        //0, 8, 0.2,    // turn = abs( f( abs( tempD ) ) )
        
        int newTurn = (int) ( SIGN(tempD) * Polynomial2(ABS(tempD), 0, 12, -0.2));
        
        turn = MIDDLE_DEGREE + sGPID_realize(0, newTurn , pSGPID);
        //turn = MIDDLE_DEGREE + newTurn;
  
/* 
	if(ABS(tempD) < 5){							//ֱ��
	  	pSGPID->Kp = SG_Pid_Set[0]; // 40
	}else if(ABS(tempD) < 10){					//С��
	  	pSGPID->Kp = SG_Pid_Set[1]; // 60
//		outProMark = FUNC_CLOSE;			//��������ȡ������
	}else if(ABS(tempD) < 15){					//С��
	  	pSGPID->Kp = SG_Pid_Set[2]; // 90
//		outProMark = FUNC_CLOSE;			//��������ȡ������
	}else if(ABS(tempD) < 20){					//����
	  	pSGPID->Kp = SG_Pid_Set[3]; // 110
//		outProMark = FUNC_CLOSE;			//��������ȡ������
	}else{
	  	pSGPID->Kp = SG_Pid_Set[4]; //130
//		outProMark = FUNC_CLOSE;			//��������ȡ������
	}
*/
	
//	if((pointNum >= 1 || ABS(tempD)<8) && outProMark == FUNC_CLOSE){
		//turn = MIDDLE_DEGREE + sGPID_realize(0, tempD * 5 , pSGPID);
//	}else{
//	  	if(outProMark == FUNC_CLOSE){
//			turn = ((-tempD)>0)?(LEFT_MAX):(RIGHT_MAX);	//����
//			outProMark = turn;								//��¼�������򿪱���
//		}else{
//		  	turn = outProMark;			//��������
//		}
//	}
        
        //turn = CONTROLED_TURN(turn);
        turn  = CONTROLED_TURN(turn);
	pointNum = 0;
        
	FTM_PWM_Duty(FTM2, CH0, turn);		//���ת������
	LCD_Num_8x16_X(70,106,turn ,WHITE,BLACK);			//��ӡ���pwmֵ
	return turn;
}


/******************************************************
 * ��������sGPID_init
 * ����  ����ʼ�����PID(�ڲ�����)
 * ����  : sGPIDָ��
 * ���  ��void
 * ע��  ��	��������
*********************************************************/  
void sGPID_init(sGPID *pid){
    pid->SetX=40;     
	pid->ActualX=40;     
	pid->err=0;      
	pid->err_last=0;     
	pid->integral=0;     
	pid->Kp=20;     
	pid->Ki=0;     
	pid->Kd=25; 
//	LCD_Num_8x16_X(100,100,roadMark,WHITE,BLACK);
//	switch(roadMark){
//		case STRAIGHT:		pid->Kp=10;break;
//		case BEND_SMALL:	pid->Kp=12;break;
//		case BEND_BIG:		pid->Kp=13;break;
//		case BEND_MAX:		pid->Kp=14;break;
//	}
}

/******************************************************
 * ��������sGPID_realize
 * ����  ��ʵ�ֶ��PID(�ڲ�����)
 * ����  : Ŀ��Xֵ����ǰXֵ��pidָ��
 * ���  ��PWM��
 * ע��  ��	��������
*********************************************************/  
int sGPID_realize(int X,int toX, sGPID *pid){
  	register int iErr, sG_PWM; 			//��ǰ���
	pid->SetX = toX;
	pid->ActualX = X;
	iErr = pid->ActualX - pid->SetX; 		//��������
	pid->integral += iErr;
	
//	if(iErr-pid->err_last>5){
//		if(iErr-pid->err_last>15){
//			pid->Kd=10;
//		}else{
//			pid->Kd=15;
//		}
//	}else{
//		pid->Kd=5;
//	}
	
	sG_PWM  = pid->Kp * iErr 							//p
			- pid->Ki * pid->integral 					//i
			+ pid->Kd * (pid->err-pid->err_last); 		//d
	pid->err_last = iErr;					//�洢�������´μ���
	
	//����ļ�ֵ����
	if(sG_PWM > LEFT_MAX - MIDDLE_DEGREE)
	  	sG_PWM = LEFT_MAX - MIDDLE_DEGREE;
	else if(sG_PWM < RIGHT_MAX - MIDDLE_DEGREE)
	  	sG_PWM = RIGHT_MAX - MIDDLE_DEGREE;
	
	return(sG_PWM);							//��������ֵ
}

/******************************************************
 * ��������sGTurn
 * ����  ���ֶ�ת�����(�ڲ�����)
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/  
void sGManual(void){
  	int SGturn = steero_middle;
  	LCD_Clear(BLUE);
	LCD_Str_8x16_X(10,10, "SGturn:",RED,BLACK);			
	FTM_PWM_Duty(FTM2, CH0, SGturn);
	
	while(1){
		if(keyOperation == KEY_UP){
		  	keyOperation = KEY_NULL;
			SGturn += 100;
		}else if(keyOperation == KEY_DOWN){
			keyOperation = KEY_NULL;
			SGturn -= 100;
		}
		FTM_PWM_Duty(FTM2, CH0, SGturn);
		LCD_Num_8x16_X(70,10,SGturn,WHITE,BLACK);
		
		if(keyOperation == KEY_LEFT){		//�������
		  	keyOperation = KEY_NULL;
			LCD_Clear(BLUE);
			break;
		}
	}
}

