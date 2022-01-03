/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 * 
 *      Demo Link:https://youtu.be/UoYmH70B-4o
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

enum States{START, UPDATE, UP, DOWN, U_PRESS, D_PRESS} state;

unsigned long counter = 0;

unsigned char freq = 2;
unsigned char z = 0;
void Tick_ChangeFrequencySM(){
    unsigned long x[6] = {1, 2, 3, 4, 5, 6};
    unsigned char temp = ~PINA;
        switch(state){
        case START:
            state = UPDATE;
            break;
        case UPDATE:
            if(counter % (100/freq) == 1){
                if(temp == 0x01){
                    state = UP;
                }
                else if(temp == 0x02){
                    state = DOWN;
                }
                else{
                    state = UPDATE;
                }
            }
            else{
                state = UPDATE;
            }
            break;
        case UP:
            if(temp == 0x01){
                state = U_PRESS;
            }
            else{
                state = UPDATE;
            }
            break;
        case DOWN:
            state = D_PRESS;
            break;
        case U_PRESS:
            if(temp == 0x01){
                state = U_PRESS;
            }
            else{
                state = UPDATE;
            }
            break;
        case D_PRESS:
            if(temp == 0x02){
                state = D_PRESS;
            }
            else{
                state = UPDATE;
            }
            break;
        default:
            break;
    }
    switch(state){
        case START:
            break;
        case UPDATE:
            break;
        case UP:
            if(z > 0){
                z--;
            }
            freq = x[z];
            TimerSet(freq);
            break;
        case DOWN:
            if(z < 5){
                z++;
            }
            freq = x[z];
            TimerSet(freq);
            break;
        case U_PRESS:
            break;
        case D_PRESS:
            break;
        default:
            break;
    }
}

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
            if(counter % (300/freq) == 1){
                if(i < 2){
                    i++;
                }
                else{
                    i = 0;
                }
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
            if(counter == (900/freq)){
                if(j == 0){
                    blinkingLED = 0x08;
                    j++;
                }
                else{
                    blinkingLED = 0x00;
                    j = 0;
                }
            }
            break;
        default:
            break;
    }
}


unsigned char toggle = 0;
unsigned char k = 0;
void Tick_ToggleSpeakerSM(){
    unsigned char temp = ~PINA;
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
            if(temp == 0x04){
                if(k == 0){
                    toggle = 0x10;
                    k++;
                }
                else{
                    toggle = 0x00;
                    k = 0;
                }
            }
            else{
                k = 0;
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
            if(counter < (1000/freq)){
                counter++;
            }
            else{
                counter = 0;
            }
            break;
        default:
            break;
    }
    switch(state){
        case START:
            break;
        case UPDATE:
            PORTB = (blinkingLED | threeLeds | toggle);
            break;
        default:
            break;
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(2);
    TimerOn();
    counter = 0;
    while (1) {
        Tick_ChangeFrequencySM();
        Tick_ThreeLEDsSM();
        Tick_BlinkingLEDSM();
        Tick_ToggleSpeakerSM();
        Tick_CombineLEDsSM();
        while(!TimerFlag){}
        TimerFlag = 0;
    }
    return 1;
}