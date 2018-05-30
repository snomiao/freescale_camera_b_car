/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���       ��isr.h
 * ����         ������ͷС���ж϶�����
 * ����         �������
**********************************************************************************/



#ifndef __ISR_H
#define __ISR_H 1

#include  "include.h"

/*                          ���¶����ж�������
 *  ��ȡ��Ĭ�ϵ��ж�����Ԫ�غ궨��       #undef  VECTOR_xxx
 *  �����¶��嵽�Լ���д���жϺ���       #define VECTOR_xxx    xxx_IRQHandler
 *  ���磺
 *       #undef  VECTOR_003
 *       #define VECTOR_003    HardFault_Handler    ���¶���Ӳ���Ϸ��жϷ�����
 *
 *       extren void  HardFault_Handler(void);      ����������Ȼ����isr.c�ﶨ��
 */
#undef  VECTOR_084
#define VECTOR_084    PIT0_IRQHandler     	//���¶���84���ж�ΪPIT0_IRQHandler�ж�

#undef  VECTOR_085
#define VECTOR_085    PIT1_IRQHandler     	//���¶���85���ж�ΪPIT1_IRQHandler�ж�

#undef  VECTOR_086
#define VECTOR_086    PIT2_IRQHandler     	//���¶���86���ж�ΪPIT1_IRQHandler�ж�
   
#undef  VECTOR_101
#define VECTOR_101   LPT_Handler

#undef  VECTOR_105                        	//ȡ���жϺŵĶ���
#define VECTOR_105    PORTC_IRQHandler    	//PORTc�ж�

#undef  VECTOR_016							//ȡ���жϺŵĶ���
#define VECTOR_016   DMA0_IRQHandler		//DMA�ж�

#undef  VECTOR_069                        //Ҫ��ȡ���ˣ���Ϊ��vectors.h��Ĭ���Ƕ���Ϊ default_isr  
#define VECTOR_069    USART4_IRQHandler   //���¶���67���жϵ�ISR��UART1��Single interrupt vector for UART status sources

//�����жϺ��� vectors.h ���

extern void PORTC_IRQHandler();			//PORTA�жϷ�����
extern void DMA0_IRQHandler();				//DMA�жϷ�����
extern void LPT_Handler(void);				//LPT��ʱ�жϷ�����
extern void PIT0_IRQHandler();            	//PIT0 ��ʱ�жϷ�����
extern void PIT1_IRQHandler(void);         //PIT1 ��ʱ�жϷ�����
extern void PIT2_IRQHandler(void);         //PIT1 ��ʱ�жϷ�����
extern void USART4_IRQHandler(void);       // ���ڽ����ж�

#endif  //__ISR_H

/* End of "isr.h" */
