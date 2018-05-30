/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名       ：isr.h
 * 描述         ：摄像头小车中断都在这
 * 作者         ：沈泽皓
**********************************************************************************/



#ifndef __ISR_H
#define __ISR_H 1

#include  "include.h"

/*                          重新定义中断向量表
 *  先取消默认的中断向量元素宏定义       #undef  VECTOR_xxx
 *  在重新定义到自己编写的中断函数       #define VECTOR_xxx    xxx_IRQHandler
 *  例如：
 *       #undef  VECTOR_003
 *       #define VECTOR_003    HardFault_Handler    重新定义硬件上访中断服务函数
 *
 *       extren void  HardFault_Handler(void);      声明函数，然后在isr.c里定义
 */
#undef  VECTOR_084
#define VECTOR_084    PIT0_IRQHandler     	//重新定义84号中断为PIT0_IRQHandler中断

#undef  VECTOR_085
#define VECTOR_085    PIT1_IRQHandler     	//重新定义85号中断为PIT1_IRQHandler中断

#undef  VECTOR_086
#define VECTOR_086    PIT2_IRQHandler     	//重新定义86号中断为PIT1_IRQHandler中断
   
#undef  VECTOR_101
#define VECTOR_101   LPT_Handler

#undef  VECTOR_105                        	//取消中断号的定义
#define VECTOR_105    PORTC_IRQHandler    	//PORTc中断

#undef  VECTOR_016							//取消中断号的定义
#define VECTOR_016   DMA0_IRQHandler		//DMA中断

#undef  VECTOR_069                        //要先取消了，因为在vectors.h里默认是定义为 default_isr  
#define VECTOR_069    USART4_IRQHandler   //重新定义67号中断的ISR：UART1：Single interrupt vector for UART status sources

//具体中断号在 vectors.h 里查

extern void PORTC_IRQHandler();			//PORTA中断服务函数
extern void DMA0_IRQHandler();				//DMA中断服务函数
extern void LPT_Handler(void);				//LPT定时中断服务函数
extern void PIT0_IRQHandler();            	//PIT0 定时中断服务函数
extern void PIT1_IRQHandler(void);         //PIT1 定时中断服务函数
extern void PIT2_IRQHandler(void);         //PIT1 定时中断服务函数
extern void USART4_IRQHandler(void);       // 串口接收中断

#endif  //__ISR_H

/* End of "isr.h" */
