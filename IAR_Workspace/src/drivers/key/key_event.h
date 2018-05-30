#ifndef _KEY_EVNET_H_
#define _KEY_EVNET_H_

#include "common.h"
#include "LCD.h"
#include "key.h"

#define	VAR_SELECT_HOLD_OFFSET		((VAR_MAX+4-1)/4)		//���Ͽ���ʱ��ÿ���л�ƫ������Ŀǰ����Ϊ����������1/4.���޸ĳ�������
#define	VAR_VALUE_HOLE_OFFSET		10


//�����ķ��������
typedef enum
{
    //�����ı��
    
    /* 8λ���� */ 
    VAR1, 
    VAR2,
    VAR_8BIT = VAR2, //8λ�����Ľ������
   
    /* 16λ���� */
    VAR3,
    VAR4,
    VAR_16BIT  = VAR4,          //16λ�����Ľ������
    
    /* 32λ���� */
    VAR5,
    VAR6,
    VAR_32BIT  = VAR6,          //32λ�����Ľ������
    
    VAR_MAX,        //������Ŀ
}var_tab_e;

//extern u32 * var_addr[VAR_MAX];                  //��������ĵ�ַ��ͬ����ʱ����num_info���ֵ��ֵ����Ӧ�ı���

//extern ui_var_info_t  num_info[VAR_MAX];        //�����������ʱ�ı�ֵ


typedef enum
{
	VAR_NEXT,			//��һ��
	VAR_PREV,			//��һ��
	VAR_NEXT_HOLD,		//���£�ƫ��Ϊ��VAR_SELECT_HOLD_OFFSET
	VAR_PREV_HOLD,		//���ϣ�ƫ��Ϊ��VAR_SELECT_HOLD_OFFSET
	
	VAR_ADD,			//��1
	VAR_SUB,			//��1
	VAR_ADD_HOLD,		//��ӣ�ƫ��Ϊ��VAR_VALUE_HOLE_OFFSET
	VAR_SUB_HOLD,		//�����ƫ��Ϊ��VAR_VALUE_HOLE_OFFSET
	
	VAR_OK,				//ȷ��
	VAR_CANCEL,			//ȡ��
	
	VAR_EVENT_MAX,
}ui_var_event_e;


void key_event_init();
void deal_key_event(void);
void save_var(var_tab_e var_tal,u32 var_data);
void get_var(var_tab_e var_tal,u32 *var_data);
void var_display(u8 tab);                //��ʾָ����ֵ��tab Ϊ VAR_NUM ʱ��ʾȫ����ʾ��С������ʾ��Ӧ��











#endif  //_KEY_EVNET_H_