/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 * 
 *      Demo Link:https://youtu.be/Ofi2CWhqjIY
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ADC_init(){
    ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<< ADATE);
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    unsigned char MAX = 0x70;
    unsigned char MIN = 0x3F;
    unsigned char threshold = (MAX - MIN)/8;
    unsigned char level = 0;

    ADC_init();

    while (1) {
        unsigned short x = ADC;

        if(x <= MAX - (7 *threshold)){
            level = 0x01;
        }
        else if(x <= MAX - (6 *threshold)){
            level = 0x03;
        }
        else if(x <= MAX - (5 *threshold)){
            level = 0x07;
        }
        else if(x <= MAX - (4 *threshold)){
            level = 0x0F;
        }
        else if(x <= MAX - (3 *threshold)){
            level = 0x1F;
        }
        else if(x <= MAX - (2 * threshold)){
            level = 0x3F;
        }
        else if(x <= MAX - threshold){
            level = 0x7F;
        }
        else{
            level = 0xFF;
        }
        PORTB = level;
    }
    return 1;
}