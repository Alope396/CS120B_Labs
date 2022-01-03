/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 * 
 *      Demo Link: https://youtu.be/_rhcfyABio8
 */
#include <avr/io.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include <avr/interrupt.h>

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

enum States{START, WAIT, INCREASE, DECREASE, ZERO} state;


unsigned char i = 0;
unsigned char check = 0;
void Tick(){
    unsigned char input;
    input = (~PINA & 0x03);

    switch(state){
        case START:
            state = WAIT;
            break;
        case WAIT:
            if(input == 0x01){
                state = INCREASE;
            }
            else if(input == 0x02){
                state = DECREASE;
            }
            else if(input == 0x03){
                state = ZERO;
            }
            else{
                state = WAIT;
            }
            break;
        case INCREASE:
            if(input == 0x01){
                state = INCREASE;
            }
            else if(input == 0x03){
                state = ZERO;
            }
            else{
                state = WAIT;
            }
        case DECREASE:
            if(input == 0x02){
                state = DECREASE;
            }
            else if(input == 0x03){
                state = ZERO;
            }
            else{
                state = WAIT;
            }
        case ZERO:
            if(input == 0x01){
                state = INCREASE;
            }
            else if(input == 0x02){
                state = DECREASE;
            }
            else{
                state = WAIT;
            }
        default:
            break;
    }
    switch(state){
        case START:
            LCD_WriteData(PORTB + '0');
            break;
        case WAIT:
            i = 0;
            check = 0;
            break;
        case INCREASE:
            if(PORTB < 9){
                if(check == 0){
                    check = 1;
                    PORTB += 1;
                }
                else if(i < 10){
                    i++;
                }
                else{
                    PORTB += 1;
                    i = 0;
                }
            }
            LCD_ClearScreen();
            LCD_WriteData(PORTB + '0');
            break;
        case DECREASE:
            if(PORTB > 0){
                if(check == 0){
                    check = 1;
                    PORTB -= 1;
                }
                else if(i < 10){
                    i++;
                }
                else{
                    PORTB -= 1;
                    i = 0;
                }
            }
            LCD_ClearScreen();
            LCD_WriteData(PORTB + '0');
            break;
        case ZERO:
            PORTB = 0;
            LCD_ClearScreen();
            LCD_WriteData(PORTB + '0');
            break;
        default:
            break;
    }
}

int main(void){
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    PORTB = 7;
    TimerSet(100);
    TimerOn();
    LCD_init();
    LCD_WriteData(PORTB + '0');
    state = START;
    while(1){
        Tick();
        while(!TimerFlag){}
        TimerFlag = 0;
    }
}