#ifndef _KEY_EVNET_H_
#define _KEY_EVNET_H_

#include "common.h"
#include "LCD.h"
#include "key.h"

#define	VAR_SELECT_HOLD_OFFSET		((VAR_MAX+4-1)/4)		//快上快下时，每次切换偏移量。目前设置为变量总数的1/4.可修改成其他数
#define	VAR_VALUE_HOLE_OFFSET		10


//变量的发送与接收
typedef enum
{
    //变量的编号
    
    /* 8位变量 */ 
    VAR1, 
    VAR2,
    VAR_8BIT = VAR2, //8位变量的结束编号
   
    /* 16位变量 */
    VAR3,
    VAR4,
    VAR_16BIT  = VAR4,          //16位变量的结束编号
    
    /* 32位变量 */
    VAR5,
    VAR6,
    VAR_32BIT  = VAR6,          //32位变量的结束编号
    
    VAR_MAX,        //变量数目
}var_tab_e;

//extern u32 * var_addr[VAR_MAX];                  //保存变量的地址，同步的时候会把num_info里的值赋值到对应的变量

//extern ui_var_info_t  num_info[VAR_MAX];        //保存变量的临时改变值


typedef enum
{
	VAR_NEXT,			//下一个
	VAR_PREV,			//上一个
	VAR_NEXT_HOLD,		//快下，偏移为：VAR_SELECT_HOLD_OFFSET
	VAR_PREV_HOLD,		//快上，偏移为：VAR_SELECT_HOLD_OFFSET
	
	VAR_ADD,			//加1
	VAR_SUB,			//减1
	VAR_ADD_HOLD,		//快加，偏移为：VAR_VALUE_HOLE_OFFSET
	VAR_SUB_HOLD,		//快减，偏移为：VAR_VALUE_HOLE_OFFSET
	
	VAR_OK,				//确定
	VAR_CANCEL,			//取消
	
	VAR_EVENT_MAX,
}ui_var_event_e;


void key_event_init();
void deal_key_event(void);
void save_var(var_tab_e var_tal,u32 var_data);
void get_var(var_tab_e var_tal,u32 *var_data);
void var_display(u8 tab);                //显示指定的值。tab 为 VAR_NUM 时表示全部显示，小于则显示对应的











#endif  //_KEY_EVNET_H_