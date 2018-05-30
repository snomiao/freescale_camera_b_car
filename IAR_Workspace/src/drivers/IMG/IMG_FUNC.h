#ifndef _IMG_FUNC_H_
#define _IMG_FUNC_H_

//图形处理一行中最多有几行路
//#define xRoadNum      1

//图形处理上下左右边界
#define LEFT_EDGE               1               
#define RIGHT_EDGE              78
#define TOP_EDGE                1
#define BOTTOM_EDGE             57

#define START_HEIGHT                    4                                //删选点起点位
#define EDGE_WIDTH              10              //探测边界时精度
#define ROAD_OFFSET             1               //边线往内偏置距离
#define MID_ACCURACY            5               //中线前后一组精度
#define MID_FILTER_ACCURACY     3               //中线左右过滤精度
#define OFFSET_MIDDLE           14              //当中线与边界太近则偏移边界
#define BOUNDARY_ERROR          10              //边界连续性x误差

#define TOP_USE_Y               55              //计算中间点最高Y值
#define BOTTOM_USE_Y            5               //计算中间点最小Y值

extern u8 img_buff[80][60];                     //扩大作用域

typedef struct rB{
    int xLBuff[10];
    int xRBuff[10];                     //存储 x 边界坐标
    int xLMark;
    int xRMark;                         //x 边界扫出点的个数
}RoadBoundary;                          //记录处理标记

typedef enum                            //作为按键传递参数使用
{
    OUT_OF_RANGE = 0,                   //出边界
    ALL_WHITE,                          //全白
    NO_EXIST,                           //不存在
    IMG_ERROR,
    LEFT_SG_MAX = 11,
    RIGHT_SG_MAX = 10,
    CAR_LEFT,
    CAR_RIGHT
}IMG_INFORM;                            //图像信息枚举


void  imageProcessing       (void);                                 //图像处理的主函数
void  getRoadBoundary       (RoadBoundary *rB,int y);               //获得一行中所有边界
int   filterRoadBoundary    (RoadBoundary *rB,int y);               //过滤出所需N个边界
int   findMaxRoadB          (RoadBoundary *rB,int y);               //过滤出最大的边界
int   ifImgError            (void);                                 //判断图像是否正确
int   findEffectiveRoad     (RoadBoundary *rB,int y);               //找到有效边界，根据边界连续性
int   ringProcess           (void);                                 //圆环处理
int   crossProcess          (void);                                 //十字处理
int   obstacleProcess       (void);
int   getMiddle             (void);                                     //将roadFB边界转换成中点
int   avgMid                (int* middle,int bottonY,int topY);         //求平均中点
int   ifOutBoundary         (int* middle);
int   CrossSlowDown         (void);                                     //经过十字减速    

#endif