#include "traffic_led.h"
#include "..\spi\spi.h"

#define     LED     PORTD
#define     PORTD_OUT   PORTD
#define     TRISD_OUT   TRISD

#define     INIT_SYSTEM         255
#define     PHASE1_GREEN        0
#define     PHASE1_YELLOW       1
#define     PHASE2_GREEN        2
#define     PHASE2_YELLOW       3
#define     WAIT                4

unsigned char arrayMapOfOutput [8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char statusOfLight = INIT_SYSTEM;
unsigned char timeOfGreenPhase1 = 25;
unsigned char timeOfYellowPhase1 = 3;
unsigned char timeOfGreenPhase2 = 20;
unsigned char timeOfYellowPhase2 = 3;
unsigned char timeOfLight = 0;
unsigned char timeOfLight2 = 0;
unsigned char cntOfLight = 0;

void init_output_led(void)
{
    TRISD_OUT = 0x00;
    PORTD_OUT = 0x00;      
}

void OpenOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		PORTD_OUT = PORTD_OUT | arrayMapOfOutput[index];
	}
}

void CloseOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		PORTD_OUT = PORTD_OUT & ~arrayMapOfOutput[index];
	}
}

void Phase1_GreenOn()
{
    CloseOutput(0);
}
void Phase1_GreenOff()
{
    OpenOutput(0);
}

void Phase1_YellowOn()
{
    CloseOutput(1);
}
void Phase1_YellowOff()
{
    OpenOutput(1);
}

void Phase1_RedOn()
{
    CloseOutput(2);
}
void Phase1_RedOff()
{
    OpenOutput(2);
}

void Phase2_GreenOn()
{
    CloseOutput(3);
}
void Phase2_GreenOff()
{
    OpenOutput(3);
}

void Phase2_YellowOn()
{
    CloseOutput(4);
}
void Phase2_YellowOff()
{
    OpenOutput(4);
}

void Phase2_RedOn()
{
    CloseOutput(5);
}
void Phase2_RedOff()
{
    OpenOutput(5);
}

void AppTrafficLight(char enable)
{
    if(enable == 0)
        return ;
    cntOfLight = (cntOfLight + 1)%10;
    if (cntOfLight == 0)
    {
        timeOfLight --;
        timeOfLight2 --;
    }
    switch (statusOfLight)
    {
        case INIT_SYSTEM:
            Phase1_GreenOff();
            Phase1_RedOff();
            Phase1_YellowOff();
            Phase2_GreenOff();
            Phase2_RedOff();
            Phase2_YellowOff();
            timeOfLight = timeOfGreenPhase1;
            timeOfLight2 = timeOfGreenPhase1 + timeOfYellowPhase1;
            statusOfLight = PHASE1_GREEN;
            break;
        case PHASE1_GREEN:
            Phase1_GreenOn();
            Phase1_RedOff();
            Phase2_RedOn();
            
            update_led_7seg(timeOfLight,timeOfLight2);
            
            if (timeOfLight == 0)
            {
                statusOfLight = PHASE1_YELLOW;
                Phase1_GreenOff();
                timeOfLight = timeOfYellowPhase1;
            }
            break;
        case PHASE1_YELLOW:
            Phase1_YellowOn();
            Phase2_RedOn();
            update_led_7seg(timeOfLight,timeOfLight2);
            
            if (timeOfLight == 0)
            {
                statusOfLight = PHASE2_GREEN;
                Phase1_YellowOff();
                timeOfLight = timeOfGreenPhase2;
                timeOfLight2 = timeOfGreenPhase2 + timeOfYellowPhase2;
            }
            break;
        case PHASE2_GREEN:
            Phase2_GreenOn();
            Phase2_RedOff();
            Phase1_RedOn();
            update_led_7seg(timeOfLight2,timeOfLight);
            
            if (timeOfLight == 0)
            {
                statusOfLight = PHASE2_YELLOW;
                Phase2_GreenOff();
                timeOfLight = timeOfYellowPhase2;
            }
            break;
        case PHASE2_YELLOW:
            Phase2_YellowOn();
            Phase1_RedOn();
            update_led_7seg(timeOfLight2,timeOfLight);
            
            if (timeOfLight == 0)
            {
                statusOfLight = PHASE1_GREEN;
                Phase2_YellowOff();
                timeOfLight = timeOfGreenPhase1;
                timeOfLight2 = timeOfGreenPhase1 + timeOfYellowPhase1;
            }
            break;
        case WAIT:
            Phase1_GreenOff();
            Phase2_GreenOff();
            Phase1_RedOff();
            Phase2_RedOff();
            if (cntOfLight<=12)
            {
                Phase1_YellowOn();
                Phase2_YellowOn();
            }
            else
            {
                Phase1_YellowOff();
                Phase2_YellowOff();
            }
            break;
        default:
            statusOfLight = PHASE1_GREEN;
            break;
    }
}




