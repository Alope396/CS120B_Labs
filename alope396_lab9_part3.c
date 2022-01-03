/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 * 
 *      Demo Link:https://youtu.be/0__0Jex1aRg
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

enum States{START, WAIT, TOGGLE, PLAY} state;

unsigned char status = 0;
unsigned char i = 0;
unsigned char j = 0;
unsigned char timer = 0;
unsigned char pressed = 0;
void Tick(){
    unsigned char temp = ~PINA;
    unsigned long notes[] = {261.63, 349.23, 392.00, 440.00, 493.88, 392.00, 293.66, 392.00, 523.25, 440.00, 349.23, 293.66, 329.63};
    unsigned long timing[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3};

    switch(state){
        case START:
            state = WAIT;
            break;
        case WAIT:
            if(temp == 0x01 && pressed == 0){
                state = PLAY;
            }
            else if(temp == 0x00){
                state = WAIT;
                pressed = 0;
            }
            break;
        case PLAY:
            if(temp == 0x01){
                pressed = 1;
            }
            if(j < 14){
                state = PLAY;
            }
            else{
                state = WAIT;
                i = 0;
                j = 0;
            }
            break;
        default:
            break;
    }
    switch(state){
        case START:
            i = 0;
            break;
        case WAIT:
            set_PWM(0);
            break;
        case PLAY:
            if(timer < timing[j]){
                timer++;
            }
            else{
                if(i < 13){
                    set_PWM(notes[i]);
                    i++;
                }
                j++;
                timer = 0;
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
    PWM_on();
    while (1) {
        Tick();
        while(!TimerFlag){}
        TimerFlag = 0;
    }
    return 1;
}