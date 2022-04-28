#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <p18f4620.h>

#define PORT_BUTTON		PORTC
#define TRIS_BUTTON		TRISC
#define	MAX_COL			4
#define MAX_ROW			4

#define BTN_PRESS_FAST  1
#define BTN_PRESS_MID   2
#define BTN_PRESS_LONG  3

extern unsigned int key_code[16];



void init_key_matrix();
void init_key_matrix_with_uart();
void scan_key_matrix();
void scan_key_matrix_1_line();
void scan_key_matrix_demo();
void button_process();
void scan_key_matrix_with_uart();
void scan_key_matrix_with_uart_i2c();
void init_key_matrix_new_board();
void scan_key_matrix_new_board();

unsigned char isButton(char index);
unsigned char isButtonPressKeep(char index);

#endif