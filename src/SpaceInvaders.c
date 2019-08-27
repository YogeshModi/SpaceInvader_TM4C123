// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the edX Lab 15
// In order for other students to play your game
// 1) You must leave the hardware configuration as defined
// 2) You must not add/remove any files from the project
// 3) You must add your code only this this C file
// I.e., if you wish to use code from sprite.c or sound.c, move that code in this file
// 4) It must compile with the 32k limit of the free Keil

// This virtual Nokia project only runs on the real board, this project cannot be simulated
// Instead of having a real Nokia, this driver sends Nokia
//   commands out the UART to TExaSdisplay
// The Nokia5110 is 48x84 black and white
// pixel LCD to display text, images, or other information.

// April 19, 2014
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
// ******* Required Hardware I/O connections*******************
// PA1, PA0 UART0 connected to PC through USB cable
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "UART.h"
#include "Draw.h"
#include "buttons.h"


// *************************** Capture image dimensions out of BMP**********
#define BUNKERW     ((unsigned char)Bunker0[18])
#define BUNKERH     ((unsigned char)Bunker0[22])
#define ENEMY30W    ((unsigned char)SmallEnemy30PointA[18])
#define ENEMY30H    ((unsigned char)SmallEnemy30PointA[22])
#define ENEMY20W    ((unsigned char)SmallEnemy20PointA[18])
#define ENEMY20H    ((unsigned char)SmallEnemy20PointA[22])
#define ENEMY10W    ((unsigned char)SmallEnemy10PointA[18])
#define ENEMY10H    ((unsigned char)SmallEnemy10PointA[22])
#define ENEMYBONUSW ((unsigned char)SmallEnemyBonus0[18])
#define ENEMYBONUSH ((unsigned char)SmallEnemyBonus0[22])
#define LASERW      ((unsigned char)Laser0[18])
#define LASERH      ((unsigned char)Laser0[22])
#define MISSILEW    ((unsigned char)Missile0[18])
#define MISSILEH    ((unsigned char)Missile0[22])
#define PLAYERW     ((unsigned char)PlayerShip0[18])
#define PLAYERH     ((unsigned char)PlayerShip0[22])


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
unsigned long TimerCount;
unsigned long score = 0;
unsigned long Semaphore;


unsigned char String[10]; // null-terminated ASCII string
unsigned long Distance;   // units 0.001 cm
unsigned long ADCdata;    // 12-bit 0 to 4095 sample
unsigned long Flag;       // 1 means valid Distance, 0 means Distance is empty
volatile unsigned long SwitchFlag = 0;

struct State {
  unsigned long x;      // x coordinate
  unsigned long y;      // y coordinate
  const unsigned char *image[2]; // two pointers to images
  long life;            // 0=dead, 1=alive
}; 

typedef struct State STyp;
typedef struct Wepon WTyp;
STyp Enemy[4];
STyp Player;
STyp wepon[2];

void Init(void){ int i;
  for(i=0;i<4;i++){
    Enemy[i].x = 20*i;
    Enemy[i].y = 10;
    Enemy[i].image[0] = SmallEnemy30PointA;
    Enemy[i].image[1] = SmallEnemy30PointB;
    Enemy[i].life = 1;
   }
		Player.x = 47;
		Player.y = 47;
		Player.image[0] = PlayerShip0;
		Player.image[1] = PlayerShip0;
		wepon[0].x = 0;
		wepon[0].y = 0;
		wepon[0].image[0] = Missile0;
		wepon[0].image[1] = Missile1;
	  wepon[0].life = 0;
	 
	  wepon[1].x = 6;
		wepon[1].y = 15;
		wepon[1].image[0] = Missile0;
		wepon[1].image[1] = Missile1;
	  wepon[1].life = 0;
		Player.life = 1;
	 score = 0;
}

unsigned long Random4(void){
  return ((Random()>>24)%4)+1;  // returns 1, 2, 3, or 4
}
unsigned long n;
void Move(void){ int i;
  for(i=0;i<4;i++){
    if(Enemy[i].x < 74){
      Enemy[i].x += 1; // move to right
    }else{
      Enemy[i].x = 0;
    }
  }
  n = Random4();
	wepon[1].y++;
	
	if(wepon[1].y>47){
		wepon[1].y = 15;
		wepon[1].x = Enemy[n].x +6;
		wepon[1].life = Enemy[n].life;
	}
	
	
}
void Fire(void){
	if(SwitchFlag==1)
		wepon[0].life = 1;
	if(wepon[0].y>7){
		wepon[0].y--;
}
  else if(SwitchFlag == 1){
		wepon[0].y = 40;
		wepon[0].life = 0;}
    
}
 
unsigned long FrameCount=0;
int count=0,delay =20;
void Draw(void){
	int i = 0;
  Nokia5110_ClearBuffer();
	//Enemy kill Detection
	for(i=0;i<4;i++){
		if(((wepon[0].x -7) == Enemy[i].x || (wepon[0].x -8) == Enemy[i].x || (wepon[0].x -6) == Enemy[i].x)  && wepon[0].y == Enemy[i].y && wepon[0].life ==1  ){
			Enemy[i].life = 0;
			score++;
			count++;
			delay = 20;
		}
		if(Enemy[i].life ==0 && delay>0)
		{
			Nokia5110_PrintBMP(Enemy[i].x, Enemy[i].y, SmallExplosion0, 0);
			delay--;
		}
		if(delay==0)
		{
			Nokia5110_PrintBMP(Enemy[i].x, Enemy[i].y, SmallExplosion1, 0);
		}
	}
	if(count ==4)
	{
		for(i=0;i<4;i++)
			Enemy[i].life =1;
		count = 0;
	}
	//player kill detection
	if((wepon[1].x -6) == Player.x && wepon[1].y == Player.y-5 && wepon[1].life==1)
	{
		Player.life = 0;
	}
	// Printing the Enemy
	for(i=0;i<4;i++)
	{
		if(Enemy[i].life == 1)
			Nokia5110_PrintBMP(Enemy[i].x, Enemy[i].y, Enemy[i].image[FrameCount], 0);
	}
	// Printing the Player
  if(Player.life > 0){
		Nokia5110_PrintBMP(Player.x, Player.y, Player.image[FrameCount], 0);
  }
	//Player wepon control
	Fire();
	if(SwitchFlag == 1 && wepon[0].life ==1){
		wepon[0].x = Player.x+7;
		Nokia5110_PrintBMP(wepon[0].x, wepon[0].y, wepon[0].image[FrameCount], 0);
	}
	
	if(wepon[1].life==1)
		Nokia5110_PrintBMP(wepon[1].x,wepon[1].y,wepon[1].image[FrameCount],0);	
		
	
  
			
  Nokia5110_DisplayBuffer();      // draw buffer
  FrameCount = (FrameCount+1)&0x01; // 0,1,0,1,...
}



// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(unsigned long period){ unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x00000020; // activate port F
  delay = SYSCTL_RCGC2_R;
	GPIO_PORTF_DIR_R |= 0x02;     // make PF1 out
  GPIO_PORTF_AMSEL_R &= ~0x02;      // no analog 
  GPIO_PORTF_PCTL_R &= ~0x00F00000; // regular function
  
  GPIO_PORTF_DR8R_R |= 0x02;    // can drive up to 8mA out
  GPIO_PORTF_AFSEL_R &= ~0x02;  // disable alt funct on PA5
  GPIO_PORTF_DEN_R |= 0x02;     // enable digital I/O on PA5
  NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
  NVIC_ST_RELOAD_R = period - 1;     // reload value(assuming 80MHz)
  NVIC_ST_CURRENT_R = 0;        // any write to current clears it
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0                
  NVIC_ST_CTRL_R = 0x00000007;  // enable with core clock and interrupts
  EnableInterrupts();
  
}
// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){ 
	GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x02;
  GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x02;
	ADCdata = ADC0_In();
	Distance = Convert(ADCdata);
	Distance = (Distance*73)/2000;
	if(Distance>66)
				Distance = 66;
	Player.x = Distance;
	//if(SwitchFlag ==1)
		


	Flag = 1;
	GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x02;
}

void GPIOPortE_Handler(void){
  GPIO_PORTE_ICR_R = 0x01;      // acknowledge flag4
  SwitchFlag = SwitchFlag+1;
	SwitchFlag = SwitchFlag%2;
	

}


int main(void){ int AnyLife = 1; int i;
  TExaS_Init(NoLCD_NoScope);  // set system clock to 80 MHz
  // you cannot use both the Scope and the virtual Nokia (both need UART0)
  Random_Init(1);
  Nokia5110_Init();
	ADC0_Init();
	SysTick_Init(114286);  // 1KHz
  Timer2_Init(1000000);
	EdgeCounter_Init(); 
  EnableInterrupts(); // virtual Nokia uses UART0 interrupts
  
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();      // draw buffer
	Init();
  while(1){
		Flag = 0;
		WaitForInterrupt();
		if(Flag==1){
			Draw();			
		}
		if(Player.life==0)
		{
			Nokia5110_Clear();
			Nokia5110_SetCursor(1, 0);
			Nokia5110_OutString("GAME OVER");
			Nokia5110_SetCursor(1, 1);
			Nokia5110_OutString("Nice try,");
			Nokia5110_SetCursor(1, 2);
			Nokia5110_OutString("Earthling!");
			Nokia5110_SetCursor(1, 3);
			Nokia5110_OutString("---Score---");
			Nokia5110_SetCursor(1, 4);
			Nokia5110_OutUDec(score);
			break;
	}
		
		
		
  }

}


// You can use this timer only if you learn how it works
void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  Semaphore = 0;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}
void Timer2A_Handler(void){ 
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
  TimerCount++;
  Move(); 
  Semaphore = 1; // trigger
}
void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
