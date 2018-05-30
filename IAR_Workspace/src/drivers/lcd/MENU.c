#include "common.h"
#include "MENU.h"
#include "BL144002.h"
#include "LCD.h"
#include "ASCII.h"
#include "FTM.h"
#include "STEERING_GEAR.h"
#include "MOTOR.h"
#include "CAMERA_FUNC.h"

LCDMenu myMenu = {1,5},pidMenu = {1,6},moreFuncMenu = {1,6};    //两个结构体变量，第一项为初始选项位置，第二项为选项数量
KEY_STATUS keyOperation = KEY_NULL;                        //定义变量存储按键信号，用于各个文件
s16 SG_Pid_Set[6] = {40,60,90,110,130,20};          //pid值，前5个为舵机，第6个为电机
extern u16 LEFT_MAX;
extern u16 MIDDLE_DEGREE;
extern u16 RIGHT_MAX;
/******************************************************
 * 函数名：startMenu
 * 描述  ：加载菜单(外部调用)
 * 输入  : void
 * 输出  ：void
 * 注意  ：    （）测试
*********************************************************/    


void func_empty(){} //空函数，什么也不干
void func_return(){} //返回函数，作标记用

void enter_pidMenuList(){
  pidMenuList(&pidMenu);
}

void ss_menu_print(struct ss_MENU_ITEM* menu_item, s16 menu_length, s16 sel_index){
    Site_type pos = {0, 0}; //左上角
    for(s16 i = 0; i < menu_length; i++){
        if( i == sel_index)
          LCD_Str_8x16(pos, menu_item[i].title, BLACK, WHITE);
        else
          LCD_Str_8x16(pos, menu_item[i].title, WHITE, BLACK);
        pos.y += 16; //换行
    }
}

#define FUNC_RISING_EDGE(NAME, VALUE) \
  u8 NAME##_RISING_EDGE(){\
    static u8 last = 0;\
    u8 now = VALUE;\
    if(now && !last){\
      last = now;\
      return 1;\
    }else{\
      last = now;\
      return 0;\
    }\
  }

  

void ss_menu_sel(struct ss_MENU_ITEM* menu_item, s16 menu_length, s16 sel_index){
    // 检查参数
    ASSERT(sel_index == LIMITED(sel_index, 0, menu_length - 1));

    ss_menu_print(menu_item, menu_length, sel_index);
    printf("SEL> %d: %s\n", sel_index, menu_item[sel_index].title);
}

KEY_STATUS processKeyOperation(){
  KEY_STATUS keyOperation_return = keyOperation;
  keyOperation = KEY_NULL;
  return keyOperation_return ;
}

#define GO_PREV(VAL, LEN) (VAL = (VAL == 0 ? LEN - 1 : VAL - 1))
#define GO_NEXT(VAL, LEN) (VAL = (VAL == LEN - 1 ? 0 : VAL + 1))
  
void ss_menu(struct ss_MENU_ITEM* menu_item, s16 menu_length){
    // 为了使用 % 号，这里使用有符号数
    s16 sel_index = 0; 
    
    LCD_Clear(BLACK);
    ss_menu_sel(menu_item, menu_length, sel_index);
            
    void (*f)() = func_empty; 
    while(1){

        // 处理按键
        int key = keyOperation;
        keyOperation = KEY_NULL;
        switch(key){
        case KEY_NULL:   continue;
        case KEY_UP:
            GO_PREV(sel_index, menu_length);
            ss_menu_sel(menu_item, menu_length, sel_index);
            break;
        case KEY_DOWN:
            GO_NEXT(sel_index, menu_length);
            ss_menu_sel(menu_item, menu_length, sel_index);
            break;
        case KEY_LEFT:   f = menu_item[sel_index].func_left;   break;
        case KEY_MIDDLE: f = menu_item[sel_index].func_middle; break;
        case KEY_RIGHT:  f = menu_item[sel_index].func_right;  break;
        }
        if(f == func_empty)
            continue;
        if(f == func_return)
            return;
        
        // 进入函数
        // printf("KEY_LEFT> %d: %s\n", sel_index, menu_item[sel_index].title);
        (*f)();
        //bugfix
        keyOperation = KEY_NULL;
        // 退出函数
        LCD_Clear(BLACK);
        ss_menu_sel(menu_item, menu_length, sel_index);
    }
}

void menuStart(void){
#define startMenu_MENU_LEN (8)
  struct ss_MENU_ITEM startMenu[startMenu_MENU_LEN] = {
    // 显示名称           左键         中键               右键
    {"CONTROL"         , func_return, CONTROL          , CONTROL          },
    {"LEARN"           , func_return, LEARN            , LEARN            },
    {"START"           , func_return, START            , START            },
    {"DealedView"      , func_return, DealedView       , DealedView       },
    {"STEERO_DEBUG"    , func_return, STEERO_DEBUG     , STEERO_DEBUG     },
    {"CameraView"      , func_return, CameraView       , CameraView       },
    {"Param SET"       , func_return, enter_pidMenuList, enter_pidMenuList},
    {"sGManual"        , func_return, sGManual         , sGManual         },
  };
  // 根菜单无法退出
  while(1) ss_menu(startMenu, startMenu_MENU_LEN );
}

/******************************************************
 * 函数名：menuPrint
 * 描述  ：打印目录页，带有强调哪一项被选择（黑底白字）(内部调用)
 * 输入  : LCDMenu结构体指针
 * 输出  ：void
 * 注意  ：    （）测试
*********************************************************/ 
void pidMenuList(LCDMenu* plcdMenu){
    LCD_Clear(BLUE);
    while(1){
        pidMenuPrint(plcdMenu); //打印pid子菜单
        menuChoose(plcdMenu);   //选择pid子菜单选项
        if(keyOperation == KEY_MIDDLE){     //中键进入修改
            keyOperation = KEY_NULL;
            while(1){
                pidMenuPrint(&pidMenu);             //打印pid子菜单
                setPid(&pidMenu);                   //设置pid
                if(menuChoose(&pidMenu) == -1){     //左键返回
                    keyOperation = KEY_NULL;
                    LCD_Clear(BLUE);
                    break;
                }
            }
        }
        if(keyOperation == KEY_LEFT){       //左键返回
            LCD_Clear(BLUE);
            break;
        }
    }
}

/******************************************************
 * 函数名：menuPrint
 * 描述  ：打印目录页，带有强调哪一项被选择（黑底白字）(内部调用)
 * 输入  : LCDMenu结构体指针
 * 输出  ：void
 * 注意  ：    （）测试
*********************************************************/ 
void moreFuncMenuList(LCDMenu* plcdMenu){
    LCD_Clear(BLUE);
    while(1){
        moreMenuPrint(plcdMenu);    //打印pid子菜单
        menuChoose(plcdMenu);       //选择pid子菜单选项
        if(keyOperation == KEY_MIDDLE){     //中键进入修改
            keyOperation = KEY_NULL;
            switch(plcdMenu->chosenLine){           //判断进入哪个子菜单或功能
                case 1: sGManual() ; break;         
                case 2: CameraView(); break;
                case 3: ; break;
                case 4: ; break;
                case 5: ; break;
                default :continue;
            }
        }
        if(keyOperation == KEY_LEFT){       //左键返回
            LCD_Clear(BLUE);
            break;
        }
    }
}

/******************************************************
 * 函数名：pidMenuPrint
 * 描述  ：打印目录页，带有强调哪一项被选择（黑底白字）(内部调用)
 * 输入  : LCDMenu结构体指针
 * 输出  ：void
 * 注意  ：    （）测试
*********************************************************/    
 

void pidMenuPrint(LCDMenu* plcdMenu){
    s16 loopTimes = 0;
    Site_type menuStringType = {40,MENU_STRING_Y}; //menuString位置
    Site_type lineStringType = {15,LINE_STRING_Y}; //选项第一行位置
    u8 *lineString[6]={pidLINE1,pidLINE2,pidLINE3,pidLINE4,pidLINE5,pidLINE6};

    //LCD_Init(WHITE);  //新增语句，用于刷新，未过多测试
    LCD_Str_8x16(menuStringType,"pidMenu", BLACK, WHITE);//lcd打印menu菜单的标题

    //打印1――5行选项
    for(loopTimes = 0;loopTimes < plcdMenu->endLine;loopTimes ++){
        if(loopTimes+1 == plcdMenu->chosenLine)
            LCD_Str_8x16(lineStringType,lineString[loopTimes], WHITE, BLACK);
        else
            LCD_Str_8x16(lineStringType,lineString[loopTimes], BLACK, WHITE);
        lineStringType.y += LINE_SPACING; //换行
    } 
}


// 折叠 //


    /******************************************************
     * 函数名：menuPrint
     * 描述  ：打印目录页，带有强调哪一项被选择（黑底白字）(内部调用)
     * 输入  : LCDMenu结构体指针
     * 输出  ：void
     * 注意  ：    （）测试
    *********************************************************/    
     

    void menuPrint(LCDMenu* plcdMenu){
        s16 loopTimes = 0;
        Site_type menuStringType = {MENU_STRING_X,MENU_STRING_Y}; //menuString位置
        Site_type lineStringType = {LINE_STRING_X,LINE_STRING_Y}; //选项第一行位置
        u8 *lineString[5]={LINE1,LINE2,LINE3,LINE4,LINE5};

        //LCD_Init(WHITE);  //新增语句，用于刷新，未过多测试
        LCD_Str_8x16(menuStringType,"Menu", BLACK, WHITE);//lcd打印menu菜单的标题

        //打印1――5行选项
        for(loopTimes = 0;loopTimes < plcdMenu->endLine;loopTimes ++){
            if(loopTimes+1 == plcdMenu->chosenLine)
                LCD_Str_8x16(lineStringType,lineString[loopTimes], WHITE, BLACK);
            else
                LCD_Str_8x16(lineStringType,lineString[loopTimes], BLACK, WHITE);
            lineStringType.y += LINE_SPACING; //换行
        } 
    }

    /******************************************************
     * 函数名：moreMenuPrint
     * 描述  ：打印目录页，带有强调哪一项被选择（黑底白字）(内部调用)
     * 输入  : LCDMenu结构体指针
     * 输出  ：void
     * 注意  ：    （）测试
    *********************************************************/    
     

    void moreMenuPrint(LCDMenu* plcdMenu){
        s16 loopTimes = 0;
        Site_type menuStringType = {15,MENU_STRING_Y}; //menuString位置
        Site_type lineStringType = {15,LINE_STRING_Y}; //选项第一行位置
        u8 *lineString[6]={moreLINE1,moreLINE2,moreLINE3,moreLINE4,moreLINE5,moreLINE6};

        //LCD_Init(WHITE);  //新增语句，用于刷新，未过多测试
        LCD_Str_8x16(menuStringType,"moreFuncMenu", BLACK, WHITE);//lcd打印menu菜单的标题

        //打印1――5行选项
        for(loopTimes = 0;loopTimes < plcdMenu->endLine;loopTimes ++){
            if(loopTimes+1 == plcdMenu->chosenLine)
                LCD_Str_8x16(lineStringType,lineString[loopTimes], WHITE, BLACK);
            else
                LCD_Str_8x16(lineStringType,lineString[loopTimes], BLACK, WHITE);
            lineStringType.y += LINE_SPACING; //换行
        } 
    }


    /******************************************************
     * 函数名：keyChoose
     * 描述  ：根据按键改变chosenLine的值(内部调用)
     * 输入  : LCDMenu结构体指针
     * 输出  ：chosenLine值
     * 注意  ：    （）测试
    *********************************************************/    

    s16 menuChoose(LCDMenu* plcdMenu){    
        if(keyOperation == KEY_NULL){
            return 1;
        }
        else if(keyOperation == KEY_DOWN){
            if(plcdMenu->chosenLine == plcdMenu->endLine)
                plcdMenu->chosenLine = 1;
            else
                ++plcdMenu->chosenLine;
            
            
            
            switch (plcdMenu->chosenLine){
              case 1: printf(LINE1); printf("\n"); break;
              case 2: printf(LINE2); printf("\n"); break;
              case 3: printf(LINE3); printf("\n");break;
              case 4: printf(LINE4); printf("\n");break;
              case 5: printf(LINE5); printf("\n");break;
            }
            
            
            keyOperation = KEY_NULL;
            return 1;
        }
        else if(keyOperation == KEY_UP){
            if(plcdMenu->chosenLine == 1)
                plcdMenu->chosenLine = plcdMenu->endLine;
            else
                --plcdMenu->chosenLine;
            
            
            switch (plcdMenu->chosenLine){
              case 1: printf(LINE1); printf("\n");break;
              case 2: printf(LINE2); printf("\n");break;
              case 3: printf(LINE3); printf("\n");break;
              case 4: printf(LINE4); printf("\n");break;
              case 5: printf(LINE5); printf("\n");break;
            }
            
            
            keyOperation = KEY_NULL;
            return 1;
        }
        else if(keyOperation == KEY_MIDDLE){
    //      LCD_Init(BLUE);
            return -1;
        }
        return 0;
    }

/******************************************************
 * 函数名：setPid
 * 描述  ：pid设置页面，进行加减pid（内部调用）
 * 输入  : 
 * 输出  ：一个标记
 * 注意  ：    （）测试
*********************************************************/    


s16 setPid(LCDMenu* plcdMenu){
    s16 i;

    for(i = 0; i < plcdMenu->endLine ; i++){
        LCD_Num_8x16_X (70,21 + i * LINE_SPACING,SG_Pid_Set[i],BLACK,WHITE);
    }

    if(keyOperation == KEY_RIGHT){
        SG_Pid_Set[plcdMenu->chosenLine - 1] += 2;
        keyOperation = KEY_NULL;
    }else if(keyOperation == KEY_LEFT){
        SG_Pid_Set[plcdMenu->chosenLine - 1] -= 2;
        keyOperation = KEY_NULL;
    }
        
    return 0;
}


/******************************************************
 * 函数名：getKey
 * 描述  ：获取按键(内部调用)
 * 输入  : 
 * 输出  ：一个标记
 * 注意  ：    （）测试
*********************************************************/    


s16 getKey(void){
    if(downKey   == 0) return KEY_DOWN;
    if(upKey     == 0) return KEY_UP;
    if(rightKey  == 0) return KEY_RIGHT;
    if(leftKey   == 0) return KEY_LEFT;
    if(middleKey == 0) return KEY_MIDDLE;
    return KEY_NULL;
}