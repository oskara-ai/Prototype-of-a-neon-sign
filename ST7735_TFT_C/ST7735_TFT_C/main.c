/* The code below is used to read and send the user input from the terminal onto the LED screen.
Also included in the code is the text formatting. In this case, the text must be displayed on the screen
centralized with only one word per row.
All the necessary libraries are included below with comments telling what they're used for.
We used some definitions for some of the variables used in the code. These definitions are also commented below.
Most of the functions are different .c and .h files from the project. These files define the functions for the screen,
EERPOM, ATMEGA2560 and the text formatting.

Author	: Oskar Ala-Mäyry

*/
//--------------------------------------------------------------------
// Define Processor and data transfer macros
//--------------------------------------------------------------------
#define F_CPU 16000000L
#define BAUD 9600
//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <avr/io.h>				// include for DFROBOT mega pins
#include <avr/interrupt.h>		// include for enabling interrupts
#include <util/setbaud.h>		// include baud rate for UART0
#include <string.h>				// include for memset 
//--------------------------------------------------------------------
// Include TFT-screen headers
//--------------------------------------------------------------------
#include "./include/spi.h"
#include "./include/st7735.h"
#include "./include/st7735_gfx.h"
#include "./include/st7735_font.h"
#include "./fonts/free_sans.h"
//--------------------------------------------------------------------
// Command handler functions
//--------------------------------------------------------------------
#define BUFFERSIZE				0xFF	// define max size for buffer array
#define LINE					28		// define size of line for display 
#define EEPROMADDRES			0x0000	// define address for program where eeprom start writing
//--------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------
uint8_t arrayBuff[BUFFERSIZE];			//
volatile uint16_t	gloCounter;			//
volatile uint16_t	gloLinecounter;		//
volatile uint8_t	ProgFlag;			//
//--------------------------------------------------------------------
// UART functions
//--------------------------------------------------------------------
// initialize UART0
//--------------------------------------------------------------------
void UART0_init (void)
{
	UBRR0H = UBRRH_VALUE;							// baud rate high value (8 bits set in setbaud.h)
	UBRR0L = UBRRL_VALUE;							// low value
	UCSR0B |= (1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0);	// enable receiver and transmitter
	UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01);				// 8 bit data format, (default: 1 stop bit, no parity)
}
//--------------------------------------------------------------------
// receive UART0 data
//--------------------------------------------------------------------
uint8_t UART0_receive (void)
{
	while (!((UCSR0A) & (1<<RXC0)));   // wait while data is being received i.e. until bit RXC1 in register UCSR2A becomes  1
	return UDR0;                       // return 8-bit data
}
//--------------------------------------------------------------------
// function for programming text
//--------------------------------------------------------------------
void program(){

st7735_draw_text(25, 60,arrayBuff, &FreeSans, 1, ST7735_COLOR_WHITE);		// draw text to screen

	if ((arrayBuff[gloCounter-1] == ' ') || (gloLinecounter % LINE == 0)){	// if user inputs space or Linecounter goes over LINE size start next page
			arrayBuff[gloCounter-1] = '\n';									// space char is overridden whit line break
			gloLinecounter = 0;												// linecounter is set to 0 for next char
	}
	else if (arrayBuff[gloCounter-1] == '\r'){								// if user press Enter programming ends
		ProgFlag = 0;														// flag is unset
		cli();																// disable global interrupt flag just to be sure
		st7735_fill_rect(0, 0, 160, 128, ST7735_COLOR_BLACK);				// screen is filled black
		spi_write_eeprom(EEPROMADDRES, arrayBuff, gloCounter);				// write data to eeprom
		gloCounter = 0;														// set counter to 0 
		
		UCSR0B &= ~(1<<RXEN0);												// UART0 register is disabled for securing clearing of it 
		UCSR0B |= (1<<RXEN0);												// UART0 register is enabled back on
		sei();																//enable global interrupt flag
	}
}
//--------------------------------------------------------------------
int main(void) 
//--------------------------------------------------------------------
{
	uint8_t y;													
	uint8_t cWordcount = 0;										// counter for words
	
    spi_init();													// set spi transferring working
    st7735_init();												// set screen working
	UART0_init();												// set UART0_init function to work for serial communication 
	
	st7735_set_bl();											// set back light on 
	//--------------------------------------------------------------------
	// set starting screen
	//--------------------------------------------------------------------
	st7735_set_orientation(ST7735_LANDSCAPE_INV);				// set screen orientation to landscape
	st7735_fill_rect(0, 0, 160, 128, ST7735_COLOR_BLACK);		// fill screen whit black
	st7735_draw_text(25, 60, " Loading ", &FreeSans, 1, ST7735_COLOR_WHITE); // set Loading text
	//--------------------------------------------------------------------
	memset(arrayBuff,0,sizeof(arrayBuff));						// formate memory of array
	spi_read_eeprom(EEPROMADDRES,arrayBuff,BUFSIZE);			// start reading from eeprom
	
	sei();														//enable global interrupt flag
		
	while(1) {
		
		switch(ProgFlag){
			case 0:												// case for scrolling text
				
			for (y = 128; y > (-cWordcount); y -= 1){			// start scrolling up on screen
				if (ProgFlag == 1){								// if progFlag is 1 program is in the programming mode
					st7735_fill_rect(0, 0, 160, 128, ST7735_COLOR_BLACK); // fill screen 
					break;
				}
				else{											//
					st7735_draw_text(25, y, arrayBuff, &FreeSans, 1,ST7735_COLOR_WHITE); // write received text
					st7735_draw_text(25, y, arrayBuff, &FreeSans, 1,ST7735_COLOR_BLACK); // write received text
				}
			}
				break;
			
			case 1:												// programming mode
				program();										// start program function
				break;
				
			default:
				break;
		
			} // end case
	} // end while
} // end main
//--------------------------------------------------------------------
// function for ISR interrupt handler
//--------------------------------------------------------------------
ISR(USART0_RX_vect)
{
	ProgFlag = 1;												// set flag to 1
	
	char rxData =UDR0;											// read data
	
	if (gloCounter == BUFFERSIZE) return;							// if buff array is full return
	
	arrayBuff[gloCounter] = rxData;								//put data into array
	arrayBuff[gloCounter+1] = '\0';								//write next array item to null
	
	gloCounter++;												// let's raise counter by 1
	gloLinecounter++;											// also raise Linecounter by 1 
		
}
//--------------------------------------------------------------------


