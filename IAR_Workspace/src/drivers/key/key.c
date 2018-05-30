/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���       ��key.c
 * ����         ��key��������
 *
 * ʵ��ƽ̨     ��Ұ��kinetis������
 * ��汾       ��
 * Ƕ��ϵͳ     ��
 *
 * ����         ��Ұ��Ƕ��ʽ����������
 * �Ա���       ��http://firestm32.taobao.com
 * ����֧����̳ ��http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/	


#include "key.h"


PTxn KEY_PTxn[KEY_MAX] = {PTD8,PTD9,PTD10,PTD11,PTE6,PTE7};

/*************************************************************************
*                             Ұ��Ƕ��ʽ����������
*
*  �������ƣ�key_init
*  ����˵������ʼ��key�˿ڣ�����IO��Ϊ���뷽������
*  ����˵������������
*  �������أ���
*  �޸�ʱ�䣺2012-10-7   �Ѳ���
*  ��    ע��
*************************************************************************/
void    KEY_init(KEY_e key)                                               // KEY��ʼ�� ����
{
    gpio_init((PORTx)(KEY_PTxn[key]>>5),KEY_PTxn[key] & 0x1F,GPI_UP,0);
} 

/*************************************************************************
*                             Ұ��Ƕ��ʽ����������
*
*  �������ƣ�key_init
*  ����˵������ʼ��key�˿ڣ�����IO��Ϊ���뷽������
*  ����˵������������
*  �������أ���
*  �޸�ʱ�䣺2012-11-18   �Ѳ���
*  ��    ע��
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
*                             Ұ��Ƕ��ʽ����������
*
*  �������ƣ�key_check
*  ����˵�������key״̬
*  ����˵������������
*  �������أ�����״̬
*  �޸�ʱ�䣺2012-10-7   �Ѳ���
*  ��    ע��
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



    
#define KEY_MSG_FIFO_SIZE   20      //��� 255��������Ҫ�޸�key_msg_front/key_msg_rear����

typedef enum
{
    KEY_MSG_EMPTY,      //û�а�����Ϣ
    KEY_MSG_NORMAL,     //�������а�����Ϣ��������
    KEY_MSG_FULL,       //������Ϣ��
}key_msg_e;


KEY_MSG_t       key_msg[KEY_MSG_FIFO_SIZE];
volatile u8     key_msg_front = 0,key_msg_rear = 0; //����FIFO��ָ��
volatile u8     key_msg_flag = KEY_MSG_EMPTY;


void send_key_msg(KEY_MSG_t keymsg)
{
    //������FIFO��
    if(key_msg_flag == KEY_MSG_FULL)
    {
        //����ֱ�Ӳ�����
        return ;
    }
    key_msg[key_msg_rear].key=keymsg.key;
    key_msg[key_msg_rear].status=keymsg.status;
    
    key_msg_rear++;
    
    if(key_msg_rear >= KEY_MSG_FIFO_SIZE)
    {
        key_msg_rear=0;                         //��ͷ��ʼ
    }
    
    if(key_msg_rear == key_msg_front)                   //׷��ƨ���ˣ�����
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
    if(key_msg_flag == KEY_MSG_EMPTY)   //�գ�ֱ�ӷ���0
    {
        return 0;
    }
    keymsg->key = key_msg[key_msg_front].key;
    keymsg->status = key_msg[key_msg_front].status;
    
    key_msg_front++;                //���ڷǿգ����Կ���ֱ�ӳ�����
    
    if(key_msg_front >= KEY_MSG_FIFO_SIZE)
    {
        key_msg_front=0;            //��ͷ��ʼ
    }
    
    if(key_msg_front == key_msg_rear)       //׷��ƨ���ˣ����ն��п�
    {
        key_msg_flag = KEY_MSG_EMPTY;
    }
    
    return 1;
}

/*************************************************************************
*                             Ұ��Ƕ��ʽ����������
*
*  �������ƣ�key_IRQHandler
*  ����˵������ʱ���key״̬
*  ����˵������������
*  �������أ�����״̬
*  �޸�ʱ�䣺2012-10-7   �Ѳ���
*  ��    ע��
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
                send_key_msg(keymsg);           //��������
            }
            keytime[keynum] = 0;
        }
    }
}



