/*------------------------------------------------------------------------------
HELLO.C
Copyright 1995-2005 Keil Software, Inc.
------------------------------------------------------------------------------*/

#include <REG52.h>                /* special function register declarations   */
                                  /* for the intended 8051 derivative         */

#include <stdio.h>                /* prototype declarations for I/O functions */
#include <string.h>

#ifdef MONITOR51                         /* Debugging with Monitor-51 needs   */
char code reserve [3] _at_ 0x23;         /* space for serial interrupt if     */
#endif                                   /* Stop Exection with Serial Intr.   */
                                         /* is enabled                        */
 
int userA[9];	
int userB[9];	
int temp[9];
int counter = 0;	
int ua = 0;
int ub = 0;
unsigned i,j,k,l;
sbit LED = P0^0;
//configure pin 1 of port 0 as input
P0_1=1;
sbit switch1 = P0^1;
P0_2=1;
sbit switch2 = P0^2; 


char key[12] = {'.','t','i','e','5','R','o','n','a','l','\0'};
	
char z ;

unsigned int T0_ISR_count = 0;


void T0_ISR(void) interrupt 1 {
	counter++; //every 1 ms
	TH0 = 0xFA;
	TL0 = 0xCB;
	TF0 = 0; // Reset the interrupt request
}


void main (void) {

	#ifndef MONITOR51
    SCON  = 0x50;		        		/* SCON: mode 1, 8-bit UART, enable rcvr      */
    TMOD |= 0x29;               /* TMOD: timer 1, mode 2, 8-bit reload        */
    TH1   = 0xF3;               /* TH1:  reload value for 1200 baud @ 16MHz   */
    TR1   = 1;                  /* TR1:  timer 1 run                          */
    TI    = 1;                  /* TI:   set TI to send first char of UART    */
	#endif

	ET0 = 1;	
	EA = 1;
	TR0 = 1;
	/*--------------------------------------
	Set Timer0 for 16-bit interval timer mode.
	--------------------------------------*/
	//TMOD = (TMOD & 0xF0) | 0x09;
	while(1){
	  if(!switch1){
		//printf ("U %i s \n", (i+1));
     if(switch2)
			 i=2;
		 else i=1;
		for (  j = 0; j < 5; j++){
			
			printf ("User %i s w %i \n", i ,j+1);

				for( k = 0; k< 10; k++){
								
					z = _getkey();
				
					if(z == key[k] ){
					
						if(k > 0){
							temp[k-1] = counter;
							printf ("N.\n");
						}
						
						counter = 0;
						TH0 = 0xFA;
						TL0 = 0xCB;
					}
					else{
						k = -1;
						printf ("ag \n");
					}
				}
				
				for(l = 0; l<9; l++){
					
					if(i == 1){
						userA[l] += temp[l];
					}
					else{
						userB[l] += temp[l];
					}
				}
			}
		
	}
		// Test
	if(switch1){
		//moved this to test to make sure both users already added their training
		for( k = 0; k< 9; k++){
			userA[k] = userA[k]/5;
			userB[k] = userB[k]/5;
		}
		
		for(j = 0; j< 8; j++){
			printf ("test \n");

			for( k = 0; k< 10; k++){
				
				z = _getkey();
				
				if(z == key[k] ){
					
					if(k > 0){
							
						temp[k-1] = counter;
						printf ("N.\n");
					}
					counter = 0;
					TH0 = 0xFA;
					TL0 = 0xCB;
				}
				else{
					k = -1;
					printf ("ag \n");
				}
			}
			
			for( k = 0; k <9; k++){
				ua += ((userA[k]-temp[k])*(userA[k]-temp[k]));
				ub += ((userB[k]-temp[k])*(userB[k]-temp[k]));
			}
			
			if(ua < ub){
				/*LED =! LED;
				LED =! LED;*/
			}
			else{
				/*LED =! LED;
				LED =! LED;
				LED =! LED;
				LED =! LED;*/
			}
			
		}
	} 
	}
		
}