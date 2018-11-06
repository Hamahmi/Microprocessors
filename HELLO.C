/*------------------------------------------------------------------------------
Copyright 2018 Hardwired, Inc.
------------------------------------------------------------------------------*/

// Includes

#include <REG52.h>  									/* special function register declarations 			*/
																			/* for the intended 8051 derivative 						*/
#include <stdio.h> 										/* prototype declarations for I/O functions 		*/
#include <string.h>

#ifdef MONITOR51 											/* debugging with Monitor-51 needs space for 		*/
	char code reserve [3] at 0x23; 			/* serial interrupt if Stop Exection with Serial*/
#endif 																/* Intr. is enabled 														*/

// End of Includes -------------------------------------------------------------

// Declarations
unsigned int userA[9];	// 1st User training
unsigned int userB[9];	// 2nd user training
unsigned int temp[9];  // test data

unsigned int counter;	// timer, increments every 1ms
unsigned int ua;     // 1st user sum
unsigned int ub;			// 2nd user sum
unsigned i,j,k,l; // iterators

sbit LED = P0^0;  // output, flashes once for 1st user, twice for the 2nd

P0_1=1;																/* configure pin 1 of port 0 as input 				*/
sbit switch1 = P0^1;		// 1 training , 0 test

P0_2=1;
sbit switch2 = P0^2;    // 1 user1 , 0 user2

char key[11] = {'.','t','i','e','5','R','o','n','a','l','\0'};

char z ;    // the key from user 

// End of Declarations -------------------------------------------------------------


// Methods

void setTimer(){
	
	TH0 = 0xFA;
	TL0 = 0xCB;
}

void resetCounter(){
	
	counter = 0;
	setTimer();
}

void T0_ISR(void) interrupt 1 {

	counter++; //every 1 ms
	setTimer();
	TF0 = 0; // Reset the interrupt request
}

void getWord(){
	
	for( k = 0; k< 10; k++){
						
		z = _getkey();
		if(z == key[k] ){ // the char input is correct
							
			if(k > 0){
								
				temp[k-1] = counter;
				printf ("N\n");
			}
							
			resetCounter();
		}
		else{						// the char is wrong, ask the user to start the word again
				
			k = -1;
			printf ("ag \n");
		}
	}
}


void main (void) {

	#ifndef MONITOR51
		SCON = 0x50;		 		/* SCON: mode 1, 8-bit UART, enable rcvr  */
		TMOD |= 0x29; 			/* TMOD: timer 1, mode 2, 8-bit reload    */
		TH1 = 0xF3; 				/* TH1: reload value for F3 baud @ 11 MHz */
		TR1 = 1; 						/* TR1: timer 1 run 											*/
		TI = 1; 						/* TI: set TI to send first char of UART  */
	#endif

	ET0 = 1;	
	EA = 1;
	TR0 = 1;

	// Start of the classifier
	
	while(1){
		
		// Train
		
		if(switch1){ // switch1 == 1, Training
			
			if(switch2)  // switch2 == 1, user1
				i=1;
			else 			 // switch2 == 0, user2
				i=2;
			
			for ( j = 0; j < 2; j++){  // each user inputs the word 5 times

				printf ("Usr %i wrd %i \n", i ,j+1);			

				getWord();

				for(l = 0; l<9; l++){
					
					if(i == 1)
						userA[l] += temp[l];
					else
						userB[l] += temp[l];
				}
			}
		}

		// Test

		if(!switch1){ // switch1 == 0, Testing

			//moved this to test to make sure both users already added their training

			for( k = 0; k< 9; k++){
				
				userA[k] = userA[k]/5;
				userB[k] = userB[k]/5;
			}

				
				printf ("tst \n");
				
				getWord();
				
				ua = 0;
				ub = 0;
				for( k = 0; k <9; k++){
					ua += ((userA[k]-temp[k])*(userA[k]-temp[k]));
					ub += ((userB[k]-temp[k])*(userB[k]-temp[k]));
				}
				
				if(ua < ub){
					resetCounter();
					LED =! LED;
					while(counter < 500);
					LED =! LED;
				}
				else{
				resetCounter();
					LED =! LED;
					while(counter < 500);
					LED =! LED;
					while(counter < 1000);
					LED =! LED;
					while(counter < 1500);
					LED =! LED;
				}
			}
		}
	}
