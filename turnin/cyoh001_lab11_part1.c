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
#endif

enum STATES { Display };

unsigned char tempB = 0x00;

int DisplayKeyPad_Tick(int state) {
	unsigned char input;
	input = GetKeypadKey();
	switch (state) {
	case Display:
		switch (input) {
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
			tempB = 0x0A;
			break;
		case 'B':
			tempB = 0x0B;
			break;
		case 'C':
			tempB = 0x0C;
			break;
		case 'D':
			tempB = 0x0D;
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
		state = STATE_OUT;
		PORTB = tempB;
		break;
	}
	return state;
}


int main(void) {
	DDRA = 0xF0; PORTA = 0x0F;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	static task task1;
	task* tasks[] = { &task1 };
	const unsigned short numTasks = sizeof(tasks) / sizeof(task*);

	const char start = -1;

	task1.state = start;
	task1.period = 10;
	task1.elapsedTime = task1.period;
	task1.TickFct = &DisplayKeyPad_Tick;

	unsigned long GCD = tasks[0]->period;
	unsigned short x;
	for (x = 1; x < numTasks; x++)
	{
		GCD = findGCD(GCD, tasks[x]->period);
	}

	TimerSet(GCD);
	TimerOn();

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
