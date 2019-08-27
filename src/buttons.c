#include "buttons.h"
#include "..//tm4c123gh6pm.h"

void EdgeCounter_Init(void){ unsigned long volatile delay;                          
  SYSCTL_RCGC2_R |= 0x00000010; // (a) activate clock for port E
	delay = SYSCTL_RCGC2_R;
  GPIO_PORTE_DIR_R &= ~0x01;    // (c) make PE0 in (built-in button)
  GPIO_PORTE_AFSEL_R &= ~0x01;  //     disable alt funct on PE0
  GPIO_PORTE_DEN_R |= 0x01;     //     enable digital I/O on PE0   
  GPIO_PORTE_PCTL_R &= ~0x0000000F; // configure PE0 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PE0
  GPIO_PORTE_PUR_R |= 0x01;     //     enable weak pull-up on PE0
  GPIO_PORTE_IS_R &= ~0x01;     // (d) PE0 is edge-sensitive
  GPIO_PORTE_IBE_R |= 0x01;    //     PE0 is  both edges
  //GPIO_PORTE_IEV_R |= 0x01;    //     PE0 Raising edge event
  GPIO_PORTE_ICR_R = 0x01;      // (e) clear flag0
  GPIO_PORTE_IM_R |= 0x01;      // (f) arm interrupt on PE0
  NVIC_PRI7_R = (NVIC_PRI1_R&0xFFFFFF0F)| 0x00000020; // (g) priority 0
  NVIC_EN0_R = 0x00000010;      // (h) enable interrupt 30 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
}