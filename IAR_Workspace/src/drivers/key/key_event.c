
#include "key_event.h"
#include "NRF24L0.h"
#include "NRF24L0_MSG.h"


typedef struct
{
	u32 		val;		//Ŀǰ��ֵ
	u32 		oldval;		//ͨ������£���������ͬ���޸ĺ�û��ȷ�ϼ�������ͬ����ȷ�ϼ����ͺ�����ͬ
							//������͵�ֵ
	u32			minval;		//��Сֵ
	u32			maxval;		//���ֵ
	Site_type	    site;	
}ui_var_info_t;				//������Ϣ


static void var_select(ui_var_event_e  ctrl);   //ѡ���л�����
static void var_value(ui_var_event_e ctrl);     //�ı������ֵ
static void var_ok();                           //ȷ�ϵ�ǰѡ���
static void val_cancel();                       //ȡ����ǰѡ���ֵ  OK

//ͬ��ָ����ֵ��tab Ϊ VAR_NUM ʱ��ʾȫ��ͬ����С����ͬ����Ӧ��
//������ͬ������ʾȫ������Ϊ�п���ͬ��ʧ�ܡ�
static u8 var_syn(u8 tab);         //ͬ�����ݣ�1��ʾ�ɹ���0��ʾʧ��

#define	VAR_VALUE(var_tab)		num_info[var_tab].val		//ָ����ŵı�����ֵ
#define	VAR_OLDVALUE(var_tab)	num_info[var_tab].oldval	//ָ����ŵı��������ȷ��ֵ
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
    //val,oldval,���ڵ���key_event_init��ʱ������Ӧ�����︳ֵ���������������ֵ��������д
    //��Ҫ����minval,maxval,{x,y}
    //���ע����Сֵ��Ҫ�������ֵ
	{0,0,0,100,{90,0}},			    //���� var1��
	{0,0,0,100,{90,20}},			//���� var2��
	{0,0,0,300,{90,40}},			//���� var3��
	{0,0,0,300,{90,60}},			//���� var4��
	{0,0,0,65540,{90,80}},			//���� var5��
	{0,0,0,65540,{90,100}}			//���� var6��
};



u8	loop_tab = 0;		//��һ��Ĭ����ʾ�ı���

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
    //��ʼ����ʱ����
    for(var_num=0;var_num<VAR_MAX;var_num++)
    {
        get_var((var_tab_e)var_num,&vartemp);
        num_info[var_num].val       = vartemp;
        num_info[var_num].oldval    = vartemp;
        
        //�����Сֵ�����ֵ
        ASSERT(num_info[var_num].maxval  >=  num_info[var_num].minval );
    }
    
    //ͬ��ȫ�����ݲ���ʾ
    var_syn(VAR_MAX);       //ͬ��ȫ�� ,������ͬ������ʾȫ������Ϊ�п���ͬ��ʧ�ܡ�
    var_display(VAR_MAX);   //��ʾȫ��
    
}

void deal_key_event()
{
    KEY_MSG_t keymsg;

    while(get_key_msg(& keymsg))     //��ð����ͽ��д���
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
            //����Ҫ����OK�� cancel               

            default:
                break;
            }
        }
        else
        {
            //KEY_UP ,�����д���
        }
    }
}


//ѡ����һ������
void var_select(ui_var_event_e  ctrl)
{
	ASSERT(loop_tab<VAR_MAX);
	
	u8 old_tab = loop_tab;		//�ȱ��ݵ�ǰ�������
	
	//�л�����һ������
	switch(ctrl)
	{
		case VAR_NEXT:					//��һ��
		loop_tab++;
		if(loop_tab >= (VAR_MAX) )
		{
			loop_tab = 0;				//��ͷ��ʼ
		}
		break;
		
		case VAR_PREV:					//��һ��
		loop_tab--;
		if(loop_tab >= (VAR_MAX) )
		{
			loop_tab = VAR_MAX-1;		//��β��ʼ
		}
		break;
		
		case VAR_NEXT_HOLD:					//����
		loop_tab += VAR_SELECT_HOLD_OFFSET;
		if(loop_tab >= (VAR_MAX) )
		{
			loop_tab = 0;				//��ͷ��ʼ
		}		
		break;
		
		case VAR_PREV_HOLD:					//����
		loop_tab -= VAR_SELECT_HOLD_OFFSET;
		if(loop_tab >= (VAR_MAX) )
		{
			loop_tab = VAR_MAX-1;		//��β��ʼ
		}
		break;

		default:						//��Чѡ�񣬲���Ҫ�л�
		return;
	}

	var_display(old_tab);				//������һ������
	
	var_display(loop_tab);				//����ǰ������
	
}

void var_value(ui_var_event_e ctrl)
{
	ASSERT(loop_tab<VAR_MAX);
	
	
	//�޸ĵ�ǰ������ֵ
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
            VAR_VALUE(loop_tab) = VAR_MAXVALUE(loop_tab) ;//��Сֵ��һΪ���ֵ
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
            VAR_VALUE(loop_tab) = VAR_MAXVALUE(loop_tab) ;//��Сֵ��һΪ���ֵ
        }
		break;
		
		default:						//��Чѡ�񣬲���Ҫ�л�
		break;
	}
	
	var_display(loop_tab);
}

//ȷ�ϵ�ǰѡ���
void var_ok()
{
	ASSERT(loop_tab<VAR_MAX);
	
	//�Ƚ��Ƿ��иı�ֵ
//	if(VAR_VALUE(loop_tab) != VAR_OLDVALUE(loop_tab))	//ֵ�ı��ˣ�����Ҫ����
	{
		var_syn(loop_tab);			//ͬ���µ�ֵ
	}
	
	var_display(loop_tab);
}

//ȡ����ǰѡ���ֵ  OK
void val_cancel()
{
	ASSERT(loop_tab<VAR_MAX);
	
	//ֱ�ӻ�ԭ��ǰֵ
	VAR_VALUE(loop_tab) = VAR_OLDVALUE(loop_tab);
	
	var_display(loop_tab);
}

//��ʾָ����ֵ��tab Ϊ VAR_MAX ʱ��ʾȫ����ʾ��С������ʾ��Ӧ��
void var_display(u8 tab)
{
//���屳����ʱ
#define SELECT_NO_CHANGE_BG         WHITE   //��ǰѡ�У�����û�иı�
#define SELECT_CHANGE_BG            WHITE   //��ǰѡ�У����Ҹı��� 
#define NO_SELECT_NO_CHANGE_BG      RED     //û��ѡ�У�����û�иı䣨��ͨ�ľ���������
#define NO_SELECT_CHANGE_BG         RED     //û��ѡ�У����Ҹı���    
    
//����������ɫ
#define SELECT_NO_CHANGE            BLUE  //��ǰѡ�У�����û�иı�
#define SELECT_CHANGE               GREEN  //��ǰѡ�У����Ҹı��� 
#define NO_SELECT_NO_CHANGE         BLUE  //û��ѡ�У�����û�иı䣨��ͨ�ľ���������
#define NO_SELECT_CHANGE            GREEN  //û��ѡ�У����Ҹı���  
    
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
			//��ʾ��ǰ��ֵ���ж�ֵ�Ƿ�ı�
			if(VAR_VALUE(tab) == VAR_OLDVALUE(tab))	//ֵû�ı䣬����Ҫ����
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
			//��ʾ�ǵ�ǰ��ֵ
			if(VAR_VALUE(tab) == VAR_OLDVALUE(tab))	//ֵû�ı䣬����Ҫ����
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
        
		//��ʾ����
		LCD_Num_C(VAR_SITE(tab),VAR_VALUE(tab),Color,bkColor);

		tab++;
	}while(i--);			//tab != VAR_MAX ��ʱ��ִ��һ�ξ�����
}

//ͬ��ָ����ֵ��tab Ϊ VAR_MAX ʱ��ʾȫ��ͬ����С����ͬ����Ӧ��
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
        //��ֵ���Ƶ���Ӧ�ı���
        save_var((var_tab_e)tab,VAR_VALUE(loop_tab));
        
		//�����µ�ֵ
		//********��д
		save_var2buff((var_tab_e)tab,tempbuff);         //�ѱ���д�� tempbuff ��
        NRF_MSG_send(COM_EVENT_INFORM,tempbuff);        //��������
        do
        {
            status = NRF_MSG_send_state();		//��ȡ����״̬
        }while( status == TX_ISR_SEND);       	//��������У�����Ҫ�����ȴ��������
        
        if(TX_ISR_FAIL == status)               //����ʧ��
        {
            //��ԭֵ
            //VAR_VALUE(loop_tab) = VAR_OLDVALUE(loop_tab);           //��ԭ�ɵ�ֵ
            save_var((var_tab_e)tab,VAR_OLDVALUE(loop_tab));           //�޸ı����ı���ֵ
            //get_var((var_tab_e)tab,&(VAR_OLDVALUE(loop_tab)));      //�޸ı����ı���ֵ
            
            //����ͬ��
            return 0;
        }
        
		VAR_OLDVALUE(loop_tab) = VAR_VALUE(loop_tab);//���¾ɵ�ֵ
		
		tab++;
	}while(i--);
    
    return 1;
}




