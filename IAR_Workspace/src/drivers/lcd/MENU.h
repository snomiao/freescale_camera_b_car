#ifndef	_MENU_H_
#define	_MENU_H_

#define LINE_SPACING 16  //�и�
#define MENU_STRING_X 50 //menu�����xλ��50
#define MENU_STRING_Y 5  //menu�����yλ��
#define LINE_STRING_X 15  //menuĿ¼�µ�һ��x����
#define LINE_STRING_Y 21   //menuĿ¼�µ�һ��y����

//Ŀ¼ÿһ����ʾ����
#define LINE1   "1:RUN"
#define LINE2   "2:DealedView" 
#define LINE3   "3:LEARN"
#define LINE4   "4:PidSet"
#define LINE5   "5:MoreFunc"

//pid��Ŀ¼����ʾ����
#define pidLINE1   "1:e5"
#define pidLINE2   "2:e10" 
#define pidLINE3   "3:e15"
#define pidLINE4   "4:e20"
#define pidLINE5   "5:e40"
#define pidLINE6   "6:mP"

//moreFunc��Ŀ¼����ʾ����
#define moreLINE1   "1:SGManual"
#define moreLINE2   "2:CameraView"
#define moreLINE3   "3:"
#define moreLINE4   "4:"
#define moreLINE5   "5:"
#define moreLINE6   "6:"

typedef	struct{
	int chosenLine;						//Ŀǰѡ����
	int endLine;						//ѡ��������
}LCDMenu;								//����˵���ѡ����

typedef enum 							//��Ϊ�������ݲ���ʹ��
{
 	KEY_NULL = 0,
	KEY_MIDDLE,
	KEY_DOWN,
	KEY_UP,
	KEY_RIGHT,
	KEY_LEFT,
}KEY_STATUS;

typedef enum 							//��Ϊ�������ݲ���ʹ��
{
 	FUNC_OPEN = 0,
	FUNC_CLOSE
}FUNC_KEY;								//���ܿ���

struct ss_MENU_ITEM{
  u8 *title;
  void (*func_left)();
  void (*func_middle)();
  void (*func_right)();
};


struct ss_VAL{
  u8 *title;
  float val;
};


void  menuStart			(void);						//���˵�����
void  pidMenuList		(LCDMenu* plcdMenu);		//pid�˵�����
void  moreFuncMenuList	(LCDMenu* plcdMenu);		//moreFunc�˵�����		
void  menuPrint			(LCDMenu* plcdMenu);		//lcd��ӡ�˵�
void  pidMenuPrint		(LCDMenu* plcdMenu);		//pid�˵�ҳ���ӡ�˵�
void  moreMenuPrint		(LCDMenu* plcdMenu);		//moreFunc�˵�ҳ���ӡ�˵�
s16 menuChoose		(LCDMenu* plcdMenu);		//���ݰ����źŸ���ѡ��
s16 setPid			(LCDMenu* plcdMenu);		//���ݰ����ı�pֵ
s16    getKey			(void);						//��ð����ź�

#endif