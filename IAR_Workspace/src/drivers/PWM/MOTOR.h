#ifndef	_MOTOR_H_
#define	_MOTOR_H_

#define SPEED_MAX 40
#define MAX_PWM   100000
#define STOP_PWM  50000
#define MIN_PWM   0

typedef struct motor_PID{
	int SetX;									//�����趨ֵ     
	int ActualX;        						//����ʵ��ֵ     
	int err;									//����ƫ��ֵ      
	int err_next;								//������һ��ƫ��ֵ     
	int err_last;								//��������ǰ��ƫ��ֵ      
	int Kp,Ki,Kd;								//������������֡�΢��ϵ��
}motorPID;

void motorStart(int turn);
void speedChange(int turn);
void motorPID_init(motorPID *pid);
int  motorPID_realize(int speed,int toSpeed, motorPID *pid);

#endif