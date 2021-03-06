/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 * 
 *      Demo Link:https://youtu.be/6swjR1UiSkU
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

void set_PWM(double frequency){
    static double current_frequency;
    if(frequency != current_frequency){
        if(!frequency){TCCR3B &= 0x08;}
        else{TCCR3B |= 0x03;}
        
        if(frequency < 0.954){OCR3A = 0xFFFF;}
        else if(frequency > 31250){OCR3A = 0x0000;}
        else{OCR3A = (short)(8000000/(128 * frequency)) - 1;}
        
        TCNT3 = 0;
        current_frequency = frequency;
    }
}

void PWM_on(){
    TCCR3A = (1 << COM3A0);
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    set_PWM(0);
}

void PWM_off(){
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}

enum States{START, WAIT, TOGGLE, UP, DOWN} state;

unsigned char status = 0;
unsigned char i = 0;

void Tick(){
    unsigned char temp = ~PINA;
    unsigned long notes[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

    switch(state){
        case START:
            state = WAIT;
            break;
        case WAIT:
            if(temp == 0x01){
                state = DOWN;
            }
            else if(temp == 0x02){
                state = UP;
            }
            else if(temp == 0x04){
                state = TOGGLE;
            }
            else{
                state = WAIT;
            }
            break;
        case DOWN:
            if(temp == 0x01){
                state = DOWN;
            }
            else{
                state = WAIT;
            }
            break;
        case UP:
            if(temp == 0x02){
                state = UP;
            }
            else{
                state = WAIT;
            }
            break;
        case TOGGLE:
            if(temp == 0x04){
                state = TOGGLE;
            }
            else{
                state = WAIT;
            }
            break;
        default:
            break;
    }
    switch(state){
        case START:
            i = 0;
            status = 0;
            PWM_off();
            break;
        case WAIT:
            break;
        case DOWN:
            if(i > 0){
                i--;
                set_PWM(notes[i]);
            }
            break;
        case UP:
            if(i < 7){
                i++;
                set_PWM(notes[i]);
            }
            break;
        case TOGGLE:
            if(status == 0){
                PWM_on();
                set_PWM(notes[i]);
                status = 1;
            }
            else{
                PWM_off();
                status = 0;
            }
            break;
        default:
            break;
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(300);
    TimerOn();

    while (1) {
        Tick();
        while(!TimerFlag){}
        TimerFlag = 0;
    }
    return 1;
}