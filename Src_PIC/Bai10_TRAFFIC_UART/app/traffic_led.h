#ifndef _TRAFFIC_LED_H_
#define _TRAFFIC_LED_H_

#include <p18f4620.h>

void Phase1_GreenOn();
void Phase1_GreenOff();
void Phase1_YellowOn();
void Phase1_YellowOff();
void Phase1_RedOn();
void Phase1_RedOff();

void Phase2_GreenOn();
void Phase2_GreenOff();
void Phase2_YellowOn();
void Phase2_YellowOff();
void Phase2_RedOn();
void Phase2_RedOff();


void OpenOutput(int index);
void CloseOutput(int index);

void init_output_led(void);
void AppTrafficLight(char enable);



extern unsigned char timeOfGreenPhase1, timeOfYellowPhase1, timeOfGreenPhase2, timeOfYellowPhase2,statusOfLight;
#endif

