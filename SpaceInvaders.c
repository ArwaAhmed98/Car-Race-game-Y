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

// April 10, 2014
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

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "time.h"
#include "stdlib.h"

void PortF_Init(void);
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
unsigned long TimerCount;
unsigned long Semaphore;
// *************************** Images ***************************
// enemy ship that starts at the top of the screen (arms/mouth closed)
// width=16 x height=10

const unsigned char car[] ={
 0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF,
 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x0F, 0xFF, 0xF0, 0x00, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xF0,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x0F, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF,

};
const unsigned char SmallEnemy20PointA[] = {
 0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0xFF, 0xF0, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF,
 0xF0, 0x0F, 0xF0, 0x0F, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
 /************Another names for my drawings and getting the dimensions*****************************/
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
 

#define carW        ((unsigned char)car[18])
#define carH        ((unsigned char)car[22])
#define  enemyWidth             ( (unsigned char) SmallEnemy20PointA[18]);	
#define  enemyHeight            ( (unsigned char) SmallEnemy20PointA[22]);
#define true 1
#define false 0
//______________________________________________________________________________________________
//Some Global Declarations 
//_______________________________________________________________________________
/////////////////////////////////////////////////////////
//Screen_width=84,Screen_Hieight=47;
int Screen_divisions[3]={28,11,45}; // the coordinates of Y-axis that the Car is moving on 
int flagSW1 , flagSW2 , points; //  flags for the 2 switchs to check that SW1 & SW2 is pressed only once
//points that have collected after the game has ended
int caX,caY ; // Coordinates of the car to move it 
int eneNEWY , enefrontX,eneX2; 
//// Coordinates of the enemy  place  and motion 
////____________________________________________________________________________
int main(void){
	
	PortF_Init();
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
  Random_Init(1);
  Nokia5110_Init();
	////////////////////////////////////////////////////
	flagSW1=0,flagSW2=0,points=0,caX=0,caY=28;
	////////////////////////////////////////////////////////
	points=0;
	
	// ******************** initializations ***************************** //
	PortF_Init();
	caY=28;
	enefrontX = 84- enemyWidth;
	eneX2 = 84- enemyWidth;
	eneNEWY = rand()%3;
	///////________________________________________________
	//***********************START-UP SCREEN ********************//////////
	Nokia5110_Clear();
	Nokia5110_SetCursor(3, 0);
  Nokia5110_OutString("WELCOME");
  Nokia5110_SetCursor(3, 2);
  Nokia5110_OutString("For");
  Nokia5110_SetCursor(0, 4);
  Nokia5110_OutString("Care Racing");
  Nokia5110_SetCursor(1, 5);
  Nokia5110_OutString("Press SW2");
// SW1 is gonna move the car upward 
// SW2  is gonna move the car downward
  
	while(true){
		if(!((GPIO_PORTF_DATA_R&0x1))) // Check if SW2 is pressed , it sends 0 
//			Since  !0=1  and *****if(1)******  break to the game
			break;
  }
	Nokia5110_ClearBuffer();
	Nokia5110_PrintBMP(0, 28, car, 0);
	Nokia5110_DisplayBuffer();
	Delay100ms(1000);
	while(true){
		Nokia5110_ClearBuffer();		
		
								// ************** Motion of the CAR  ************** //
		 // WE use flags to check that the switch is pressed ONLY ONCE 
		if(!(((GPIO_PORTF_DATA_R&(1<<4))==0) && ((GPIO_PORTF_DATA_R&0x1)==0  ))){
			if((GPIO_PORTF_DATA_R&(1<<4)) == 0){
				if((caY > 11) && (flagSW1==0)){
					caY -= 17;
					flagSW1 = 1;
				}
			}
			else{
				flagSW1 = 0;
			}

			if(((GPIO_PORTF_DATA_R&0x1) == 0)){ // check SW2 is pressed ?
				if((caY < 45) && (flagSW2==0)){ // if the car is found between 0 to 28 coordinates of Y axis
					// it will statisfy that condition
					caY += 17;
					flagSW2 = 1;
				}
			}
			else{
				flagSW2 = 0;
			}
		}
		
		Nokia5110_PrintBMP(0,caY , car, 0);
			//Delay100ms(20);
		//	Nokia5110_DisplayBuffer();
		

		if(enefrontX != 0){
			Nokia5110_PrintBMP(enefrontX, Screen_divisions[eneNEWY], SmallEnemy20PointA, 0);
			enefrontX--;
		}/*
		while(enefrontX--){
		Nokia5110_PrintBMP(enefrontX, Screen_divisions[eneNEWY], SmallEnemy20PointA, 0);
		}*/
		
		if(enefrontX == 0){
			
			if(eneX2 == 0){
				points++;					//number of passed enemys
				eneNEWY = rand()%3;
				eneX2 = 84- enemyWidth;
			}
			Nokia5110_PrintBMP(eneX2, Screen_divisions[eneNEWY], SmallEnemy20PointA, 0);
		}
		if((eneX2 ==  carW) && (Screen_divisions[eneNEWY] == caY)){
			break;
		}
		eneX2--;
				
								//***************** Screen displaying ********************//
		Nokia5110_DisplayBuffer();
		Delay100ms(8);
	}
	
//Delay100ms(10);				
////////////////////////////////////GAME --- OVER ///////////////////////////////
  Nokia5110_Clear();
  Nokia5110_SetCursor(1, 1);
  Nokia5110_OutString("GAME OVER");
	 Delay100ms(1);
	Nokia5110_Clear();
  Nokia5110_SetCursor(1, 2);
  Nokia5110_OutString("Nice try,");
	 //Delay100ms(20);
	Nokia5110_Clear();
  Nokia5110_SetCursor(1, 3);
  Nokia5110_OutString("Your points");
  Nokia5110_SetCursor(2, 4);
  Nokia5110_OutUDec(points);


}
	

void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
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
  Semaphore = 1; // trigger
}
void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240/800;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
