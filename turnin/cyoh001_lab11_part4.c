/*	Author: Brandon Porter
 *  Partner(s) Name: 
 *	Lab Section: 21
 *	Assignment: Lab 11  Exercise 4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "keypad.h"
#include "scheduler.h"
#include "timer.h"
#include "io.h"


unsigned char tmpB = 0x00;
unsigned char keypadInput;

enum keypad_States { keypad_input };
int keypadSMTick(int state) {
	keypadInput = GetKeypadKey();

	switch(state) {
		case keypad_input:
			state = keypad_input;
			switch(keypadInput) {
				case '\0':
					tmpB = 0x1F;
					break;
				case '1':
					tmpB = 0x01;
					break;
				case '2':
					tmpB = 0x02;
					break;
				case '3':
					tmpB = 0x03;
					break;
				case '4':
					tmpB = 0x04;
					break;
				case '5':
					tmpB = 0x05;
					break;
				case '6':
					tmpB = 0x06;
					break;
				case '7':
					tmpB = 0x07;
					break;
				case '8':
					tmpB = 0x08;
					break;
				case '9':
					tmpB = 0x09;
					break;
				case 'A':
					tmpB = 0x11;
					break;
				case 'B':
					tmpB = 0x12;
					break;
				case 'C':
					tmpB = 0x13;
					break;
				case 'D':
					tmpB = 0x14;
					break;
				case '*':
					tmpB = 0x0E;
					break;
				case '0':
					tmpB = 0x00;
					break;
				case '#':
					tmpB = 0x0F;
					break;
				default:
					tmpB = 0x1B;
					break;
			}
			break;
		default:
			state = keypad_input;
			break;
	}
	return state;
}

unsigned char cursor = 0;

enum fillScreen_States { fillScreen_fillScreen };
int fillScreenSMTick(int state) {
	switch(state) {
		case fillScreen_fillScreen:
			state = fillScreen_fillScreen;
			break;
		default:
			state = fillScreen_fillScreen;
			break;
	}
	switch(state) {
		case fillScreen_fillScreen:
			if((tmpB & 0xFF) != 0x1F) {	
				LCD_Cursor(cursor);
				cursor++;		
				LCD_WriteData(tmpB + '0');
			}
			break;
	}
	return state;
}

/*enum display_States { display_display };
int displaySMTick(int state) {
	//unsigned char output;
	switch(state) {
		case display_display:
			state = display_display;
			break;
		default:
			state = display_display;
			break;
	}
	switch(state) {
		case display_display:
			//output = tmpB;
			break;
	}
	//PORTB = output;
	return state;
}*/

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
	static task task1, task2;
	task *tasks[] = { &task1, &task2 };
	const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
	const char start = -1;

	// Task 1 (fillScreenSM)
	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &keypadSMTick;

	// Task 2 (displaySM)
	task2.state = start;
	task2.period = 250;
	task2.elapsedTime = task2.period;
	task2.TickFct = &fillScreenSMTick;

	unsigned short i;

	unsigned long GCD = tasks[0]->period;
	for(i = 1; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i]->period);
	}

	TimerSet(GCD);
	TimerOn();

	LCD_init();
	LCD_ClearScreen();
	LCD_DisplayString(1, "Congratulations!");

	while (1) {
		for(i = 0; i < numTasks; i++) {
			if(tasks[i]->elapsedTime == tasks[i]->period) {
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += GCD;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 1;
}
