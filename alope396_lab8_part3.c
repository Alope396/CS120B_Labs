/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 * 
 *      Demo Link:https://youtu.be/3DqSQ6T4VWQ
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
    ADC_init();

    while (1) {
        unsigned short x = ADC;
        unsigned char on = 0x01;
        unsigned char off = 0;

        if(x >= MAX/2){
            PORTB = on;
        }
        else{
            PORTB = off;
        }
    }
    return 1;
}