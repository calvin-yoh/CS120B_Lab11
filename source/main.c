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
#include <bit.h>
#include <keypad.h>
#include <avr/interrupt.h>
#include <timer.h>
#include <scheduler.h>
#include <io.h>

enum States { Start, Wait, Display, Prepare } state;

int Display_Key(int state) {
	unsigned char key = GetKeypadKey();
	static unsigned char prev;
	switch (state) {
	case Start:
		state = Wait;
		break;
	case Wait:
		if (key == '\0' || prev == key) {
			state = Wait;
		}
		else {
			state = Display;
			prev = key;
		}
		break;
	case Display:
		if (key == '\0') {
			state = Prepare;
		}
		else {
			state = Display;
		}
		break;
	case Prepare:
		state = Wait;
		break;
	default:
		state = Start;
		break;
	}
	switch (state) {
	case Prepare:
		switch (prev) {
		case '1': LCD_DisplayString(1, "1"); break;
		case '2': LCD_DisplayString(1, "2"); break;
		case '3': LCD_DisplayString(1, "3"); break;
		case '4': LCD_DisplayString(1, "4"); break;
		case '5': LCD_DisplayString(1, "5"); break;
		case '6': LCD_DisplayString(1, "6"); break;
		case '7': LCD_DisplayString(1, "7"); break;
		case '8': LCD_DisplayString(1, "8"); break;
		case '9': LCD_DisplayString(1, "9"); break;
		case 'A': LCD_DisplayString(1, "A"); break;
		case 'B': LCD_DisplayString(1, "B"); break;
		case 'C': LCD_DisplayString(1, "C"); break;
		case 'D': LCD_DisplayString(1, "D"); break;
		case '*': LCD_DisplayString(1, "*"); break;
		case '0': LCD_DisplayString(1, "0"); break;
		case '#': LCD_DisplayString(1, "#"); break;
		}
		break;
	default:
		LCD_WriteData('#');
		break;
	}
	return state;
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	DDRD = 0xFF; PORTC = 0x00;

	static task task1;
	task* tasks[] = { &task1 };
	const unsigned short numTasks = sizeof(tasks) / sizeof(task*);

	task1.state = Start;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &Display_Key;

	unsigned long GCD = tasks[0]->period;
	unsigned short x;
	for (x = 1; x < numTasks; x++)
	{
		GCD = findGCD(GCD, tasks[x]->period);
	}
	TimerSet(GCD);
	TimerOn();

	LCD_init();
	LCD_Cursor(1);
	LCD_DisplayString(1, "Hello World");

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
}