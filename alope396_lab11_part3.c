/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 * 
 *      Demo Link:https://drive.google.com/file/d/1GNjw5K7gyV4djxJPapI8OnkADftAd8Lz/view?usp=sharing
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

int Tick_LCD(){
    x = GetKeypadKey();
    switch(state){
        case START:
            state = WAIT;
            break;
        case WAIT:
            if(x == '\0'){
                state = WAIT;
            }
            else{
                state = UPDATE;
            }
            break;
        case UPDATE:
            if(input != '\0'){
                state = UPDATE;
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
            if(x != '\0'){
                LCD_ClearScreen();
                LCD_DisplayString(1, &x);
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
    DDRC = 0xF0; PORTC = 0x0F;
    DDRD = 0xFF; PORTD = 0x00;

    static task LCD_Task;
    task *tasks[] = {&LCD_Task};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = 0;

    LCD_Task.state = start;
    LCD_Task.period = 50;
    LCD_Task.elapsedTime = LCD_Task.period;
    LCD_Task.TickFct = &Tick_LCD;

    TimerSet(50);
    TimerOn();

    LCD_init();

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