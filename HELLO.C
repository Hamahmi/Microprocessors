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
unsigned char xdata serial_input_buffer [] ="asfasfasf"; 
																 
unsigned i,j,k,l;
char key[12] = {'.','t','i','e','5','R','o','n','a','l','\0'};
	
char x ;
char z ;

unsigned int T0_ISR_count = 0;


void T0_ISR(void) interrupt 1 {
	T0_ISR_count++;
	TF0 = 0; // Reset the interrupt request
}


int main (void) {

	#ifndef MONITOR51
    SCON  = 0x50;		        /* SCON: mode 1, 8-bit UART, enable rcvr      */
    TMOD |= 0x20;               /* TMOD: timer 1, mode 2, 8-bit reload        */
    TH1   = 0xF3;                /* TH1:  reload value for 1200 baud @ 16MHz   */
    TR1   = 1;                  /* TR1:  timer 1 run                          */
    TI    = 1;                  /* TI:   set TI to send first char of UART    */
	#endif

	ET0 = 1;	
	EA = 1;
	TR0 = 1;
	/*--------------------------------------
	Set Timer0 for 16-bit interval timer mode.
	--------------------------------------*/
	TMOD = (TMOD & 0xF0) | 0x09;
	
	for (i = 0; i < 2; i++){
		
		printf ("User %i please start \n", (i+1));

		for (  j = 0; j < 5; j++){
			
			printf ("User %i please start entering word %i \n", (i+1),j+1);

				for( k = 0; k< 10; k++){
								
					z = _getkey();
					printf (" %c , %c , %i %\n" , z , key[k] , k);
				
					if(z == key[k] ){
					
						if(k > 0){

							temp[k] = (((unsigned long)( ((unsigned int)TH0 << 8) | TL0 | ((unsigned long)T0_ISR_count << 16))/3)/1000);
							printf ("Next please.\n");
						}
						
						T0_ISR_count = 0;
						TH0 = 0;
						TL0 = 0;
					}
					else{
						
						k = -1;
						printf ("User %i please write word %i again from the beginning \n", (i+1),j+1);
					}
				}
				
				//key[0] = '.';
				//key[1] = 't';
				for(l = 0; l<9; l++){
					
					if(i == 0){
						userA[l] += temp[l];
					}
					else{
						userB[l] += temp[l];
					}
				}
			}
		}
		return 0;
}


