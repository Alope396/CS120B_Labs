/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 * 
 *      Demo Link:https://youtu.be/JqkePNdcBBY
 */
#include <avr/io.h>
#include <avr/eeprom.h>
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

typedef struct task{
    signed char state;
    unsigned long int period;
    unsigned long int elapsedTime;
    int (*TickFct)(int);
}task;

void ADC_init(){
    ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<< ADATE);
}

void clearLED(){
    PORTA = 0x00;
    PORTD = 0x00;
    for (int i = 0; i < 8 ; ++i){
        PORTC = 0x08;
        PORTC |= ((0x00 >> i) & 0x01);
        PORTC |= 0x02;  
    }
    PORTC |= 0x04;
    PORTC = 0x00;
}

void setLED(unsigned char x, unsigned char y){
    if(x == 0){
        PORTD = y;
    }
    else if(x == 1){
        PORTA = y;
    }
    else if(x == 2){
        for (int i = 0; i < 8 ; ++i){
            PORTC = 0x08;
            PORTC |= ((y >> i) & 0x01);
            PORTC |= 0x02;  
        }
        PORTC |= 0x04;
        PORTC = 0x00;
    }
    else{
        return;
    }
}

unsigned int r = 6321;
unsigned long getRandomNumber(){
    unsigned x = r;
    x ^= x << 11;
    x ^= x >> 12;
    x ^= x << 4;
    r = x;
    return x;
}

void createSequence(unsigned long* game){
    for(int k = 0; k < 5; k++){
        game[k] = (getRandomNumber() % 3);
    }
    return;
}

unsigned char high_score;
void loadHighScore(){
    high_score = eeprom_read_byte((uint8_t*)46);
    return;
}

void displayHighScore(){
    if(high_score == 1){
        PORTB = PORTB | 0x02;
    }
    else if(high_score == 2){
        PORTB = PORTB | 0x06;
    }
    else if(high_score == 3){
        PORTB = PORTB | 0x0E;
    }
    return;
}

unsigned char c1[8] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
unsigned char c2[8] = {0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00};
unsigned char c3[8] = {0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
unsigned char o1[8] = {0xFF, 0x31, 0x42, 0x11, 0x80, 0x04, 0x40, 0x81};
unsigned char o2[8] = {0xFF, 0x42, 0x80, 0x40, 0x31, 0x81, 0x11, 0x40};
unsigned char o3[8] = {0xFF, 0x40, 0x31, 0x42, 0x81, 0x04, 0x80, 0x11};
unsigned long game[5] = {0,1,2,0,1};


enum STATE{Game_Start, Game_Wait, Game_Display, Game_Select};

enum D_STATE{Display_Start, Display_Wait, Display_Seq, Display_Buffer, Display_Incorrect, Display_Correct, Display_GameOver, Display_Victory};

unsigned char turn = 0;
unsigned char level = 1;
unsigned char key = 0;
unsigned char pass = 0;
unsigned char fail = 0;
unsigned char lives = 0;
unsigned char toggle_timer = 0;
unsigned char timer = 0;
unsigned char score = 0;
unsigned char reset = 0;
unsigned char i = 0;
unsigned char j = 0;
unsigned char counter = 0;
int Tick_DisplayLED(int state){
    unsigned char input = ~PINB & 0x01;
    switch(state){
        case Display_Start:
            if(input == 0x01){
                state = Display_Wait;
                PORTB = PORTB | 0x0E;
                reset = 0;
            }
            else{
                state = Display_Start;
            }
            break;
        case Display_Wait:
            if(turn == 0){
                toggle_timer = 0;
                if(pass == 1){
                    state = Display_Correct;
                }
                else if(fail == 1){
                    state = Display_Incorrect;
                }
                else if(timer == 0){
                    state = Display_GameOver;
                }
                else{
                    state = Display_Seq;
                }
            }
            else{
                state = Display_Wait;
            }
            break;
        case Display_Seq:
            if(counter < 10){
                counter++;
            }   
            else{
                if(i < level){
                    clearLED();
                    setLED(game[i], 0xFF);
                    i++;
                    state = Display_Buffer;
                }
                else{
                    i = 0;
                    state = Display_Wait;
                    turn = 1;
                    clearLED();
                }
                counter = 0;
            }
            break;
        case Display_Buffer:
            if(counter < 10){
                counter++;
            }
            else{
                clearLED();
                state = Display_Seq;
                counter = 0;
            }
            break;
        case Display_Incorrect:
            if(counter < 5){
                counter++;
            }
            else{
                if(j < 8){
                    if(j%2 == 0){
                        setLED(game[key], 0xFF);
                    }
                    else{
                        setLED(game[key], 0x00);
                    }
                    j++;
                }
                else{
                    j = 0;
                    fail = 0;
                    key = 0;
                    // if(lives == 1){
                    //     PORTB = PORTB & 0x03;
                    // }
                    // else if(lives == 2){
                    //     PORTB = PORTB & 0x07;
                    // }
                    if(lives == 0){
                        state = Display_GameOver;
                    }
                    else{
                        state = Display_Wait;
                    }
                }
                counter = 0;
            }    
            break;
        case Display_Correct:
            if(j < 8){
                if(i < 8){
                    setLED(0, c1[i]);
                    i++;
                }
                else{
                    i = 0;
                }
                j++;
            }
            else if(j < 17){
                if(i < 8){
                    setLED(0, c2[i]);
                    setLED(1, c1[i]);
                    i++;
                }
                else{
                    i = 0;
                }
                j++;
            }
            else if(j < 26){
                if(i < 8){
                    setLED(1, c2[i]);
                    setLED(2, c3[i]);
                    i++;
                }
                else{
                    i = 0;
                }
                j++;
            }
            else{
                j = 0;
                i = 0;
                clearLED();
                if(level < 5){
                    turn = 0;
                    state = Display_Wait;
                    level++;
                    pass = 0;
                }
                else{
                    state = Display_Victory;                
                }
            }
            break;
        case Display_GameOver:
            if(counter < 10){
                counter++;
            }
            else{
                if(j < 8){
                    PORTA = o1[j];
                    setLED(2, o2[j]);
                    PORTD = o3[j];
                    PORTB = PORTB & 0x01;
                    j++;
                }
                else{
                    clearLED();
                    j = 0;
                    state = Display_Start;
                }
                counter = 0;
            }
            break;
        case Display_Victory:
            state = Display_Start;
            break;
    }
    switch(state){
        case Display_Start:
            lives = 3;
            level = 1;
            timer = 30;
            turn = 0;
            reset = 1;
            displayHighScore();
            createSequence(game);
            break;
        case Display_Wait:
            j = 0;
            break;
        case Display_Victory:
            if(score > high_score){
                high_score = score;
                eeprom_write_byte ((uint8_t*)46, high_score);
            }
            break;
    }
    return state;
}

unsigned char pos = 1;
enum S_STATES{Select_Start, Select_Wait, Select_Check, Select_Buffer};
int Tick_Select(int state){
    unsigned short x = ADC;
    unsigned input = ~PINB & 0x01;
    switch(state){
        case Select_Start:
            if(turn == 1 && reset == 0){
                state = Select_Wait;
            }
            else{
                state = Select_Start;
            }
            break;
        case Select_Wait:
            toggle_timer = 1;
            if(timer == 0){
                turn = 0;
                state = Select_Start;
                clearLED();

            }
            else{
                if(x < 400 && pos > 0){
                    pos--;
                }
                else if(x > 600 && pos < 2){
                    pos++;
                }
                if(input == 0x01){
                    state = Select_Check;
                }
                else{
                    state = Select_Wait;
                }
                clearLED();
                setLED(pos, 0xFF);
            }
            if(reset == 1){
                state = Select_Start;
            }
            break;
        case Select_Check:
            if(pos == game[key]){
                if(key == level - 1){
                    clearLED();
                    state = Select_Start;
                    key = 0;
                    turn = 0;
                    pass = 1;
                }
                else{
                    clearLED();
                    state = Select_Wait;
                    key++;
                }
            }
            else{
                state = Select_Start;
                turn = 0;
                fail = 1;
                lives--;
                clearLED();
            }
            if(reset == 1){
                state = Select_Start;
            }
            break;
    }
    switch(state){
        case Select_Start:
            pos = 1;
            if(turn == 1){
                clearLED();
            }
            break;
        case Select_Wait:
            break;
        case Select_Check:
            break;
    }
    return state;
}

enum T_STATES{T_Start, T_On, T_Off};
int Tick_Timer(int state){
    switch(state){
        case T_Start:
            if(toggle_timer == 0){
                state = T_Start;
            }
            else{
                state = T_On;
            }
            break;
        case T_On:
            if(toggle_timer == 0){
                state = T_Off;
            }
            else{
                state = T_On;
            }
            break;
        case T_Off:
            if(toggle_timer == 0){
                state = T_Off;
            }
            else{
                state = T_On;
            }
            break;
    }
    switch(state){
        case T_Start:
            break;
        case T_On:
            if(timer > 0){
                timer--;
            }
            if(timer == 0){
                PORTB = PORTB & 0x01;
                score = 0;
            }
            else if(timer >= 0 && timer < 10){
                PORTB = PORTB & 0x03;
                score = 1;
            }
            else if(timer >= 10 && timer < 20){
                PORTB = PORTB & 0x07;
                score = 2;
            }
            else if(timer >= 20 && timer < 30){
                PORTB = PORTB & 0x0F;
                score = 3;
            }
            break;
        case T_Off:
            break;
        default:
            break;
    }
    return state;
}

int main(void) {
    DDRA = 0xFE; PORTA = 0x00;
    DDRB = 0x0E; PORTB = 0x01;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    static task D_Task, S_Task, T_Task;
    task *tasks[] = {&D_Task, &S_Task, &T_Task};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = 0;

    D_Task.state = start;
    D_Task.period = 50;
    D_Task.elapsedTime = D_Task.period;
    D_Task.TickFct = &Tick_DisplayLED;

    S_Task.state = start;
    S_Task.period = 200;
    S_Task.elapsedTime = S_Task.period;
    S_Task.TickFct = &Tick_Select;

    T_Task.state = start;
    T_Task.period = 1000;
    T_Task.elapsedTime = T_Task.period;
    T_Task.TickFct = &Tick_Timer;

    TimerSet(50);
    TimerOn();
    clearLED();
    ADC_init();

    if(eeprom_read_byte((uint8_t*)46) == 0xFF){
        eeprom_write_byte((uint8_t*)46, 0);
    }

    loadHighScore();

    unsigned short i;

    while (1) {
        for(i = 0; i < numTasks; i++){
            if(tasks[i]->elapsedTime == tasks[i]->period){
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 50;
        }
        while(!TimerFlag);
            TimerFlag = 0;
    }
    return 0;
}