/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
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
enum states {start,one,two,two2,three,wait,wait2}state;
unsigned char last=0;
unsigned char direc=0;
void Tick(){
	unsigned char PINAi = ~PINA;
	switch(state){
		case start:
			state=one;
			break;
		case one:
			last=1;
			direc=0;
			if(PINAi==0x01){
				state=wait;
                        }
			else{
				state=two;
			}
			break;
		case two:
			last=2;
			direc=0;
			if(PINAi==0x01){
				state=wait;
                        }
			else{
				state=three;
			}
			break;
		case three:
			last=3;
			direc=1;
			if(PINAi==0x01){
				state=wait;
                        }
			else{
				state=two2;
			}
			break;
		case two2:
			last=2;
			direc=1;
			if(PINAi==0x01){
				state=wait;
			}
			else{
				state=one;
			}
			break;
		case wait:
			if(PINAi==0x01){
				state=wait;
                        }
			else{
				state=wait2;
			}
			break;
		case wait2:
			if(PINAi==0x01){
				if(last==1){
					state=two;
				}
				else if(last==2&&direc==1){
					state=one;
				}
				else if(last==2&&direc==0){
					state=three;
				}
				else if(last==3){
					state=two2;
				}
			}
			else{
				state=wait2;
			}
			break;
	}
	switch(state){
		case start:
			PORTB=0x00;
			break;
		case one:
			PORTB=0x01;
			break;
		case two:
			PORTB=0x02;
			break;
		case two2:
			PORTB=0x02;
			break;
		case three:
			PORTB=0x04;
			break;
		case wait:
			break;
		case wait2:
			break;
	}
}
int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00;
    PORTA = 0xFF;
    DDRB = 0xFF;
    PORTB = 0x00;
    TimerSet(300);
    TimerOn();
    state=start;
    /* Insert your solution below */
    while (1) {
	Tick();
	while(!TimerFlag);
	TimerFlag=0;
    }
    return 1;
}
