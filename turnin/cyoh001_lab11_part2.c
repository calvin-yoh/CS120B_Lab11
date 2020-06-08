/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#include "scheduler.h"
#include "keypad.h"
#include "io.c"
#endif

enum STATES { Display };

unsigned char count = 1;
unsigned char a;

const unsigned char sentance[60] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','C','S','1','2','0','B',' ','i','s',' ','L','e','g','e','n','d','.','.','.','w','a','i','t',' ','f','o','r',' ','i','t',' ','D','A','R','Y','!',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
int DisplaySentance_Tick(int state) {
    switch (state) {
    case Display:
        for (a = 1; a <= 16; a++) {
            LCD_Cursor(a);
            LCD_WriteData(sentance[count + a - 2]);
            if (count + a + 1 == 60) {
                count = 1;
            }
        }
        count++;
        break;
    }
    return state;
}
int main(void) {
    DDRB = 0xFF; PORTB = 0x00;
    DDRA = 0xF0; PORTA = 0x0F;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    static task task1;
    task* tasks[] = { &task1 };
    const unsigned short numTasks = sizeof(tasks) / sizeof(task*);

    task1.state = Display;
    task1.period = 10;
    task1.elapsedTime = task1.period;
    task1.TickFct = &DisplaySentance_Tick;

    unsigned long GCD = tasks[0]->period;
    unsigned short x;
    for (x = 1; x < numTasks; x++)
    {
        GCD = findGCD(GCD, tasks[x]->period);
    }

    TimerSet(GCD);
    TimerOn();
    LCD_init();

    unsigned short i;
    while (1) {
        for (i = 0; i < numTasks; i++) {
            if (tasks[i]->elapsedTime == tasks[i]->period) {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += GCD;
        }
        while (!TimerFlag);
        TimerFlag = 0;
    }
    return 0;
}