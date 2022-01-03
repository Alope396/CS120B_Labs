/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 * 
 *      Demo Link:https://youtu.be/WB-MapBJiWI
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include <avr/interrupt.h>
#include "simAVRHeader.h"
#endif

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

volatile unsigned char TimerFlag = 0;
void TimerISR(){TimerFlag = 1;}
void TimerSet(unsigned long M){
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}
void TimerOn(){
    TCCR1B = 0x0B;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1 = 0;
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}
void TimerOff(){
    TCCR1B = 0x00;
}
ISR(TIMER1_COMPA_vect){
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0){
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

enum States{START, UPDATE} state;

unsigned char threeLeds = 0;
unsigned char i = 0;
void Tick_ThreeLEDsSM(){
    unsigned char led[3] = {0x01, 0x02, 0x04};
    switch(state){
        case START:
            state = UPDATE;
            break;
        case UPDATE:
            state = UPDATE;
            break;
        default:
            break;
    }
    switch(state){
        case START:
            break;
        case UPDATE:
            threeLeds = led[i];
            if(i < 2){
                i++;
            }
            else{
                i = 0;
            }
            break;
        default:
            break;
    }
}

unsigned char blinkingLED = 0;
unsigned char j = 0;
void Tick_BlinkingLEDSM(){
    switch(state){
        case START:
            state = UPDATE;
            break;
        case UPDATE:
            state = UPDATE;
            break;
        default:
            break;
    }
    switch(state){
        case START:
            break;
        case UPDATE:
            if(j == 0){
                blinkingLED = 0x08;
                j++;
            }
            else{
                blinkingLED = 0x00;
                j = 0;
            }
            break;
        default:
            break;
    }
}

void Tick_CombineLEDsSM(){
    switch(state){
        case START:
            state = UPDATE;
            break;
        case UPDATE:
            state = UPDATE;
            break;
        default:
            break;
    }
    switch(state){
        case START:
            break;
        case UPDATE:
            PORTB = (blinkingLED | threeLeds);
            break;
        default:
            break;
    }
}

int main(void) {
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(1000);
    TimerOn();

    while (1) {
        Tick_ThreeLEDsSM();
        Tick_BlinkingLEDSM();
        Tick_CombineLEDsSM();
        while(!TimerFlag){}
        TimerFlag = 0;
    }
    return 1;
}