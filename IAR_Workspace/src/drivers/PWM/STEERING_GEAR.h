#ifndef	_STEERING_GEAR_H_
#define	_STEERING_GEAR_H_

#define FTM_MULTIPLE	100						//PWM的精度倍数
#define CMD_WARE     	3						//发送到虚拟示波器指令


//舵机角度最大设置（第二辆车）
//#define LEFT_MAX 		22000						//舵机左转最大PWM
//#define MIDDLE_DEGREE	20000						//舵机中间PWM
//#define RIGHT_MAX 		18000						//舵机右转最大PWM

//舵机角度最大设置（黄小陆车子）
//#define LEFT_MAX 		18000						//舵机左转最大PWM
//#define MIDDLE_DEGREE	16000						//舵机中间PWM
//#define RIGHT_MAX 	14000						//舵机右转最大PWM

typedef struct sG_PID{
	int SetX;									//定义设定值     
	int ActualX;        						//定义实际值     
	int err;									//定义偏差值      
	int err_last;								//定义上一个偏差值     
	int integral;								//定义最上前的偏差值      
	int Kp,Ki,Kd;								//定义比例、积分、微分系数
}sGPID;

typedef enum 							//作为按键传递参数使用
{
 	RUN_MIDDLE = 0,
	RUN_LEFT,
	RUN_RIGHT
}RUN_STATE;								//功能开关

int sGStart(int* Point);		//舵机外部调用启动

int   followRoad		(void);										//使用PID调整舵机
void  sGPID_init		(sGPID *pid);										//PID初始化
int   sGPID_realize		(int X,int toX, sGPID *pid);						//PID实现
void  sGManual			(void);												//手动控制舵机

#endif