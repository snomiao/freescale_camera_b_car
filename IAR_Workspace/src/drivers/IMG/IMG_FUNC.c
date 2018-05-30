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

RoadBoundary roadFB[60] = {{0},{0},0,0};            //边界
int middleBuff[BOTTOM_EDGE] = {0};					//定义一个数组存储所有中点x坐标
volatile int middlePoint = 0;								//根据中线得到的一个偏差点
int pointNum = 0;									//有效点数量
int delaytime = 0;									//延迟时间
extern int slowMark;								//减速记号
extern int runState;								//行驶记号

int carTurn = CAR_RIGHT;


/*********************************************************
 * 函数名：imageProcessing
 * 描述  ：图像处理外部接口（外部调用）
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/
void imageProcessing(void){
  	int imgIfEffect = 0;
	int temp;
  	int y = BOTTOM_EDGE;
  	int i = 0;				//定义y坐标循环变量以及临时循环变量
	RoadBoundary roadB = {{0},{0},0,0};			//定义一个结构体存储边界
	RoadBoundary *pRoadB = &roadB;				//定义一个结构体指针方便函数调用
	
	for(y = BOTTOM_EDGE;y >= TOP_EDGE;y--){
		getRoadBoundary(pRoadB,y);				//获得一行中所有边界
		imgIfEffect = filterRoadBoundary(pRoadB,y);
		if(imgIfEffect == IMG_ERROR){
		  	break;
		}
		pRoadB->xLMark = 0;pRoadB->xRMark = 0;					//初始化标记
	}
	
	if(imgIfEffect != IMG_ERROR){								//如果图像没出错，则进行处理
	  	crossProcess(); // 十字路口处理
		getMiddle();    // 获取平均中线
		//ringProcess();
		//CrossSlowDown();
		//obstacleProcess();
		for(y = BOTTOM_EDGE;y >= TOP_EDGE;y--){
		  	if(middleBuff[BOTTOM_EDGE-y] == OUT_OF_RANGE){
				break;
			}
			for(i = 0;i < roadFB[BOTTOM_EDGE-y].xLMark ;i++){
				img_buff[middleBuff[BOTTOM_EDGE-y]][y] = 3;
				img_buff[roadFB[BOTTOM_EDGE-y].xLBuff[i]][y] = 2;				//打印中心线
				img_buff[roadFB[BOTTOM_EDGE-y].xRBuff[i]][y] = 2;				//打印中心线
			}
		}
		temp = avgMid(middleBuff, BOTTOM_USE_Y, TOP_USE_Y);
		if(temp != -1){
			middlePoint = temp - 40; // 得到一个 -40~40 的中点
		}
	}
	
  	//打印上下边界
	for(i = 0;i < 80; i++){
		img_buff[i][TOP_USE_Y] = 2;
		img_buff[i][BOTTOM_USE_Y] = 2;
	}
	
}
  

/*********************************************************
 * 函数名：getRoadBoundary
 * 描述  ：获得一行中路的边界（内部调用）
 * 输入  : 记录边界数量坐标的结构体指针，行数
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/

#define FOR(VAR, FROM, TO)  for(int VAR = LEFT_EDGE; VAR <= RIGHT_EDGE; VAR++)


void  getRoadBoundary(RoadBoundary *rB,int y){
	u8 xLTimes = 0,xRTimes = 0;			//当发现EDGE_WIDTH个连续色后找出边界
	int xLeft,xRight;					//左右边界临时循环变量
	int temp = 0;						//中途临时使用变量
	
	for(xLeft = LEFT_EDGE;xLeft <= RIGHT_EDGE;xLeft++)
        //FOR(xLeft, LEFT_EDGE, RIGHT_EDGE){			//找出所有左边界
        {
		if(img_buff[xLeft][y] == 1){							//允许大白条中夹杂1到2个小黑点
			if(img_buff[xLeft+1][y] == 1)
				xLTimes = 0;
		}else{
			xLTimes++;
		}
		if(xLTimes == EDGE_WIDTH){								//当白块的宽度大于一个值，则认为最左边那个点为边界
			temp = xLeft-EDGE_WIDTH+1+ROAD_OFFSET;				//得到左边界x坐标
//			img_buff[temp][y] = 2;								
			rB->xLBuff[rB->xLMark] = temp;						//将x坐标存储
			rB->xLMark++;										//左边界数量+1
		}
	}
	
	for(xRight = RIGHT_EDGE;xRight >= LEFT_EDGE;xRight--){		//找出所有右边界
		if(img_buff[xRight][y] == 1){
			xRTimes = 0;
		}else{
			xRTimes++;
		}
		if(xRTimes == EDGE_WIDTH){								//当白块的宽度大于一个值，则认为最左边那个点为边界
			temp = xRight+EDGE_WIDTH-1-ROAD_OFFSET;				//得到右边界x坐标
//			img_buff[temp][y] = 2;
			rB->xRBuff[rB->xRMark] = temp;						//将x坐标存储
			rB->xRMark++;										//右边界数量+1
		}
	}
	img_buff[xLeft][y] = 2;					//打印右边界
	img_buff[xRight][y] = 2;				//打印左边界
	img_buff[40][y] = 4;				//打印左边界
}

/*********************************************************
 * 函数名：filterRoadBoundary
 * 描述  ：找出一行所有边界中最宽的左右边界坐标（内部调用）
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/
int filterRoadBoundary(RoadBoundary *rB,int y){
	int temp = 0;
  
	roadFB[BOTTOM_EDGE - y].xLMark = 0;				//初始化存储的点数量
	roadFB[BOTTOM_EDGE - y].xRMark = 0;				//初始化存储的点数量
	
	roadFB[BOTTOM_EDGE-y].xLBuff[0] = temp;
	roadFB[BOTTOM_EDGE-y].xRBuff[0] = temp;
	
	if(rB->xLMark == 0){							//如果没有点直接结束取点环节
		return 0;
	}
	
	if(y > BOTTOM_EDGE - 3){						//最下面3行特殊处理，进行判断图像是否有效
		findMaxRoadB(rB,y);
		if(y == BOTTOM_EDGE - 2){					//判断当已处理3行后，进行判断图像是否有效
		  	if(ifImgError() == IMG_ERROR){			//图像错误返回
				return IMG_ERROR;
		  	}
		}
	}else{
		findEffectiveRoad(rB,y);
	}
	
	return 0;
}

/*********************************************************
 * 函数名：findMaxRoadB
 * 描述  ：找出一行所有边界中最宽的左右边界坐标（内部调用）
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
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
  
	max = xRMax - xLMax;					//初始化最大宽度
	
	for(i = 0;(i < rB->xLMark)&&(i < rB->xRMark);i++){		//取出一行中白色行最宽的中点
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
 * 函数名：ifImgError
 * 描述  ：找出一行所有边界中最宽的左右边界坐标（内部调用）
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
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
 * 函数名：findEffectiveRoad
 * 描述  ：找出一行所有边界中最宽的左右边界坐标（内部调用）
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/
int findEffectiveRoad(RoadBoundary *rB,int y){
	int i = 0,j = 0,temp1 = 0,temp2 = 0; 
	int LB = 0;
	int RB = 0;
	
  	for(i = 0;i < rB->xLMark;i++){		//根据扫出边界多少个进行多少次循环
	  	LB = rB->xLBuff[i];									//左边界
		RB = rB->xRBuff[rB->xRMark-1-i];					//右边界
	  	
		for(j = 0;(j < roadFB[BOTTOM_EDGE-y-1].xLMark);j ++){			//和上一次得到的边界进行比较
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
 * 函数名：ringProcess
 * 描述  ：十字处理
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
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
 * 函数名：crossProcess
 * 描述  ：十字处理
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/
int crossProcess(void){
//  	int errorY1 = 0,errorY2 = 0;		//保存十字第横边
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
 * 函数名：obstacleProcess
 * 描述  ：十字处理
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/
int obstacleProcess(void){
  	int i = 0;
	double sum = 0;							//把点相加
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
 * 函数名：getMiddle
 * 描述  ：将roadFB中存储的边界取出中点
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/
int getMiddle(void){
  	int y = 0;
	for(y = BOTTOM_EDGE;y >= TOP_EDGE;y--){
	  	if(carTurn == CAR_LEFT){
	  		middleBuff[BOTTOM_EDGE - y] = (roadFB[BOTTOM_EDGE-y].xLBuff[0]+roadFB[BOTTOM_EDGE-y].xRBuff[0])/2;
		}else if(carTurn == CAR_RIGHT){
		  	middleBuff[BOTTOM_EDGE - y] = (roadFB[BOTTOM_EDGE-y].xLBuff[roadFB[BOTTOM_EDGE-y].xLMark-1]+roadFB[BOTTOM_EDGE-y].xRBuff[roadFB[BOTTOM_EDGE-y].xLMark-1])/2;
		}
		if(middleBuff[BOTTOM_EDGE - y] >= RIGHT_EDGE - 7||middleBuff[BOTTOM_EDGE - y] <= LEFT_EDGE + 7){		//当超过边界，则
		  	middleBuff[BOTTOM_EDGE - y] = OUT_OF_RANGE;
		}
	}
//	LCD_Num_8x16_X(100,100,roadFB[30].xLMark,RED,BLUE);
	return 0;
  
}

/******************************************************
 * 函数名：avgMid
 * 描述  ：根据中线算出一个重点值(内部调用)
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/  
int avgMid(int* middle,int bottonY,int topY){
	int i;									//循环的临时变量
	double sum = 0;							//把点相加
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
 * 函数名：ifOutBoundary
 * 描述  ：根据中线算出一个重点值(内部调用)
 * 输入  : void
 * 输出  ：void
 * 注意  ：	（）测试
*********************************************************/  
int ifOutBoundary(int* middle){
	int i = 0;									//循环的临时变量
	double sum = 0;							//把点相加
	double temp = 0;						//临时使用值
	static int priBMid = 40;				//上一个底部三行得到的点
	static int bottomMid = 40;				//底部三行得到的点
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
	
//	LCD_Num_8x16_X(50,100,bottomMid,WHITE,BLACK);//打印得到的中间值
	return 0;
}

/******************************************************
 * 函数名：CrossSlowDown
 * 描述  ：圆环十字减速(内部调用)
 * 输入  : 
 * 输出  ：
 * 注意  ：	（）测试
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
