
#include "key_event.h"
#include "NRF24L0.h"
#include "NRF24L0_MSG.h"


typedef struct
{
	u32 		val;		//目前的值
	u32 		oldval;		//通常情况下，两者是相同，修改后，没按确认键，则不相同。按确认键发送后，则相同
							//即最后发送的值
	u32			minval;		//最小值
	u32			maxval;		//最大值
	Site_type	    site;	
}ui_var_info_t;				//变量信息


static void var_select(ui_var_event_e  ctrl);   //选择切换变量
static void var_value(ui_var_event_e ctrl);     //改变变量的值
static void var_ok();                           //确认当前选择的
static void val_cancel();                       //取消当前选择的值  OK

//同步指定的值。tab 为 VAR_NUM 时表示全部同步，小于则同步对应的
//必须先同步再显示全部，因为有可能同步失败。
static u8 var_syn(u8 tab);         //同步数据，1表示成功，0表示失败

#define	VAR_VALUE(var_tab)		num_info[var_tab].val		//指定标号的变量的值
#define	VAR_OLDVALUE(var_tab)	num_info[var_tab].oldval	//指定标号的变量的最后确认值
#define	VAR_MAXVALUE(var_tab)	num_info[var_tab].maxval
#define	VAR_MINVALUE(var_tab)	num_info[var_tab].minval
#define	VAR_SITE(var_tab)	    num_info[var_tab].site

extern u8  var1,var2;
extern u16 var3,var4;
extern u32 var5,var6;


u32 * var_addr[VAR_MAX]={(u32 *)&var1,(u32 *)&var2,(u32 *)&var3,(u32 *)&var4,(u32 *)&var5,(u32 *)&var6};


ui_var_info_t  num_info[VAR_MAX]=
{
    //	{val,oldval,minval,maxval,{x,y}}
    //val,oldval,会在调用key_event_init的时候从其对应变量里赋值过来，所以这里的值可以随意写
    //需要设置minval,maxval,{x,y}
    //务必注意最小值不要大于最大值
	{0,0,0,100,{90,0}},			    //变量 var1，
	{0,0,0,100,{90,20}},			//变量 var2，
	{0,0,0,300,{90,40}},			//变量 var3，
	{0,0,0,300,{90,60}},			//变量 var4，
	{0,0,0,65540,{90,80}},			//变量 var5，
	{0,0,0,65540,{90,100}}			//变量 var6，
};



u8	loop_tab = 0;		//第一次默认显示的变量

void save_var(var_tab_e var_tal,u32 var_data)
{
    var_tab_e tmp; 
    if(var_tal <= VAR_8BIT)
    {
        *((u8 *)(var_addr[var_tal])) = var_data;
    }
    else if(var_tal <= VAR_16BIT)
    {
        *((u16 *)(var_addr[var_tal])) = var_data;
    }
    else if(var_tal <= VAR_32BIT)
    {
        *((u32 *)(var_addr[var_tal])) = var_data;
    }
    
    VAR_VALUE(var_tal) = var_data;
    VAR_OLDVALUE(var_tal) = var_data;
    
    if(loop_tab != var_tal )
    {
        tmp = loop_tab;
        loop_tab = var_tal;
        var_display(tmp);
        var_display(loop_tab);
    }
    else
    {
        var_display(loop_tab);
    }
}

void get_var(var_tab_e var_tal,u32 *var_data)
{
    if(var_tal <= VAR_8BIT)
    {
        *var_data = (u32)*((u8 *)(var_addr[var_tal]));
    }
    else if(var_tal <= VAR_16BIT)
    {
        *var_data = (u32)*((u16 *)(var_addr[var_tal])) ;
    }
    else if(var_tal <= VAR_32BIT)
    {
        *var_data = (u32)*((u32 *)(var_addr[var_tal]));
    }
}

void key_event_init()
{
    u8 var_num;
    u32 vartemp;
    //初始化临时变量
    for(var_num=0;var_num<VAR_MAX;var_num++)
    {
        get_var((var_tab_e)var_num,&vartemp);
        num_info[var_num].val       = vartemp;
        num_info[var_num].oldval    = vartemp;
        
        //检测最小值与最大值
        ASSERT(num_info[var_num].maxval  >=  num_info[var_num].minval );
    }
    
    //同步全部数据并显示
    var_syn(VAR_MAX);       //同步全部 ,必须先同步再显示全部，因为有可能同步失败。
    var_display(VAR_MAX);   //显示全部
    
}

void deal_key_event()
{
    KEY_MSG_t keymsg;

    while(get_key_msg(& keymsg))     //获得按键就进行处理
    {
        if(keymsg.status == KEY_DOWN)
        {
            switch(keymsg.key)
            {
            case KEY_ADD:
                var_value(VAR_ADD);
                break;
                
            case KEY_SUB:
                var_value(VAR_SUB);
                break;
                
            case KEY_PREV:
                var_select(VAR_PREV);
                break;
                
            case KEY_NEXT:
                var_select(VAR_NEXT);
                break;
                
            case KEY_OK:
                var_ok();
                break;
                
            case KEY_CANCEL:
                val_cancel();
                break;
            default:
                break;
            }
        }
        else if(keymsg.status == KEY_HOLD)
        {
            switch(keymsg.key)
            {
            case KEY_ADD:
                var_value(VAR_ADD_HOLD);
                break;
                
            case KEY_SUB:
                var_value(VAR_SUB_HOLD);
                break;
                
            case KEY_PREV:
                var_select(VAR_PREV_HOLD);
                break;
                
            case KEY_NEXT:
                var_select(VAR_NEXT_HOLD);
                break;
            //不需要处理OK和 cancel               

            default:
                break;
            }
        }
        else
        {
            //KEY_UP ,不进行处理
        }
    }
}


//选择下一个变量
void var_select(ui_var_event_e  ctrl)
{
	ASSERT(loop_tab<VAR_MAX);
	
	u8 old_tab = loop_tab;		//先备份当前变量标号
	
	//切换到下一个变量
	switch(ctrl)
	{
		case VAR_NEXT:					//下一个
		loop_tab++;
		if(loop_tab >= (VAR_MAX) )
		{
			loop_tab = 0;				//从头开始
		}
		break;
		
		case VAR_PREV:					//上一个
		loop_tab--;
		if(loop_tab >= (VAR_MAX) )
		{
			loop_tab = VAR_MAX-1;		//从尾开始
		}
		break;
		
		case VAR_NEXT_HOLD:					//快下
		loop_tab += VAR_SELECT_HOLD_OFFSET;
		if(loop_tab >= (VAR_MAX) )
		{
			loop_tab = 0;				//从头开始
		}		
		break;
		
		case VAR_PREV_HOLD:					//快上
		loop_tab -= VAR_SELECT_HOLD_OFFSET;
		if(loop_tab >= (VAR_MAX) )
		{
			loop_tab = VAR_MAX-1;		//从尾开始
		}
		break;

		default:						//无效选择，不需要切换
		return;
	}

	var_display(old_tab);				//处理上一个变量
	
	var_display(loop_tab);				//处理当前变量：
	
}

void var_value(ui_var_event_e ctrl)
{
	ASSERT(loop_tab<VAR_MAX);
	
	
	//修改当前变量的值
	switch(ctrl)
	{
		case VAR_ADD:
        if(VAR_VALUE(loop_tab) < VAR_MAXVALUE(loop_tab))
        {
            VAR_VALUE(loop_tab)++;
        }
        else
        {
            VAR_VALUE(loop_tab) = VAR_MINVALUE(loop_tab);
        }
		break;
		
		case VAR_SUB:
        if(VAR_VALUE(loop_tab) > VAR_MINVALUE(loop_tab))
        {
            VAR_VALUE(loop_tab)--;
        }
        else
        {
            VAR_VALUE(loop_tab) = VAR_MAXVALUE(loop_tab) ;//最小值减一为最大值
        }
		break;
		
		case VAR_ADD_HOLD:
        if(   VAR_MAXVALUE(loop_tab) - (VAR_VALUE(loop_tab)  >  VAR_VALUE_HOLE_OFFSET) )
        {
            VAR_VALUE(loop_tab)+=VAR_VALUE_HOLE_OFFSET;
        }
        else
        {
            VAR_VALUE(loop_tab) = VAR_MINVALUE(loop_tab);
        }
		break;
		
		case VAR_SUB_HOLD:
        if( ( VAR_VALUE(loop_tab) - VAR_MINVALUE(loop_tab)) > VAR_VALUE_HOLE_OFFSET  )
        {
            VAR_VALUE(loop_tab)-=VAR_VALUE_HOLE_OFFSET;
        }
        else
        {
            VAR_VALUE(loop_tab) = VAR_MAXVALUE(loop_tab) ;//最小值减一为最大值
        }
		break;
		
		default:						//无效选择，不需要切换
		break;
	}
	
	var_display(loop_tab);
}

//确认当前选择的
void var_ok()
{
	ASSERT(loop_tab<VAR_MAX);
	
	//比较是否有改变值
//	if(VAR_VALUE(loop_tab) != VAR_OLDVALUE(loop_tab))	//值改变了，则需要处理
	{
		var_syn(loop_tab);			//同步新的值
	}
	
	var_display(loop_tab);
}

//取消当前选择的值  OK
void val_cancel()
{
	ASSERT(loop_tab<VAR_MAX);
	
	//直接还原当前值
	VAR_VALUE(loop_tab) = VAR_OLDVALUE(loop_tab);
	
	var_display(loop_tab);
}

//显示指定的值。tab 为 VAR_MAX 时表示全部显示，小于则显示对应的
void var_display(u8 tab)
{
//定义背景延时
#define SELECT_NO_CHANGE_BG         WHITE   //当前选中，而且没有改变
#define SELECT_CHANGE_BG            WHITE   //当前选中，而且改变了 
#define NO_SELECT_NO_CHANGE_BG      RED     //没有选中，而且没有改变（普通的就是这样）
#define NO_SELECT_CHANGE_BG         RED     //没有选中，而且改变了    
    
//定义文字颜色
#define SELECT_NO_CHANGE            BLUE  //当前选中，而且没有改变
#define SELECT_CHANGE               GREEN  //当前选中，而且改变了 
#define NO_SELECT_NO_CHANGE         BLUE  //没有选中，而且没有改变（普通的就是这样）
#define NO_SELECT_CHANGE            GREEN  //没有选中，而且改变了  
    
	u8  i = 0;
    u16 bkColor;
    u16 Color;
    
	ASSERT((loop_tab < VAR_MAX) && (tab<=VAR_MAX));
	
	if(tab == VAR_MAX)
	{
		i = VAR_MAX-1;
		tab = 0;
	}

	do
	{
		if(tab == loop_tab)
		{
			//显示当前的值：判断值是否改变
			if(VAR_VALUE(tab) == VAR_OLDVALUE(tab))	//值没改变，不需要处理
			{
				Color   =  SELECT_NO_CHANGE;
                bkColor =  SELECT_NO_CHANGE_BG;  
			}
			else
			{
				Color   =  SELECT_CHANGE;
                bkColor =  SELECT_CHANGE_BG;  
			}
		}
		else
		{
			//显示非当前的值
			if(VAR_VALUE(tab) == VAR_OLDVALUE(tab))	//值没改变，不需要处理
			{
				Color   =  NO_SELECT_NO_CHANGE;
                bkColor =  NO_SELECT_NO_CHANGE_BG;  
				
			}
			else
			{
				Color   =  NO_SELECT_CHANGE;
                bkColor =  NO_SELECT_CHANGE_BG;  
			}
		}
        
		//显示文字
		LCD_Num_C(VAR_SITE(tab),VAR_VALUE(tab),Color,bkColor);

		tab++;
	}while(i--);			//tab != VAR_MAX 的时候，执行一次就跳出
}

//同步指定的值。tab 为 VAR_MAX 时表示全部同步，小于则同步对应的
u8 var_syn(u8 tab)
{
	ASSERT((loop_tab < VAR_MAX) && (tab<=VAR_MAX));
	
	u8  i = 0;
    u8 tempbuff[MAX_ONCE_TX_NUM];
    u8 status;
    
	if(tab == VAR_MAX)
	{
		i = VAR_MAX-1;
		tab = 0;
	}

	do
	{
        //把值复制到对应的变量
        save_var((var_tab_e)tab,VAR_VALUE(loop_tab));
        
		//发送新的值
		//********待写
		save_var2buff((var_tab_e)tab,tempbuff);         //把变量写进 tempbuff 里
        NRF_MSG_send(COM_EVENT_INFORM,tempbuff);        //发送数据
        do
        {
            status = NRF_MSG_send_state();		//获取发送状态
        }while( status == TX_ISR_SEND);       	//如果发送中，则需要继续等待发送完成
        
        if(TX_ISR_FAIL == status)               //发送失败
        {
            //还原值
            //VAR_VALUE(loop_tab) = VAR_OLDVALUE(loop_tab);           //还原旧的值
            save_var((var_tab_e)tab,VAR_OLDVALUE(loop_tab));           //修改变量的保存值
            //get_var((var_tab_e)tab,&(VAR_OLDVALUE(loop_tab)));      //修改变量的保存值
            
            //放弃同步
            return 0;
        }
        
		VAR_OLDVALUE(loop_tab) = VAR_VALUE(loop_tab);//更新旧的值
		
		tab++;
	}while(i--);
    
    return 1;
}




