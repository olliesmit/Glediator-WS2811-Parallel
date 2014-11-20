/* Arduino MEGA 2560 Glediator Receiver - 8 Strips in Paralell
* Pin Order Explained below, adjust to your needs
* For differences between arduino pins and avr pins lookup "arduino 2560 pin mapping"
*
* My Code is Free For Any Use but contains modified copyrighted librarys and modified Original Glediator ws2801 arduino Code
* i Hope i gave reference and full respect to other peoples code, References below
* Oliver Smith, 2014,  osmizzle@gmail.com
*/



//Arduino Mega Sketch to drive strands and matrices of WS2801 pixels with Glediator.
//(c)2012 by R. Heller
//wwww.solderlab.de


/* WS2811.h Library i used for simultainious paralell writing to up to 4 strips. 
* I changed RGB_t to a normal char array in their code
* WS2811 multiple strips library - https://github.com/balrog-kun/ws2811-multi
* The WS2811 RGB LED strips have extremely many potential uses. In some of them, like POV displays, very high refresh rates are needed. If all your LEDs are connected serially, the time required to update all of the pixel values or any subset of them is 30us times the total number of LEDs connected -- even if you don't need to update all of their colours. At, say, 100 LEDs that's already 3ms per refresh, which limits your total frame rate to 300 Hz and on an Arduino if you add all other overheads you may end up with even lower rates.
*
* To improve on this a little, you can connect various strips in parallel instead of serially, i.e. with their data lines connected to different Atmega GPIO pins. The routines in this version of WS2811.h allow outputting the pixel data to up to 4 strips at the same time, if connected to GPIOs on the same Atmega port (e.g. PC0, PC1) of 2 strips if connected to different ports (e.g. PC2 and PB4).
*
* For example 4 strips of 25 LEDs connected to PC0 - PC3 (A0 to A3 in Arduino naming) can be updated in about 750us. Not an order of magnitude jump but enough to make some projects practical.
* Copyright 2012 Alan Burlison, alan@bleaklow.com.  All rights reserved.
* Use is subject to license terms.
*/

#include <WS2811.h>

// Adjust as necessary

/*
* ROW Interleaved For Better Effect - 
* ROW:  MEGA PIN:   PORT:  PORTPIN:	
* 1     41          G      0
* 2 	10          B      4
* 3 	40          G      1
* 4 	11          B      5
* 5 	39          G      2
* 6 	12          B      6
* 7 	4           G      5
* 8 	13          B      7
*/
#define PPS        72 // PIXels PER STRIP
#define NOSTRIPS   8		
#define NOPIX      NOSTRIPS*PPS

unsigned char matrix [NOPIX * 3];
static unsigned char *ptr;
static unsigned int pos = 0;
volatile unsigned char go = 0;

#define APORT          PORTG
#define ADDR           DDRG						
#define BPORT          PORTB
#define BDDR           DDRB			
#define PINA           0		
#define PINB           1						
#define PINC           2					
#define PIND           5	
#define PINE           4							
#define PINF           5	
#define PING           6						
#define PINH           7

//Bit Settings
#define BIT(B)           (0x01 << (uint8_t)(B))
#define SET_BIT_HI(V, B) (V) |= (uint8_t)BIT(B)
#define SET_BIT_LO(V, B) (V) &= (uint8_t)~BIT(B)


DEFINE_WS2811_OUT_4_COMMON_FN(ALPHA, APORT, PINA, PINB, PINC, PIND)
DEFINE_WS2811_OUT_4_COMMON_FN(BETA, BPORT, PINE, PINF, PING, PINH)





void setup() {
  //Disable global interrupts
  cli();
  
  // Initialize UART
  UCSR0A |= (1<<U2X0);                                
  UCSR0B |= (1<<RXEN0)  | (1<<TXEN0) | (1<<RXCIE0);   
  UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00)             ; 
  UBRR0H = 0;
  UBRR0L = 1; //Baud Rate 1 MBit

  //Set Ports&Pins
  SET_BIT_HI(ADDR, PINA);
  SET_BIT_LO(APORT, PINA);
  SET_BIT_HI(ADDR, PINB);
  SET_BIT_LO(APORT, PINB);
  SET_BIT_HI(ADDR, PINC);
  SET_BIT_LO(APORT, PINC);
  SET_BIT_HI(ADDR, PIND);
  SET_BIT_LO(APORT, PIND);
  SET_BIT_HI(BDDR, PINE);
  SET_BIT_LO(BPORT, PINE);
  SET_BIT_HI(BDDR, PINF);
  SET_BIT_LO(BPORT, PINF);
  SET_BIT_HI(BDDR, PING);
  SET_BIT_LO(BPORT, PING);
  SET_BIT_HI(BDDR, PINH);
  SET_BIT_LO(BPORT, PINH);

  ptr=matrix;
  
  //Enable global interrupts
  sei();
}





void loop() {
  if (go==1) 
  {
    //cli();
    ALPHA(&matrix[0], &matrix[3*2*PPS], &matrix[3*4*PPS], &matrix[3*6*PPS], PPS);
    BETA(&matrix[3*PPS], &matrix[3*3*PPS], &matrix[3*5*PPS], &matrix[3*7*PPS], PPS);
    go=0;
    //sei();
  }
}

ISR(USART0_RX_vect) 
{
  
  unsigned char b;
  b=UDR0;
  if (b == 1)  {pos=0; ptr=matrix; return;}    
  if (pos == (NOPIX*3)) {} else {*ptr=b; ptr++; pos++;}  
  if (pos == ((NOPIX*3)-1)) {go=1;}
}

