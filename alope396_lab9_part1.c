/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 * 
 *      Demo Link:https://youtu.be/XqvLnndf5HU
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

enum States{START, WAIT, B1, B2, B3} state;

void Tick(){
    unsigned char temp = ~PINA;

    switch(state){
        case START:
            state = WAIT;
            break;
        case WAIT:
            if(temp == 0x01){
                state = B1;
            }
            else if(temp == 0x02){
                state = B2;
            }
            else if(temp == 0x04){
                state = B3;
            }
            else{
                state = WAIT;
            }
            break;
        case B1:
            if(temp == 0x01){
                state = B1;
            }
            else{
                state = WAIT;
            }
            break;
        case B2:
            if(temp == 0x02){
                state = B2;
            }
            else{
                state = WAIT;
            }
            break;
        case B3:
            if(temp == 0x04){
                state = B3;
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
            break;
        case WAIT:
            set_PWM(0);
            break;
        case B1:
            set_PWM(261.63);
            break;
        case B2:
            set_PWM(293.66);
            break;
        case B3:
            set_PWM(329.63);
            break;
        default:
            break;
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    PWM_on();
    while (1) {
        Tick();
    }
    return 1;
}