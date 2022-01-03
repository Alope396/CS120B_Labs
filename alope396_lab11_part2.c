/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 * 
 *      Demo Link:https://drive.google.com/file/d/103THNVkPru3OGEjdhCP7PAwY6iW3bcTg/view?usp=sharing    
 */
#include <avr/io.h>
#include "timer.h"
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

enum STATES{START, DISPLAY}state;

char* msg = "CS120B is Legend... wait for it DARY!";
unsigned char display[16];
unsigned char i = 0;
unsigned char k = 0;
int Tick_LCD(){
    switch(state){
        case START:
            state = DISPLAY;
            break;
        case DISPLAY:
            state = DISPLAY;
            if(k < 37){
                i = (i + 1) % 38;
            }
            break;
        default:
            break;
    }
    switch(state){
        case START:
            break;
        case DISPLAY:
            for(unsigned int j = 0; j < 16; j++){
                display[j] = msg[(j + i)%38];
            }
            LCD_DisplayString(1, display);
            break;
        default:
            break;
    }
    return state;
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    static task LCD_Task;
    task *tasks[] = {&LCD_Task};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = 0;

    LCD_Task.state = start;
    LCD_Task.period = 500;
    LCD_Task.elapsedTime = LCD_Task.period;
    LCD_Task.TickFct = &Tick_LCD;

    TimerSet(500);
    TimerOn();

    LCD_init();
    LCD_ClearScreen();
    unsigned short i;

    while (1) {
        for(i = 0; i < numTasks; i++){
            if(tasks[i]->elapsedTime == tasks[i]->period){
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 500;
        }
        while(!TimerFlag){
            TimerFlag = 0;
        }
    }
    return 0;
}