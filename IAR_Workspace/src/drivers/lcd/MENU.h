#ifndef	_MENU_H_
#define	_MENU_H_

#define LINE_SPACING 16  //行高
#define MENU_STRING_X 50 //menu标题的x位置50
#define MENU_STRING_Y 5  //menu标题的y位置
#define LINE_STRING_X 15  //menu目录下第一行x坐标
#define LINE_STRING_Y 21   //menu目录下第一行y坐标

//目录每一行显示文字
#define LINE1   "1:RUN"
#define LINE2   "2:DealedView" 
#define LINE3   "3:LEARN"
#define LINE4   "4:PidSet"
#define LINE5   "5:MoreFunc"

//pid子目录下显示文字
#define pidLINE1   "1:e5"
#define pidLINE2   "2:e10" 
#define pidLINE3   "3:e15"
#define pidLINE4   "4:e20"
#define pidLINE5   "5:e40"
#define pidLINE6   "6:mP"

//moreFunc子目录下显示文字
#define moreLINE1   "1:SGManual"
#define moreLINE2   "2:CameraView"
#define moreLINE3   "3:"
#define moreLINE4   "4:"
#define moreLINE5   "5:"
#define moreLINE6   "6:"

typedef	struct{
	int chosenLine;						//目前选择项
	int endLine;						//选择项数量
}LCDMenu;								//定义菜单的选择项

typedef enum 							//作为按键传递参数使用
{
 	KEY_NULL = 0,
	KEY_MIDDLE,
	KEY_DOWN,
	KEY_UP,
	KEY_RIGHT,
	KEY_LEFT,
}KEY_STATUS;

typedef enum 							//作为按键传递参数使用
{
 	FUNC_OPEN = 0,
	FUNC_CLOSE
}FUNC_KEY;								//功能开关

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


void  menuStart			(void);						//主菜单加载
void  pidMenuList		(LCDMenu* plcdMenu);		//pid菜单加载
void  moreFuncMenuList	(LCDMenu* plcdMenu);		//moreFunc菜单加载		
void  menuPrint			(LCDMenu* plcdMenu);		//lcd打印菜单
void  pidMenuPrint		(LCDMenu* plcdMenu);		//pid菜单页面打印菜单
void  moreMenuPrint		(LCDMenu* plcdMenu);		//moreFunc菜单页面打印菜单
s16 menuChoose		(LCDMenu* plcdMenu);		//根据按键信号更改选项
s16 setPid			(LCDMenu* plcdMenu);		//根据按键改变p值
s16    getKey			(void);						//获得按键信号

#endif