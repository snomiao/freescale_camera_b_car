#ifndef	_STEERING_GEAR_H_
#define	_STEERING_GEAR_H_

#define FTM_MULTIPLE	100						//PWM�ľ��ȱ���
#define CMD_WARE     	3						//���͵�����ʾ����ָ��


//����Ƕ�������ã��ڶ�������
//#define LEFT_MAX 		22000						//�����ת���PWM
//#define MIDDLE_DEGREE	20000						//����м�PWM
//#define RIGHT_MAX 		18000						//�����ת���PWM

//����Ƕ�������ã���С½���ӣ�
//#define LEFT_MAX 		18000						//�����ת���PWM
//#define MIDDLE_DEGREE	16000						//����м�PWM
//#define RIGHT_MAX 	14000						//�����ת���PWM

typedef struct sG_PID{
	int SetX;									//�����趨ֵ     
	int ActualX;        						//����ʵ��ֵ     
	int err;									//����ƫ��ֵ      
	int err_last;								//������һ��ƫ��ֵ     
	int integral;								//��������ǰ��ƫ��ֵ      
	int Kp,Ki,Kd;								//������������֡�΢��ϵ��
}sGPID;

typedef enum 							//��Ϊ�������ݲ���ʹ��
{
 	RUN_MIDDLE = 0,
	RUN_LEFT,
	RUN_RIGHT
}RUN_STATE;								//���ܿ���

int sGStart(int* Point);		//����ⲿ��������

int   followRoad		(void);										//ʹ��PID�������
void  sGPID_init		(sGPID *pid);										//PID��ʼ��
int   sGPID_realize		(int X,int toX, sGPID *pid);						//PIDʵ��
void  sGManual			(void);												//�ֶ����ƶ��

#endif