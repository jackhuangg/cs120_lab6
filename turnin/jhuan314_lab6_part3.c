/*	Author: Jack Huang
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *      https://drive.google.com/drive/folders/1JBIqqJb-m900203LVLXI8yLaMciH493w?usp=sharing
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerISR()
{
    TimerFlag = 1;
}

void TimerOff()
{
    TCCR1B = 0x00;
}

void TimerOn()
{
    TCCR1B = 0x0B;

    OCR1A = 125;

    TIMSK1 = 0x02;

    TCNT1 = 0;

    _avr_timer_cntcurr = _avr_timer_M;

    SREG |= 0x80;

}

ISR(TIMER1_COMPA_vect)
{
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0)
    {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M)
{
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

unsigned char counter=0;
unsigned char waitminusstate=0;
enum SM1_STATES { SM1_SMStart, check, plus, waitplus, minus, reset, waitminus, waitreset,waitsecond} SM1_STATE;
void Tick_StateMachine1() {
   switch(SM1_STATE) {
      case SM1_SMStart:
         SM1_STATE = check;
         break;
      case check:
         if((~PINA&0x03)==0x03){
	    SM1_STATE = reset;
	 }
         else if ((~PINA&0x01)==0x01) {
            SM1_STATE = plus;
         }
         else if ((~PINA&0x02)==0x02) {
            SM1_STATE = minus;
         }
         //else if ((~PINA&0x03) == 0x03) {
         //   SM1_STATE = reset;
         //}
         else {
            SM1_STATE = check;
         }
         break;
      case plus:
         SM1_STATE = waitplus;
         break;
      case waitplus:
         //if ((~PINA&0x03) == 0x03) {
         //   SM1_STATE = reset;
         //}
         if ((~PINA&0x01)==0x01) {
            waitminusstate=0;
            SM1_STATE = waitsecond;
         }
         else{
	    SM1_STATE = check;
	 }
         break;
      case minus:
	 if ((~PINA&0x03) == 0x03) {
	    SM1_STATE = reset;
	 }
         if (1) {
            SM1_STATE = waitminus;
         }
         break;
      case reset:
         if ((~PINA&0x03)==0x03) {
            SM1_STATE = reset;
         }
         else {
            SM1_STATE = check;
         }
         break;
      case waitminus:
         if ((~PINA&0x03) == 0x03) {
            SM1_STATE = reset;
         }
         else if ((~PINA&0x02)==0x02) {
	    waitminusstate=1;
            SM1_STATE = waitsecond;
         }
         else {
            SM1_STATE = check;
         }
         break;
      case waitreset:
         if ((~PINA&0x03) == 0x03){
            SM1_STATE = waitreset;
         }
         else {
            SM1_STATE = check;
         }
         break;
      case waitsecond:
	 if((~PINA&0x03) == 0x03){
		counter=0;
	 	SM1_STATE = reset;
	 }
	 if(counter<9){
		counter++;
	 	SM1_STATE=waitsecond;
	 }
	 else{
		counter=0;
	 	if(!waitminusstate){
			SM1_STATE=plus;
		}
		else{
			SM1_STATE=minus;
		}
	 }
	 break;
      default:
         SM1_STATE = SM1_SMStart;
         break;
   }
   switch(SM1_STATE) {
      case SM1_SMStart:
         PORTB=7;
         break;
      case check:
         break;
      case plus:
         if(PORTB<9){
	     PORTB=PORTB+1;
 	 }
         break;
      case waitplus:
         break;
      case minus:
         if(PORTB>0){
	     PORTB=PORTB-1;
	 }
         break;
      case reset:
         PORTB=0;
         break;
      case waitminus:
         break;
      case waitreset:
         break;
      case waitsecond:
	 break;
      default:
	 PORTB=7;
         break;
   }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00;
    DDRB = 0xFF;
    PORTA = 0xFF;
    PORTB = 0x07;
    TimerSet(100);
    TimerOn();
    /* Insert your solution below */
   while(1){
	Tick_StateMachine1();
	while(!TimerFlag);
	TimerFlag=0;
   }
    return 1;
}
