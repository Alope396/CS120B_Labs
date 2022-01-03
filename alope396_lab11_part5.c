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
#include "io.h"
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

enum STATES{START, WAIT, UPDATE}state;
unsigned char input = 0;
unsigned char x;
int Tick_Keypad(){
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
                    input = 0x1F;
                    break;
                case '1':
                    input = 0x01;
                    break;
                case '2':
                    input = 0x02;
                    break;
                case '3':
                    input = 0x03;
                    break;
                case '4':
                    input = 0x04;
                    break;
                case '5':
                    input = 0x05;
                    break;
                case '6':
                    input = 0x06;
                    break;
                case '7':
                    input = 0x07;
                    break;
                case '8':
                    input = 0x08;
                    break;
                case '9':
                    input = 0x09;
                    break;
                case 'A':
                    input = 0x0A;
                    break;
                case 'B':
                    input = 0x0B;
                    break;
                case 'C':
                    input = 0x0C;
                    break;
                case 'D':
                    input = 0x0D;
                    break;
                case '*':
                    input = 0x0E;
                    break;
                case '0':
                    input = 0x00;
                    break;
                case '#':
                    input = 0x0F;
                    break;
                default:
                    input = 0x1B;
                    break;
            }
            break;
        default:
            break;
    }
    return state;
}

unsigned char i = 1;
int Tick_LCD(){
    switch(state){
        case START:
            state = WAIT;
            break;
        case WAIT:
            if(input == 0x1F){
                state = WAIT;
            }
            else{
                state = UPDATE;
            }
            break;
        case UPDATE:
            if(input != 0x1F){
                state = UPDATE;
            }
            else{
                state = WAIT;
            }
        default:
            break;
    }
    switch(state){
        case START:
            LCD_DisplayString(1, "Congratulations!");
            break;
        case WAIT:
            if(x != '\0'){
                if(i > 16){
                    i = 1;
                }
                else{
                    LCD_Cursor(i);
                    LCD_WriteData(x);
                }
                i++;
            }
            break;
        case UPDATE:
            break;
        default:
            break;
    }
    return state;
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    static task K_Task, LCD_Task;
    task *tasks[] = {&K_Task,&LCD_Task};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;

    K_Task.state = start;
    K_Task.period = 50;
    K_Task.elapsedTime = K_Task.period;
    K_Task.TickFct = &Tick_Keypad;

    LCD_Task.state = start;
    LCD_Task.period = 50;
    LCD_Task.elapsedTime = LCD_Task.period;
    LCD_Task.TickFct = &Tick_LCD;

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