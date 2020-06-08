/*	Author: lab
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


unsigned char tempB = 0x00;
unsigned char key;

enum keypad_States { Wait, Input, Prepare } key_state;
enum Display_States { Display } screen_state;

int Key_Input(int state) {

	key = GetKeypadKey();
	switch (state) {
	case Wait:
		if (key == '\0') {
			
			state = Wait;
		}
		else {
			LCD_WriteData('W');
			state = Input;
		}
		break;
	case Input:
		if (key == '\0') {
			
			state = Prepare;
		}
		else {
			state = Input;
		}
		break;
	case Prepare:
		state = Wait;
		break;
	default: 
		break;
	}
	switch (state) {
	case Prepare:
		switch (key) {
		case '\0':
			tempB = 0x1F;
			break;
		case '1':
			tempB = 0x01;
			break;
		case '2':
			tempB = 0x02;
			break;
		case '3':
			tempB = 0x03;
			break;
		case '4':
			tempB = 0x04;
			break;
		case '5':
			tempB = 0x05;
			break;
		case '6':
			tempB = 0x06;
			break;
		case '7':
			tempB = 0x07;
			break;
		case '8':
			tempB = 0x08;
			break;
		case '9':
			tempB = 0x09;
			break;
		case 'A':
			tempB = 0x11;
			break;
		case 'B':
			tempB = 0x12;
			break;
		case 'C':
			tempB = 0x13;
			break;
		case 'D':
			tempB = 0x14;
			break;
		case '*':
			tempB = 0x0E;
			break;
		case '0':
			tempB = 0x00;
			break;
		case '#':
			tempB = 0x0F;
			break;
		default:
			tempB = 0x1B;
			break;
		}
		break;
	default:
		tempB = 0x1F;
		break;
	}
	return state;
}

unsigned char currCol = 1;

int Display_Screen(int state) {
	switch (state) {
	case Display:
		state = Display;
		break;
	default:
		state = Display;
		break;
	}
	switch (state) {
	case Display:
		if ((tempB & 0xFF) != 0x1F) {
			LCD_Cursor(currCol);
			currCol++;
			LCD_WriteData(tempB + '0');
		}
		break;
	}
	return state;
}

int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	DDRD = 0xFF; PORTD = 0x00;

	static task task1, task2;
	task* tasks[] = { &task1, &task2 };
	const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
	const char start = -1;

	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &Key_Input;

	task2.state = start;
	task2.period = 250;
	task2.elapsedTime = task2.period;
	task2.TickFct = &Display_Screen;

	unsigned short x;

	unsigned long GCD = tasks[0]->period;
	for (x = 1; x < numTasks; x++) {
		GCD = findGCD(GCD, tasks[x]->period);
	}

	TimerSet(GCD);
	TimerOn();

	LCD_init();
	LCD_ClearScreen();
	LCD_DisplayString(1, "Congratulations!");

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
	return 1;
}
