#ifndef _SET_BY_TERMINAL_H_
#define _SET_BY_TERMINAL_H_

#include <p18f4620.h>

void SCH_Send_Sche();
void setByTerminal();
unsigned char str_cmp(char str1[], char str2[], char n);

extern unsigned char statusTerminal;

#endif

