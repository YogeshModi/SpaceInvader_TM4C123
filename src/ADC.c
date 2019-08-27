// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result. 
// Daniel Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "ADC.h"
#include "..//tm4c123gh6pm.h"

// This initialization function sets up the ADC 
// Max sample rate: <=125,000 samples/second
// SS3 triggering event: software trigger
// SS3 1st sample source:  channel 1
// SS3 interrupts: enabled but not promoted to controller
void ADC0_Init(void){ volatile unsigned long delay;
	  SYSCTL_RCGC2_R |= 0x00000010;   // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;         //    allow time for clock to stabilize
  GPIO_PORTE_DIR_R &= ~0x04;      // 2) make PE2 input
  GPIO_PORTE_AFSEL_R |= 0x04;     // 3) enable alternate function on PE2
  GPIO_PORTE_DEN_R &= ~0x04;      // 4) disable digital I/O on PE2
  GPIO_PORTE_AMSEL_R |= 0x04;     // 5) enable analog function on PE2
  SYSCTL_RCGC0_R |= 0x00010000;   // 6) activate ADC0
  delay = SYSCTL_RCGC2_R;        
  SYSCTL_RCGC0_R &= ~0x00000300;  // 7) configure for 125K
  ADC0_SSPRI_R = 0x0123;          // 8) Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;       // 11) clear SS3 field
  ADC0_SSMUX3_R += 1;             //    set channel Ain1 (PE2)
  ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC0_ACTSS_R |= 0x0008;         // 13) enable sample sequencer 3
 
}

	unsigned char Temp[4];
//-----------------------ConvertUDec-----------------------
// Converts a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: store the conversion in global variable String[10]
// Fixed format 4 digits, one space after, null termination
// Examples
//    4 to "   4 "  
//   31 to "  31 " 
//  102 to " 102 " 
// 2210 to "2210 "
//10000 to "**** "  any value larger than 9999 converted to "**** "

void ConvertUDec(unsigned long n,char* Strin){
// as part of Lab 11 implement this function
unsigned cnt=0,i;

char buffer[11];
unsigned long nn = n;
if(n<10000){
  do{
    buffer[cnt] = n%10;// digit
    n = n/10;
    cnt++;
  }
  while(n);// repeat until n==0

  for(i=0; i<cnt; i++){
    Strin[cnt-i-1] = buffer[i]+'0';
  }
  
	if(nn<10)
	{
		//UART_OutChar('A');
		Temp[0] = Strin[0];
		for(i=0;i<3;i++)
			Strin[i] = ' ';
		Strin[3] = Temp[0];
		Strin[4] = ' ';
		Strin[5] = '\0';
	}
	else if(nn<100 && nn>=10)
	{
		//UART_OutChar('B');
		Temp[0] = Strin[0];
		Temp[1] = Strin[1];
		for(i=0;i<2;i++)
			Strin[i] = ' ';
		Strin[2] = Temp[0];
		Strin[3] = Temp[1];
		Strin[4] = ' ';
    Strin[5] = '\0';
	}
	else if(nn<1000 && nn>=100)
	{
		//UART_OutChar('C');
		Temp[0] = Strin[0];
		Temp[1] = Strin[1];
		Temp[2] = Strin[2];
		Strin[0] = ' ' ;
		Strin[1] = Temp[0];
		Strin[2] = Temp[1];
		Strin[3] = Temp[2];
		Strin[4] = ' ';
    Strin[5] = '\0';
	}
	else if(nn>1000)
	{
		Strin[4] = ' ';
		Strin[5] = '\0';

	}
	
}
	else
	{
		//UART_OutChar('D');
		for(i=0;i<4;i++)
			Strin[i] = '*';
		Strin[4] = ' ';
		Strin[5] = '\0';
		
	}
  for(i=0;i<cnt;i++)
		buffer[i] = 0;
}
	


//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void ConvertDistance(unsigned long n,char* Strin){
// as part of Lab 11 implement this function
	unsigned cnt=0,i;

char buffer[5];
unsigned long nn = n;
if(n<10000){
  do{
    buffer[cnt] = n%10;// digit
    n = n/10;
    cnt++;
  }
  while(n);// repeat until n==0

  for(i=0; i<cnt; i++){
    Strin[cnt-i-1] = buffer[i]+'0';
  }
  
	if(nn<10)
	{
		//UART_OutChar('A');
		Temp[0] = Strin[0];
		for(i=0;i<4;i++)
			Strin[i] = '0';
		Strin[1] = '.';
		Strin[4] = Temp[0];
		Strin[5] = ' ';
	  Strin[6] = 'c';
		Strin[7] = 'm';
	}
	else if(nn<100 && nn>=10)
	{
		//UART_OutChar('B');
		Temp[0] = Strin[0];
		Temp[1] = Strin[1];
		for(i=0;i<3;i++)
			Strin[i] = '0';
		Strin[1] = '.';
		Strin[3] = Temp[0];
		Strin[4] = Temp[1];
		Strin[5] = ' ';
	  Strin[6] = 'c';
		Strin[7] = 'm';
	}
	else if(nn<1000 && nn>=100)
	{
		//UART_OutChar('C');
		for(i=0;i<3;i++)
			Temp[i] = Strin[i];
		Strin[0] = '0' ;
		Strin[1] = '.';
		for(i=0;i<3;i++)
			Strin[i+2] = Temp[i];
		Strin[5] = ' ';
	  Strin[6] = 'c';
		Strin[7] = 'm';
	}
	else if(nn>1000)
	{
		for(i=0;i<3;i++)
			Temp[i] = Strin[i+1];
		Strin[1] = '.';
		for(i=0;i<3;i++)
			Strin[i+2] = Temp[i];
		Strin[5] = ' ';
	  Strin[6] = 'c';
		Strin[7] = 'm';
	}
	
}
	else
	{
		//UART_OutChar('D');
		for(i=0;i<5;i++)
			Strin[i] = '*';
	Strin[1] = '.';
	Strin[5] = ' ';
	Strin[6] = 'c';
  Strin[7] = 'm';
	}
	
	
	for(i=0;i<cnt;i++)
		buffer[i] = 0;
}


//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC 
unsigned long result;
unsigned long ADC0_In(void){  
	ADC0_PSSI_R = 0x0008;            // 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
  result = ADC0_SSFIFO3_R&0xFFF;   // 3) read result
  ADC0_ISC_R = 0x0008;             // 4) acknowledge completion
  return result;
	}

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample){
  unsigned long x = (sample*2000)/4095;
	if(x%2 != 0)
		x = x+1;
	
  return x;  // replace this line with real code
}
