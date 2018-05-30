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

//舵机角度最大设置
volatile u16 LEFT_MAX 	       = 26500;						//舵机左转最大PWM
volatile u16 MIDDLE_DEGREE    = 19500;						//舵机中间PWM
volatile u16 RIGHT_MAX        = 12600;						//舵机右转最大PWM


extern sGPID*  pSGPID;			//定义一个PID结构体指针
extern int keyOperation;		//按键变量
extern int SG_Pid_Set[6];		//舵机5个pid
extern int middlePoint;								//根据中线得到的一个偏差点
extern int pointNum;					
				//有效点数量
int outProMark = FUNC_CLOSE;	//打死开关
int runState = 40; // ??

extern float steero_middle;
/******************************************************
 * 函数名：sGStart
 * 描述  ：调用舵机内部函数(外部调用)
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/  
int sGStart(int* Point){	
	LCD_Num_8x16_X(70,88,pSGPID->Kp ,WHITE,BLACK);		//打印SGp值	
	return followRoad();
}

/******************************************************
 * 函数名：followRoad
 * 描述  ：寻路函数(内部调用)
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/  

#define Polynomial2(x, a, b, c) (a + b * x + c * x * x)

int followRoad(void){
	int tempD = 0;					//用于得到中心线浓缩的点
	int turn = MIDDLE_DEGREE;				//舵机PWM占空比
	
	tempD = middlePoint - runState;
	// sendValue(UART4,middlePoint);
//	LCD_Num_8x16_X(10,80,tempD + 40,WHITE,BLACK);//打印得到的中间值
//	LCD_Num_8x16_X(10,96,pointNum,WHITE,BLACK);//打印得到的中间值
	LCD_Num_8x16_X(50,108,tempD,WHITE,BLACK);//打印得到的中间值
	

        //pSGPID->Kp = Polynomial2(ABS(tempD), 15, 5, 0.1);
        pSGPID->Kp = (int)Polynomial2(ABS(tempD), 24, 5.5, 0.1);
        
        //24, 5.5, 0.1, // Kp = f( abs( tempD ) )
        //0, 8, 0.2,    // turn = abs( f( abs( tempD ) ) )
        
        int newTurn = (int) ( SIGN(tempD) * Polynomial2(ABS(tempD), 0, 12, -0.2));
        
        turn = MIDDLE_DEGREE + sGPID_realize(0, newTurn , pSGPID);
        //turn = MIDDLE_DEGREE + newTurn;
  
/* 
	if(ABS(tempD) < 5){							//直线
	  	pSGPID->Kp = SG_Pid_Set[0]; // 40
	}else if(ABS(tempD) < 10){					//小弯
	  	pSGPID->Kp = SG_Pid_Set[1]; // 60
//		outProMark = FUNC_CLOSE;			//满足条件取消保护
	}else if(ABS(tempD) < 15){					//小弯
	  	pSGPID->Kp = SG_Pid_Set[2]; // 90
//		outProMark = FUNC_CLOSE;			//满足条件取消保护
	}else if(ABS(tempD) < 20){					//大弯
	  	pSGPID->Kp = SG_Pid_Set[3]; // 110
//		outProMark = FUNC_CLOSE;			//满足条件取消保护
	}else{
	  	pSGPID->Kp = SG_Pid_Set[4]; //130
//		outProMark = FUNC_CLOSE;			//满足条件取消保护
	}
*/
	
//	if((pointNum >= 1 || ABS(tempD)<8) && outProMark == FUNC_CLOSE){
		//turn = MIDDLE_DEGREE + sGPID_realize(0, tempD * 5 , pSGPID);
//	}else{
//	  	if(outProMark == FUNC_CLOSE){
//			turn = ((-tempD)>0)?(LEFT_MAX):(RIGHT_MAX);	//打死
//			outProMark = turn;								//记录打死，打开保护
//		}else{
//		  	turn = outProMark;			//保护程序
//		}
//	}
        
        //turn = CONTROLED_TURN(turn);
        turn  = CONTROLED_TURN(turn);
	pointNum = 0;
        
	FTM_PWM_Duty(FTM2, CH0, turn);		//舵机转动命令
	LCD_Num_8x16_X(70,106,turn ,WHITE,BLACK);			//打印舵机pwm值
	return turn;
}


/******************************************************
 * 函数名：sGPID_init
 * 描述  ：初始化舵机PID(内部调用)
 * 输入  : sGPID指针
 * 输出  ：void
 * 注意  ：	（）测试
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
 * 函数名：sGPID_realize
 * 描述  ：实现舵机PID(内部调用)
 * 输入  : 目标X值，当前X值，pid指针
 * 输出  ：PWM波
 * 注意  ：	（）测试
*********************************************************/  
int sGPID_realize(int X,int toX, sGPID *pid){
  	register int iErr, sG_PWM; 			//当前误差
	pid->SetX = toX;
	pid->ActualX = X;
	iErr = pid->ActualX - pid->SetX; 		//增量计算
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
	pid->err_last = iErr;					//存储误差，用于下次计算
	
	//舵机的极值保护
	if(sG_PWM > LEFT_MAX - MIDDLE_DEGREE)
	  	sG_PWM = LEFT_MAX - MIDDLE_DEGREE;
	else if(sG_PWM < RIGHT_MAX - MIDDLE_DEGREE)
	  	sG_PWM = RIGHT_MAX - MIDDLE_DEGREE;
	
	return(sG_PWM);							//返回增量值
}

/******************************************************
 * 函数名：sGTurn
 * 描述  ：手动转动舵机(内部调用)
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
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
		
		if(keyOperation == KEY_LEFT){		//左键返回
		  	keyOperation = KEY_NULL;
			LCD_Clear(BLUE);
			break;
		}
	}
}

