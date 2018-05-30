#ifndef _IMG_FUNC_H_
#define _IMG_FUNC_H_

//ͼ�δ���һ��������м���·
//#define xRoadNum      1

//ͼ�δ����������ұ߽�
#define LEFT_EDGE               1               
#define RIGHT_EDGE              78
#define TOP_EDGE                1
#define BOTTOM_EDGE             57

#define START_HEIGHT                    4                                //ɾѡ�����λ
#define EDGE_WIDTH              10              //̽��߽�ʱ����
#define ROAD_OFFSET             1               //��������ƫ�þ���
#define MID_ACCURACY            5               //����ǰ��һ�龫��
#define MID_FILTER_ACCURACY     3               //�������ҹ��˾���
#define OFFSET_MIDDLE           14              //��������߽�̫����ƫ�Ʊ߽�
#define BOUNDARY_ERROR          10              //�߽�������x���

#define TOP_USE_Y               55              //�����м�����Yֵ
#define BOTTOM_USE_Y            5               //�����м����СYֵ

extern u8 img_buff[80][60];                     //����������

typedef struct rB{
    int xLBuff[10];
    int xRBuff[10];                     //�洢 x �߽�����
    int xLMark;
    int xRMark;                         //x �߽�ɨ����ĸ���
}RoadBoundary;                          //��¼������

typedef enum                            //��Ϊ�������ݲ���ʹ��
{
    OUT_OF_RANGE = 0,                   //���߽�
    ALL_WHITE,                          //ȫ��
    NO_EXIST,                           //������
    IMG_ERROR,
    LEFT_SG_MAX = 11,
    RIGHT_SG_MAX = 10,
    CAR_LEFT,
    CAR_RIGHT
}IMG_INFORM;                            //ͼ����Ϣö��


void  imageProcessing       (void);                                 //ͼ�����������
void  getRoadBoundary       (RoadBoundary *rB,int y);               //���һ�������б߽�
int   filterRoadBoundary    (RoadBoundary *rB,int y);               //���˳�����N���߽�
int   findMaxRoadB          (RoadBoundary *rB,int y);               //���˳����ı߽�
int   ifImgError            (void);                                 //�ж�ͼ���Ƿ���ȷ
int   findEffectiveRoad     (RoadBoundary *rB,int y);               //�ҵ���Ч�߽磬���ݱ߽�������
int   ringProcess           (void);                                 //Բ������
int   crossProcess          (void);                                 //ʮ�ִ���
int   obstacleProcess       (void);
int   getMiddle             (void);                                     //��roadFB�߽�ת�����е�
int   avgMid                (int* middle,int bottonY,int topY);         //��ƽ���е�
int   ifOutBoundary         (int* middle);
int   CrossSlowDown         (void);                                     //����ʮ�ּ���    

#endif