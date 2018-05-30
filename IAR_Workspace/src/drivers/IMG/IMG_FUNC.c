#include "common.h"
#include "gpio.h"
#include "SCCB.h"
#include "OV7725.h"
#include "OV7725_REG.h"
#include "BL144002.h"
#include "LCD.h"
#include "CAMERA_FUNC.h"
#include "IMG_FUNC.h"
#include "STEERING_GEAR.h"
#include "MOTOR.h"
#include "MENU.h"

RoadBoundary roadFB[60] = {{0},{0},0,0};            //�߽�
int middleBuff[BOTTOM_EDGE] = {0};					//����һ������洢�����е�x����
volatile int middlePoint = 0;								//�������ߵõ���һ��ƫ���
int pointNum = 0;									//��Ч������
int delaytime = 0;									//�ӳ�ʱ��
extern int slowMark;								//���ټǺ�
extern int runState;								//��ʻ�Ǻ�

int carTurn = CAR_RIGHT;


/*********************************************************
 * ��������imageProcessing
 * ����  ��ͼ�����ⲿ�ӿڣ��ⲿ���ã�
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/
void imageProcessing(void){
  	int imgIfEffect = 0;
	int temp;
  	int y = BOTTOM_EDGE;
  	int i = 0;				//����y����ѭ�������Լ���ʱѭ������
	RoadBoundary roadB = {{0},{0},0,0};			//����һ���ṹ��洢�߽�
	RoadBoundary *pRoadB = &roadB;				//����һ���ṹ��ָ�뷽�㺯������
	
	for(y = BOTTOM_EDGE;y >= TOP_EDGE;y--){
		getRoadBoundary(pRoadB,y);				//���һ�������б߽�
		imgIfEffect = filterRoadBoundary(pRoadB,y);
		if(imgIfEffect == IMG_ERROR){
		  	break;
		}
		pRoadB->xLMark = 0;pRoadB->xRMark = 0;					//��ʼ�����
	}
	
	if(imgIfEffect != IMG_ERROR){								//���ͼ��û��������д���
	  	crossProcess(); // ʮ��·�ڴ���
		getMiddle();    // ��ȡƽ������
		//ringProcess();
		//CrossSlowDown();
		//obstacleProcess();
		for(y = BOTTOM_EDGE;y >= TOP_EDGE;y--){
		  	if(middleBuff[BOTTOM_EDGE-y] == OUT_OF_RANGE){
				break;
			}
			for(i = 0;i < roadFB[BOTTOM_EDGE-y].xLMark ;i++){
				img_buff[middleBuff[BOTTOM_EDGE-y]][y] = 3;
				img_buff[roadFB[BOTTOM_EDGE-y].xLBuff[i]][y] = 2;				//��ӡ������
				img_buff[roadFB[BOTTOM_EDGE-y].xRBuff[i]][y] = 2;				//��ӡ������
			}
		}
		temp = avgMid(middleBuff, BOTTOM_USE_Y, TOP_USE_Y);
		if(temp != -1){
			middlePoint = temp - 40; // �õ�һ�� -40~40 ���е�
		}
	}
	
  	//��ӡ���±߽�
	for(i = 0;i < 80; i++){
		img_buff[i][TOP_USE_Y] = 2;
		img_buff[i][BOTTOM_USE_Y] = 2;
	}
	
}
  

/*********************************************************
 * ��������getRoadBoundary
 * ����  �����һ����·�ı߽磨�ڲ����ã�
 * ����  : ��¼�߽���������Ľṹ��ָ�룬����
 * ���  ��void
 * ע��  ��	��������
*********************************************************/

#define FOR(VAR, FROM, TO)  for(int VAR = LEFT_EDGE; VAR <= RIGHT_EDGE; VAR++)


void  getRoadBoundary(RoadBoundary *rB,int y){
	u8 xLTimes = 0,xRTimes = 0;			//������EDGE_WIDTH������ɫ���ҳ��߽�
	int xLeft,xRight;					//���ұ߽���ʱѭ������
	int temp = 0;						//��;��ʱʹ�ñ���
	
	for(xLeft = LEFT_EDGE;xLeft <= RIGHT_EDGE;xLeft++)
        //FOR(xLeft, LEFT_EDGE, RIGHT_EDGE){			//�ҳ�������߽�
        {
		if(img_buff[xLeft][y] == 1){							//���������м���1��2��С�ڵ�
			if(img_buff[xLeft+1][y] == 1)
				xLTimes = 0;
		}else{
			xLTimes++;
		}
		if(xLTimes == EDGE_WIDTH){								//���׿�Ŀ�ȴ���һ��ֵ������Ϊ������Ǹ���Ϊ�߽�
			temp = xLeft-EDGE_WIDTH+1+ROAD_OFFSET;				//�õ���߽�x����
//			img_buff[temp][y] = 2;								
			rB->xLBuff[rB->xLMark] = temp;						//��x����洢
			rB->xLMark++;										//��߽�����+1
		}
	}
	
	for(xRight = RIGHT_EDGE;xRight >= LEFT_EDGE;xRight--){		//�ҳ������ұ߽�
		if(img_buff[xRight][y] == 1){
			xRTimes = 0;
		}else{
			xRTimes++;
		}
		if(xRTimes == EDGE_WIDTH){								//���׿�Ŀ�ȴ���һ��ֵ������Ϊ������Ǹ���Ϊ�߽�
			temp = xRight+EDGE_WIDTH-1-ROAD_OFFSET;				//�õ��ұ߽�x����
//			img_buff[temp][y] = 2;
			rB->xRBuff[rB->xRMark] = temp;						//��x����洢
			rB->xRMark++;										//�ұ߽�����+1
		}
	}
	img_buff[xLeft][y] = 2;					//��ӡ�ұ߽�
	img_buff[xRight][y] = 2;				//��ӡ��߽�
	img_buff[40][y] = 4;				//��ӡ��߽�
}

/*********************************************************
 * ��������filterRoadBoundary
 * ����  ���ҳ�һ�����б߽����������ұ߽����꣨�ڲ����ã�
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/
int filterRoadBoundary(RoadBoundary *rB,int y){
	int temp = 0;
  
	roadFB[BOTTOM_EDGE - y].xLMark = 0;				//��ʼ���洢�ĵ�����
	roadFB[BOTTOM_EDGE - y].xRMark = 0;				//��ʼ���洢�ĵ�����
	
	roadFB[BOTTOM_EDGE-y].xLBuff[0] = temp;
	roadFB[BOTTOM_EDGE-y].xRBuff[0] = temp;
	
	if(rB->xLMark == 0){							//���û�е�ֱ�ӽ���ȡ�㻷��
		return 0;
	}
	
	if(y > BOTTOM_EDGE - 3){						//������3�����⴦�������ж�ͼ���Ƿ���Ч
		findMaxRoadB(rB,y);
		if(y == BOTTOM_EDGE - 2){					//�жϵ��Ѵ���3�к󣬽����ж�ͼ���Ƿ���Ч
		  	if(ifImgError() == IMG_ERROR){			//ͼ����󷵻�
				return IMG_ERROR;
		  	}
		}
	}else{
		findEffectiveRoad(rB,y);
	}
	
	return 0;
}

/*********************************************************
 * ��������findMaxRoadB
 * ����  ���ҳ�һ�����б߽����������ұ߽����꣨�ڲ����ã�
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/
int findMaxRoadB(RoadBoundary *rB,int y){
  	int max;
	int i,temp;
	int xLMax,xRMax;
	
	xLMax = rB->xLBuff[0];
	xRMax = rB->xRBuff[rB->xLMark - 1];
	roadFB[BOTTOM_EDGE-y].xLBuff[0] = xLMax;
	roadFB[BOTTOM_EDGE-y].xLMark ++;
	roadFB[BOTTOM_EDGE-y].xRBuff[0] = xRMax;
	roadFB[BOTTOM_EDGE-y].xRMark ++;
  
	max = xRMax - xLMax;					//��ʼ�������
	
	for(i = 0;(i < rB->xLMark)&&(i < rB->xRMark);i++){		//ȡ��һ���а�ɫ�������е�
	  	temp = rB->xRBuff[rB->xRMark-1-i] - rB->xLBuff[i];
		if(temp>max){
		  	max =temp;
			xLMax = rB->xLBuff[i];
			xRMax = rB->xRBuff[rB->xRMark-1-i];
			roadFB[BOTTOM_EDGE-y].xLBuff[0] = xLMax;
			roadFB[BOTTOM_EDGE-y].xRBuff[0] = xRMax;
		}
	}
	return 0;
}

/*********************************************************
 * ��������ifImgError
 * ����  ���ҳ�һ�����б߽����������ұ߽����꣨�ڲ����ã�
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/
int ifImgError(void){
  	int i = 0;
	
  	int temp1 = roadFB[0].xLMark + roadFB[1].xLMark + roadFB[2].xLMark;
  	int temp2 = roadFB[0].xRMark + roadFB[1].xRMark + roadFB[2].xRMark;

//	if(temp1+temp2 != 6){
//	  	return IMG_ERROR;
//	}
	
	for(i = 1;i < 3;i++){
		temp1 = ABS(roadFB[i].xLBuff[0] - roadFB[i-1].xLBuff[0]);
		temp2 = ABS(roadFB[i].xRBuff[0] - roadFB[i-1].xRBuff[0]);
		if(temp1 > 10 || temp2 > 10){	
		  	return IMG_ERROR;
		}
	}
	
	return 0;
}

/*********************************************************
 * ��������findEffectiveRoad
 * ����  ���ҳ�һ�����б߽����������ұ߽����꣨�ڲ����ã�
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/
int findEffectiveRoad(RoadBoundary *rB,int y){
	int i = 0,j = 0,temp1 = 0,temp2 = 0; 
	int LB = 0;
	int RB = 0;
	
  	for(i = 0;i < rB->xLMark;i++){		//����ɨ���߽���ٸ����ж��ٴ�ѭ��
	  	LB = rB->xLBuff[i];									//��߽�
		RB = rB->xRBuff[rB->xRMark-1-i];					//�ұ߽�
	  	
		for(j = 0;(j < roadFB[BOTTOM_EDGE-y-1].xLMark);j ++){			//����һ�εõ��ı߽���бȽ�
			temp1 = ABS(LB - roadFB[BOTTOM_EDGE-y-1].xLBuff[j]);
			temp2 = ABS(RB - roadFB[BOTTOM_EDGE-y-1].xRBuff[j]);
			if(temp1 < BOUNDARY_ERROR || temp2 < BOUNDARY_ERROR){
				roadFB[BOTTOM_EDGE-y].xLBuff[roadFB[BOTTOM_EDGE-y].xLMark] = LB;
				roadFB[BOTTOM_EDGE-y].xRBuff[roadFB[BOTTOM_EDGE-y].xRMark] = RB;
				roadFB[BOTTOM_EDGE-y].xLMark++;
				roadFB[BOTTOM_EDGE-y].xRMark++;
				break;
//			}else if(RB-LB > roadFB[BOTTOM_EDGE-y-1].xRBuff[0] - roadFB[BOTTOM_EDGE-y-1].xLBuff[0]+2){
//				roadFB[BOTTOM_EDGE-y].xLBuff[roadFB[BOTTOM_EDGE-y].xLMark] = LB;
//				roadFB[BOTTOM_EDGE-y].xRBuff[roadFB[BOTTOM_EDGE-y].xRMark] = RB;
//				roadFB[BOTTOM_EDGE-y].xLMark++;
//				roadFB[BOTTOM_EDGE-y].xRMark++;
//				break;
			}else if(roadFB[BOTTOM_EDGE-y-1].xLBuff[0] < LEFT_EDGE+3 && roadFB[BOTTOM_EDGE-y-1].xRBuff[0] < RIGHT_EDGE-3){
			  	roadFB[BOTTOM_EDGE-y].xLBuff[roadFB[BOTTOM_EDGE-y].xLMark] = LB;
				roadFB[BOTTOM_EDGE-y].xRBuff[roadFB[BOTTOM_EDGE-y].xRMark] = RB;
				roadFB[BOTTOM_EDGE-y].xLMark++;
				roadFB[BOTTOM_EDGE-y].xRMark++;
				break;
			}
		}
	}
	
	return 0;
}

/*********************************************************
 * ��������ringProcess
 * ����  ��ʮ�ִ���
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/
int ringProcess(void){
/*
  int i = 0;
	int whiteY1 = 0,whiteY2 = 0;
	int temp;
	
	for(i = BOTTOM_USE_Y; i < TOP_USE_Y;i ++){
	  	temp = roadFB[i].xRBuff[0]-roadFB[i].xLBuff[0];
		if(whiteY1 == 0 && temp >= 70){
		  	whiteY1 = i;
//			LCD_Num_8x16_X(70,80,whiteY1,RED,BLUE);
		}else if(whiteY2 == 0 && (roadFB[i-1].xRBuff[0]-roadFB[i-1].xLBuff[0])>70 && temp < 60){
		  	whiteY2 = i;
//			LCD_Num_8x16_X(70,96,whiteY2,RED,BLUE);
		}
	}
	
//	LCD_Num_8x16_X(70,102,roadFB[whiteY2].xLBuff[1] - roadFB[whiteY2].xRBuff[0],RED,BLUE);
	if((whiteY2 - whiteY1) > 11 && roadFB[whiteY2].xLMark > 1 && (roadFB[whiteY2].xLBuff[1] - roadFB[whiteY2].xRBuff[0])>15){
	  	for(i = whiteY1 ; i < TOP_USE_Y; i++){
		  	if(carTurn == CAR_LEFT){
				middleBuff[i] = 5;
			}else if(carTurn == CAR_RIGHT){
			  	middleBuff[i] = 75;
			}
	  	}
	}
*/  
  	return 0;

}

/*********************************************************
 * ��������crossProcess
 * ����  ��ʮ�ִ���
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/
int crossProcess(void){
//  	int errorY1 = 0,errorY2 = 0;		//����ʮ�ֵں��
//	double k1 = 0,k2 = 0;
//	double temp = 0;
//	int i = 0,j = 0;
//	int xError = 0;
//	
//  	for(i = 1;i < 57 && roadFB[i].xLMark == 1; i++){
//	  	xError = roadFB[i-1].xLBuff[0] - roadFB[i].xLBuff[0];
//		if(xError >= 1 && (roadFB[i+4].xRBuff[0] - roadFB[i+4].xLBuff[0]) >= 75 
//		   && (roadFB[53].xRBuff[0] - roadFB[54].xLBuff[0]) <= 75){
//		 	errorY1 = i;
//			k1 = (double)(i-1)/(roadFB[i-1].xLBuff[0] - roadFB[0].xLBuff[0]+3);
//			k2 = (double)(i-1)/(roadFB[i-1].xLBuff[0] - roadFB[0].xLBuff[0]-1);
//		}
//		if(errorY1 != 0){
//		  	temp = (double)(i)/(roadFB[i].xLBuff[0] - roadFB[0].xLBuff[0]);
//			if(temp >= k1 && temp <= k2){
//			  	errorY2 = i;
//				LCD_Num_8x16_X(10,80,errorY1,RED,BLUE);
//				LCD_Num_8x16_X(10,100,errorY2,RED,BLUE);
//				temp = (double)(errorY2 - errorY1 + 1)/(roadFB[errorY2].xLBuff[0] - roadFB[errorY1 - 1].xLBuff[0]);
//				for(j = errorY1;j < errorY2;j++){
//				  	roadFB[j].xLBuff[0] = (int)(roadFB[errorY1 - 1].xLBuff[0] + (double)ABS((j - errorY1 + 1)/temp));
//				}
//			}
//		}
//	}
//	
//	for(i = 1;i < 57 && roadFB[i].xRMark == 1; i++){
//	  	xError = roadFB[i].xRBuff[0] - roadFB[i-1].xLBuff[0];
//		if(xError >= 1 && (roadFB[i+4].xRBuff[0] - roadFB[i+4].xLBuff[0]) >= 75 
//		   && (roadFB[53].xRBuff[0] - roadFB[54].xLBuff[0]) <= 7){
//		 	errorY1 = i;
//			k1 = (double)(i-1)/(roadFB[0].xRBuff[0] - roadFB[i-1].xRBuff[0]+3);
//			k2 = (double)(i-1)/(roadFB[0].xRBuff[0] - roadFB[i-1].xRBuff[0]-1);
//		}
//		if(errorY1 != 0){
//		  	temp = (double)(i)/(roadFB[0].xRBuff[0] - roadFB[i].xRBuff[0]);
//			if(temp >= k1 && temp <= k2){
//			  	errorY2 = i;
//				LCD_Num_8x16_X(30,80,errorY1,RED,BLUE);
//				LCD_Num_8x16_X(30,100,errorY2,RED,BLUE);
//				temp = (double)(errorY2 - errorY1 + 1)/(roadFB[errorY1 - 1].xRBuff[0] - roadFB[errorY2].xRBuff[0]);
//				for(j = errorY1;j < errorY2;j++){
//				  	roadFB[j].xRBuff[0] = (int)(roadFB[errorY1 - 1].xRBuff[0] - (double)ABS((j - errorY1 + 1)/temp));
//				}
//			}
//		}
//	}
//  
	return 0;
}

/*********************************************************
 * ��������obstacleProcess
 * ����  ��ʮ�ִ���
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/
int obstacleProcess(void){
  	int i = 0;
	double sum = 0;							//�ѵ����
	double temp = 0;
	int turnmMark = 0;
	int tempY1 = 0,tempY2 = 0;
	int leftError = 0 ,rightError = 0;
	
  	for(i = BOTTOM_USE_Y + 1;i < TOP_USE_Y;i++){
	  	leftError = roadFB[i].xLBuff[0] - roadFB[i-1].xLBuff[0];
		rightError = roadFB[i].xRBuff[0] - roadFB[i-1].xRBuff[0];
		if(turnmMark == 0 && ABS(leftError) < 3 && -rightError > 3){
		  	if(tempY1 == 0){
				tempY1 = i;
				turnmMark = CAR_LEFT;
		  	}
		}else if(turnmMark == 0 && leftError > 3 &&  ABS(rightError) < 3){
		  	if(tempY1 == 0){
				tempY1 = i;
				turnmMark = CAR_RIGHT;
		  	}
		}
		if(turnmMark == CAR_LEFT){
		  	if(ABS(leftError) < 3 && rightError > 3){
				if(tempY1 != 0){
					tempY2 = i;
					turnmMark = LEFT_SG_MAX;
		  		}
		  	}
			if(ABS(leftError) > 3){
			  	turnmMark = 0;
			}
		}else if(turnmMark == CAR_RIGHT){
		  	if(-leftError > 3 && ABS(rightError) < 3){
				if(tempY1 != 0){
					tempY2 = i;
					turnmMark = RIGHT_SG_MAX;
				}
		  	}
			if(ABS(rightError) > 3){
			  	turnmMark = 0;
			}
		}
		
		if(tempY1 == 0){
		  	sum += middleBuff[i];
			temp ++;
		}
	}
	
	if(tempY1 < 40 && tempY2 - tempY1 > 7 && ABS((int)(sum/temp)-40) < 5){
//		LCD_Num_8x16_X(70,70,tempY1,WHITE,BLACK);
//		LCD_Num_8x16_X(70,86,tempY2,WHITE,BLACK);
//		LCD_Num_8x16_X(70,102,ABS((int)(sum/temp)-40),WHITE,BLACK);
	  	if(turnmMark == LEFT_SG_MAX){
		  	delaytime = 0;
			runState = 49;
		}else if(turnmMark == RIGHT_SG_MAX){
		  	delaytime = 0;
		  	runState = 31;
		}
	}
	return 0;
}


/*********************************************************
 * ��������getMiddle
 * ����  ����roadFB�д洢�ı߽�ȡ���е�
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/
int getMiddle(void){
  	int y = 0;
	for(y = BOTTOM_EDGE;y >= TOP_EDGE;y--){
	  	if(carTurn == CAR_LEFT){
	  		middleBuff[BOTTOM_EDGE - y] = (roadFB[BOTTOM_EDGE-y].xLBuff[0]+roadFB[BOTTOM_EDGE-y].xRBuff[0])/2;
		}else if(carTurn == CAR_RIGHT){
		  	middleBuff[BOTTOM_EDGE - y] = (roadFB[BOTTOM_EDGE-y].xLBuff[roadFB[BOTTOM_EDGE-y].xLMark-1]+roadFB[BOTTOM_EDGE-y].xRBuff[roadFB[BOTTOM_EDGE-y].xLMark-1])/2;
		}
		if(middleBuff[BOTTOM_EDGE - y] >= RIGHT_EDGE - 7||middleBuff[BOTTOM_EDGE - y] <= LEFT_EDGE + 7){		//�������߽磬��
		  	middleBuff[BOTTOM_EDGE - y] = OUT_OF_RANGE;
		}
	}
//	LCD_Num_8x16_X(100,100,roadFB[30].xLMark,RED,BLUE);
	return 0;
  
}

/******************************************************
 * ��������avgMid
 * ����  �������������һ���ص�ֵ(�ڲ�����)
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/  
int avgMid(int* middle,int bottonY,int topY){
	int i;									//ѭ������ʱ����
	double sum = 0;							//�ѵ����
	double temp = 0;
	
	for(i = bottonY;i < topY;i++){
	  	if(middle[i] == OUT_OF_RANGE){
			break;
	  	}
	  	if(middle[i]>= 2 && middle[i]<= 77){	
			if(i > 10 && i < 35 && roadFB[i].xLBuff[0] < LEFT_EDGE+3 && middle[i-1] < LEFT_EDGE + 15 && middle[i] - middle[i-1] > 0){
//			  	LCD_Num_8x16_X(70,96,i,WHITE,BLACK);
				break;
			}else if(i > 10 && i < 35 && roadFB[i].xRBuff[0] > RIGHT_EDGE-3 && middle[i-1] > RIGHT_EDGE - 15 && middle[i] - middle[i-1] < 0){
//				LCD_Num_8x16_X(70,96,i,WHITE,BLACK);
				break;
			}
			sum += (double)middle[i]* (1000+11*i)/1000;
			temp += (double)(1000+11*i)/1000;
			pointNum++;
		}
	}
	
	
	
//	if(temp == 0 || (ifOutBoundary(middleBuff)==-1)){
	if(temp == 0){
	  	return -1;
	}else{
		return (int)(sum/temp);
	}
}

/******************************************************
 * ��������ifOutBoundary
 * ����  �������������һ���ص�ֵ(�ڲ�����)
 * ����  : void
 * ���  ��void
 * ע��  ��	��������
*********************************************************/  
int ifOutBoundary(int* middle){
	int i = 0;									//ѭ������ʱ����
	double sum = 0;							//�ѵ����
	double temp = 0;						//��ʱʹ��ֵ
	static int priBMid = 40;				//��һ���ײ����еõ��ĵ�
	static int bottomMid = 40;				//�ײ����еõ��ĵ�
	static int boundaryMark = FUNC_CLOSE;
	
	for(i = 0;i < 3;i++){
	  	if(middle[i] == OUT_OF_RANGE){
			break;
	  	}
	  	if(middle[i]>= LEFT_EDGE && middle[i]<= RIGHT_EDGE){
			sum += middle[i];
			temp ++;
		}
	}
	
	priBMid = bottomMid;
	if(temp == 0){
	  	bottomMid = 40;
	}else{
		bottomMid = (int)(sum/temp);
	}
//	LCD_Num_8x16_X(50,78,priBMid,WHITE,BLACK);
//	LCD_Num_8x16_X(50,94,bottomMid,WHITE,BLACK);
//	LCD_Num_8x16_X(50,110,boundaryMark,WHITE,BLACK);
	
	if(boundaryMark == FUNC_CLOSE){
		if(priBMid < LEFT_EDGE+10 && (bottomMid < LEFT_EDGE + 8 || bottomMid > LEFT_EDGE+20)){
			boundaryMark = LEFT_SG_MAX;
			return -1;
		}else if(priBMid > RIGHT_EDGE-10 && (bottomMid > RIGHT_EDGE - 8|| bottomMid < RIGHT_EDGE-20)){
			boundaryMark = RIGHT_SG_MAX;
			return -1;
		}
	}else{
	  	if(bottomMid < LEFT_EDGE+10 && boundaryMark == LEFT_SG_MAX){
			boundaryMark = FUNC_CLOSE;
		}else if(bottomMid > RIGHT_EDGE - 10 && boundaryMark == RIGHT_SG_MAX){
		  	boundaryMark = FUNC_CLOSE;
		}
		if(boundaryMark != FUNC_CLOSE){
		  	return -1;
		}
	}
	
//	LCD_Num_8x16_X(50,100,bottomMid,WHITE,BLACK);//��ӡ�õ����м�ֵ
	return 0;
}

/******************************************************
 * ��������CrossSlowDown
 * ����  ��Բ��ʮ�ּ���(�ڲ�����)
 * ����  : 
 * ���  ��
 * ע��  ��	��������
*********************************************************/  
int CrossSlowDown(void){
  	int rWidth[5] = {0};
	int i = 0;
	int slowNum = 0;
	
	for(i = 25;i < 30;i++){
	  	rWidth[i-25] = roadFB[i].xRBuff[0] - roadFB[i].xLBuff[0];
		if(rWidth[i-25] > 70){
		  	slowNum ++;
		}
	}
//	LCD_Num_8x16_X(70,80,slowNum,WHITE,BLACK);
	if(slowNum >= 5){
	  	slowMark = FUNC_OPEN;
	}else{
	  	slowMark = FUNC_CLOSE;
	}
	return 0;
}
