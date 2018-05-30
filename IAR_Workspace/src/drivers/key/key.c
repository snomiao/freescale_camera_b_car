/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名       ：key.c
 * 描述         ：key函数定义
 *
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：野火嵌入式开发工作室
 * 淘宝店       ：http://firestm32.taobao.com
 * 技术支持论坛 ：http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/	


#include "key.h"


PTxn KEY_PTxn[KEY_MAX] = {PTD8,PTD9,PTD10,PTD11,PTE6,PTE7};

/*************************************************************************
*                             野火嵌入式开发工作室
*
*  函数名称：key_init
*  功能说明：初始化key端口，设置IO口为输入方向，上拉
*  参数说明：按键类型
*  函数返回：无
*  修改时间：2012-10-7   已测试
*  备    注：
*************************************************************************/
void    KEY_init(KEY_e key)                                               // KEY初始化 函数
{
    gpio_init((PORTx)(KEY_PTxn[key]>>5),KEY_PTxn[key] & 0x1F,GPI_UP,0);
} 

/*************************************************************************
*                             野火嵌入式开发工作室
*
*  函数名称：key_init
*  功能说明：初始化key端口，设置IO口为输入方向，上拉
*  参数说明：按键类型
*  函数返回：无
*  修改时间：2012-11-18   已测试
*  备    注：
*************************************************************************/
void    KEY_ALL_init(void)
{
    uint8 key;
    for(key =0;key <KEY_MAX ;key++)
    {
        gpio_init((PORTx)(KEY_PTxn[key]>>5),KEY_PTxn[key] & 0x1F,GPI_UP,0);
    }

}

/*************************************************************************
*                             野火嵌入式开发工作室
*
*  函数名称：key_check
*  功能说明：检测key状态
*  参数说明：按键类型
*  函数返回：按键状态
*  修改时间：2012-10-7   已测试
*  备    注：
*************************************************************************/
KEY_STATUS_e key_check(KEY_e key)
{
    u8 keyport = (u8)(KEY_PTxn[key]>>5);
    u8 keyn    = (u8)(KEY_PTxn[key] & 0x1f);
    if(GPIO_Get(keyport,keyn) == KEY_DOWN)
    {
        DELAY_MS(10);
        if(GPIO_Get(keyport,keyn) == KEY_DOWN)
        {
            return KEY_DOWN;
        }
    }
    return KEY_UP;
}



    
#define KEY_MSG_FIFO_SIZE   20      //最多 255，否则需要修改key_msg_front/key_msg_rear类型

typedef enum
{
    KEY_MSG_EMPTY,      //没有按键消息
    KEY_MSG_NORMAL,     //正常，有按键消息，但不满
    KEY_MSG_FULL,       //按键消息满
}key_msg_e;


KEY_MSG_t       key_msg[KEY_MSG_FIFO_SIZE];
volatile u8     key_msg_front = 0,key_msg_rear = 0; //接收FIFO的指针
volatile u8     key_msg_flag = KEY_MSG_EMPTY;


void send_key_msg(KEY_MSG_t keymsg)
{
    //保存在FIFO里
    if(key_msg_flag == KEY_MSG_FULL)
    {
        //满了直接不处理
        return ;
    }
    key_msg[key_msg_rear].key=keymsg.key;
    key_msg[key_msg_rear].status=keymsg.status;
    
    key_msg_rear++;
    
    if(key_msg_rear >= KEY_MSG_FIFO_SIZE)
    {
        key_msg_rear=0;                         //重头开始
    }
    
    if(key_msg_rear == key_msg_front)                   //追到屁股了，满了
    {
        key_msg_flag = KEY_MSG_FULL;   
    }
    else
    {
        key_msg_flag = KEY_MSG_NORMAL;
    } 
}

u8 get_key_msg(KEY_MSG_t * keymsg)
{
    if(key_msg_flag == KEY_MSG_EMPTY)   //空，直接返回0
    {
        return 0;
    }
    keymsg->key = key_msg[key_msg_front].key;
    keymsg->status = key_msg[key_msg_front].status;
    
    key_msg_front++;                //由于非空，所以可以直接出队列
    
    if(key_msg_front >= KEY_MSG_FIFO_SIZE)
    {
        key_msg_front=0;            //重头开始
    }
    
    if(key_msg_front == key_msg_rear)       //追到屁股了，接收队列空
    {
        key_msg_flag = KEY_MSG_EMPTY;
    }
    
    return 1;
}

/*************************************************************************
*                             野火嵌入式开发工作室
*
*  函数名称：key_IRQHandler
*  功能说明：定时检测key状态
*  参数说明：按键类型
*  函数返回：按键状态
*  修改时间：2012-10-7   已测试
*  备    注：
*************************************************************************/
void key_IRQHandler(void)
{
    
    u8      keyport;
    u8      keyn;
    KEY_e   keynum;
    static u8 keytime[KEY_MAX];
    KEY_MSG_t keymsg;
    for(keynum = (KEY_e)0 ;keynum < KEY_MAX;keynum ++)
    {
        keyport = (u8)(KEY_PTxn[keynum]>>5);
        keyn    = (u8)(KEY_PTxn[keynum] & 0x1f);
        if(GPIO_Get(keyport,keyn) == KEY_DOWN)
        {
            keytime[keynum]++;
            
            if(keytime[keynum] <= KEY_DOWN_TIME)
            {
                continue;
            }
            else if(keytime[keynum] == KEY_DOWN_TIME +1 )
            {
                keymsg.key = keynum;
                keymsg.status=KEY_DOWN;
                send_key_msg(keymsg);
            }
            else if(keytime[keynum] <= KEY_HOLD_TIME)
            {
                continue;
            }
            else if(keytime[keynum]  == KEY_HOLD_TIME +1)
            {
                keymsg.key = keynum;
                keymsg.status=KEY_HOLD;
                send_key_msg(keymsg);
                //keytime[keynum] = KEY_DOWN_TIME +1;
            }
            else
            {
                keytime[keynum] = KEY_DOWN_TIME +1;
            }
        }
        else
        {
            if(keytime[keynum] > KEY_DOWN_TIME)
            {
                keymsg.key = keynum;
                keymsg.status=KEY_UP;
                send_key_msg(keymsg);           //按键弹起
            }
            keytime[keynum] = 0;
        }
    }
}



