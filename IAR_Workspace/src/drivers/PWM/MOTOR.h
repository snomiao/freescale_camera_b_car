#ifndef	_MOTOR_H_
#define	_MOTOR_H_

#define SPEED_MAX 40
#define MAX_PWM   100000
#define STOP_PWM  50000
#define MIN_PWM   0

typedef struct motor_PID{
	int SetX;									//定义设定值     
	int ActualX;        						//定义实际值     
	int err;									//定义偏差值      
	int err_next;								//定义上一个偏差值     
	int err_last;								//定义最上前的偏差值      
	int Kp,Ki,Kd;								//定义比例、积分、微分系数
}motorPID;

void motorStart(int turn);
void speedChange(int turn);
void motorPID_init(motorPID *pid);
int  motorPID_realize(int speed,int toSpeed, motorPID *pid);

#endif