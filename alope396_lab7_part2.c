/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 * 
 *      Demo Link: https://youtu.be/um5iqEIW5kU
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#ifdef SIMULATE
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

enum States{START, FIRST, SEC, LAST, PAUSE, VICTORY} state;

unsigned char score = 5;
unsigned char paused = 0;
void Tick(){
    unsigned char input = 0;
    input = (~PINA & 0x01);
    switch(state){
        case START:
            state = FIRST;
            break;
        case FIRST:
            if(input){
                state = PAUSE;
            }
            else{
                state = SEC;
            }
            break;
        case SEC:
            if(input){
                state = PAUSE;
            }
            else{
                state = LAST;
            }
            break;
        case LAST:
            if(input){
                state = PAUSE;
            }
            else{
                state = FIRST;
            }
            break;
        case PAUSE:
            if((score == 9) && (PORTB == 0x02)){
                state = VICTORY;
            }
            else if(input){
                state = FIRST;
            }
            else{
                state = PAUSE;
            }
            break;
        case VICTORY:
            if(input == 0x01){
                score = 5;
                state = FIRST;
                LCD_ClearScreen();
                LCD_WriteData(score + '0');
            }
            break;
        default:
            break;
    }
    switch(state){
        case START:
            PORTB = 0;
            score = 5;
            LCD_ClearScreen();
            LCD_WriteData(score + '0');
            break;
        case FIRST:
            PORTB = 0x01;
            paused = 0;
            break;
        case SEC:
            PORTB = 0x02;
            break;
        case LAST:
            PORTB = 0x04;
            break;
        case PAUSE:
            if(PORTB == 0x02){
                if(score < 9){
                    if(paused == 0){
                        score += 1;
                        if(score < 9){
                            LCD_ClearScreen();
                            LCD_WriteData(score + '0');
                        }
                        paused = 1;
                    }
                }
            }
            else{
                if(score > 0){
                    if(paused == 0){
                        score -= 1;
                        LCD_ClearScreen();
                        LCD_WriteData(score + '0');
                        paused = 1;
                    }
                }
            }
            break;
        case VICTORY:
            LCD_ClearScreen();
            LCD_DisplayString(1, "YOU WIN!");
            break;
        default:
            break;
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTC = 0x00;

    TimerSet(300);
    TimerOn();
    LCD_init();
    LCD_WriteData(score + '0');
    state = START;
    while(1){
        Tick();
        while(!TimerFlag){}
        TimerFlag = 0;
    }
    return 1;
}