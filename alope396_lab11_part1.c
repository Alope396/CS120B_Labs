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
#include "timer.h"
#include "keypad.h"
#ifdef _SIMULATE_
#include <avr/interrupt.h>
#include "simAVRHeader.h"
#endif

typedef struct task{
    signed char state;
    unsigned long int period;
    unsigned long int elapsedTime;
    int (*TickFct)(int);
}task;

enum STATES{START, WAIT}state;

int Tick_Keypad(){
    unsigned char x;
    x = GetKeypadKey();
    switch(state){
        case START:
            state = WAIT;
            break;
        case WAIT:
            state = WAIT;
            break;
        default:
            break;
    }
    switch(state){
        case START:
            break;
        case WAIT:
            switch(x){
                case '\0':
                    PORTB = 0x1F;
                    break;
                case '1':
                    PORTB = 0x01;
                    break;
                case '2':
                    PORTB = 0x02;
                    break;
                case '3':
                    PORTB = 0x03;
                    break;
                case '4':
                    PORTB = 0x04;
                    break;
                case '5':
                    PORTB = 0x05;
                    break;
                case '6':
                    PORTB = 0x06;
                    break;
                case '7':
                    PORTB = 0x07;
                    break;
                case '8':
                    PORTB = 0x08;
                    break;
                case '9':
                    PORTB = 0x09;
                    break;
                case 'A':
                    PORTB = 0x0A;
                    break;
                case 'B':
                    PORTB = 0x0B;
                    break;
                case 'C':
                    PORTB = 0x0C;
                    break;
                case 'D':
                    PORTB = 0x0D;
                    break;
                case '*':
                    PORTB = 0x0E;
                    break;
                case '0':
                    PORTB = 0x00;
                    break;
                case '#':
                    PORTB = 0x0F;
                    break;
                default:
                    PORTB = 0x1B;
                    break;
            }
            break;
        default:
            break;
    }
    return state;
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    static task K_Task;
    task *tasks[] = {&K_Task};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;

    K_Task.state = start;
    K_Task.period = 50;
    K_Task.elapsedTime = K_Task.period;
    K_Task.TickFct = &Tick_Keypad;

    TimerSet(50);
    TimerOn();

    unsigned short i;

    while (1) {
        for(i = 0; i < numTasks; i++){
            if(tasks[i]->elapsedTime == tasks[i]->period){
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 50;
        }
        while(!TimerFlag){
            TimerFlag = 0;
        }
    }
    return 0;
}