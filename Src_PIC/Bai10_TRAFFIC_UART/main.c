#include "main.h"
#include <string.h>
// Noi khai bao hang so

#define     ON      1
#define     OFF     0

#define     INIT    0
#define     INIT_SYSTEM        255

// Noi khai bao bien toan cuc

unsigned char statusOutput[8] = {0,0,0,0,0,0,0,0};
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);
void TestOutput(void);
void ReverseOutput(int index);
void Test_KeyMatrix();

void App_PasswordDoor();
unsigned char CheckPassword();
unsigned char isButtonNumber();
unsigned char numberValue;
unsigned char isButtonEnter();
void UnlockDoor();
void LockDoor();
// Den giao thong
unsigned char tp;
unsigned char k = 0;

void UartDataReceiveProcess();
void UartDataReceiveProcess_ElectronicDeviceControl();


//My code


#define WAIT                4
#define DISABLE_SCAN    0
#define ENABLE_SCAN     1

void UndoPage(void);

//SETTING MODE
#define DISPLAY_LIST        0
#define SET_TIME_LIGHT      1
#define SET_SCHEDULE        2
#define MANUAL_ADJUSTMENT   3
#define SLOW_MODE           4
#define SET_BY_UART         5
#define DISPLAY_TIME        6

void SettingMode();
unsigned char statusMode = DISPLAY_LIST;
unsigned char flagAccess = 0;
unsigned char flagDisableCursorLcd = 0;

// SETTING BY BUTTON
#define     SET_P1G             2
#define     SET_P1Y             3
#define     SET_P2G             4
#define     SET_P2Y             5
#define     DONE                6

#define     BTN_ENTER           6
#define     BTN_ADD             5
#define     BTN_SUB             4
#define     BTN_DONE            6

unsigned char statusofSETTING = INIT_SYSTEM;
unsigned int timecount = 0;

void SettingByButton();
unsigned char isButtonSwitch(int i);
void DisplayNumS(char row, char col, int num);

// Manual Adjustment------------------------------------------------------------

#define     INIT_MANUAL         0
#define     START_MANUAL        1
#define     WAIT_MANUAL         2
#define     STOP_MANUAL         3
#define     GREEN_LIGHT         4
#define     RED_LIGHT           5

unsigned char statusManuAdj = INIT_MANUAL;
unsigned char statusLight_Manual = GREEN_LIGHT;

#define     BTN_CHANGE          4
#define     BTN_START_STOP      5

unsigned char time_waiting = 0;
unsigned char flag_waiting = 0;
unsigned char flag_ManualAdj = 0;
unsigned char flag_Display = 0;

void ManualAdjustment();

//--------------------------------------------------------------------------------
// Slow Mode------------------------------------------------------------
#define     BTN_SWITCH          4
#define     SLOW_OFF            0
#define     SLOW_ON             1


void SlowMode();
unsigned int statusSlowMode =    SLOW_OFF;
//------------------------------------------------------------------------------
// LCD LIST
unsigned char isButtonPageUp();
unsigned char data[10];

schedule time1= {14,30,14,50,3,20,7,63};
///////////////

char cmdRec[30] = "";
char cmdRec_length =  0;
char flagNewCmdRec = 0; 

char cmd_SET_NOR[13]="!set-normal-";
char cmd_SET_MAN[13]="!set-manual-";
char cmd_STA_MAN[16]="!start-manual-";
char cmd_STO_MAN[16]="!stop-manual!";
char cmd_ADD_SCH[13]="!add-sch!";
char cmd_EDI_SCH[13]="!edit-sch!";
char cmd_DEL_SCH[16]="!delete-sch";
char cmd_STA_SLO[16]="!start-slow!";
char cmd_STO_SLO[16]="!stop-slow!";
char cmd_UPD[9]="!update-";
char cmd_UPD_ALL[5]="all!";
char cmd_UPD_NOR[8]="normal!";
char cmd_UPD_SCH[4]="sch-";
char cmd_UPD_STA[8]="status!";
//char cmd_UPD_ALL[13]="!update-all!";
//char cmd_UPD_NOR[16]="!update-normal!";
//char cmd_UPD_SCH[13]="!update-sch-";
//char cmd_UPD_STA[16]= "!update-status!";

#define PARSE_DATA  0
#define WAIT_DATA   1
#define EDIT 1
#define ADD  2

char flagControlByWeb = 0;
char statusUpdateDataWeb = PARSE_DATA ;
char flagAddOrEditSCH = 0 ;
void UpdateDataToWeb();
void UpdateNormal();
void UpdateSchToWeb(char index);

#define NORMAL      0
#define SCHEDULE    1
void UpdateStatusToWeb(char status);
void UpdateSlowModeToWeb(char status);

//#define EDIT    1
//#define ADD     2
//#define DELETE  3
//void UpdateAdjSchToWeb(char mode, char index);

#define GREEN   0
#define RED     1

void UpdateManualToWeb(char mode, char color);

unsigned char str_cmp2(char str1[], char str2[], char n)
{
    char i = 0;
    for(i = 0; i < n; i++)
    {
        if(str1[i] == str2[i])
            continue;
        
        return 1 ;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	init_system();
    delay_ms(1000);
    
	while (1)
	{
        while(flag_timer3 == 0)
        {
            if(flag_timer1 ==1)
            {
                flag_timer1 = 0;
                scan_led_7seg();
            }
        }
        {
            flag_timer3 = 0;      
            scan_key_matrix_with_uart();
//
            k = (k+1)%10;
            
            SettingMode();            
            AppTrafficLight( !flag_ManualAdj );      
//            
            SCH_Update();
            SCH_Dispatch();   
            UpdateDataToWeb();
//            LcdPrintCharS(0,0,13);
                       
            DisplayLcdScreenList(); 
        }
	}
}

// Hien thuc cac module co ban cua chuong trinh
void delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}

void init_system(void)
{
    init_output_led();
    lcd_init();
    init_key_matrix_with_uart();
    init_interrupt();
    init_i2c();
    init_uart();
    lcd_clear();
    LcdClearS();
    delay_ms(500);
    init_timer0(4695);//dinh thoi 1ms sai so 1%
    init_timer1(9390);//dinh thoi 2ms
    init_timer3(46950);//dinh thoi 10ms
    SetTimer0_ms(10);
    SetTimer1_ms(15);
    SetTimer3_ms(100); //Chu ky thuc hien viec xu ly input,proccess,output
    init_spi();
    TRISEbits.RE0=0;
    PORTEbits.RE0=0;
}

// My code 
unsigned char isButtonSwitch(int i)
{
    if (key_code[i] == 1||(key_code[i]>10&&(key_code[i]%2==0)))
        return 1;
    else
        return 0;   
}

void backToPrePage()
{
    LcdSetNumRow(5);
    flagDisableCursorLcd = 0;
    cursorSelect = statusMode - 1;
    statusMode = DISPLAY_LIST;
    LcdDisableCursor(0);
}
void DisplayNumS(char row, char col, int num)
{
    LcdPrintNumS(row,col,num/10);
     LcdPrintNumS(row,col+1,num%10);
}

void SettingByButton(){
    char isPressLong = 0;
    timecount=(timecount+1)%20;
    
    LcdDisableCursor(1);
    switch(statusofSETTING){
        case INIT_SYSTEM:
            LcdPrintStringS(0,0,"P1/G:    Y:         ");
            LcdPrintStringS(1,0,"P2/G:    Y:         ");
            DisplayNumS(1,5,timeOfGreenPhase2);
            DisplayNumS(1,11,timeOfYellowPhase2);             
            DisplayNumS(0,5,timeOfGreenPhase1);
            DisplayNumS(0,11,timeOfYellowPhase1);
            isPressLong = isButtonPressKeep(BTN_ENTER);
            if(isPressLong == BTN_PRESS_FAST){
                statusofSETTING=SET_P1G;
            }
            if(isPressLong == BTN_PRESS_LONG)
            {
                backToPrePage();
                LcdDisableCursor(0);
            }
            break;
        case SET_P1G:        
            if (timecount>10 && key_code[BTN_ADD] == 0 && key_code[BTN_SUB] == 0){
                LcdPrintStringS(0,5,"  ");
            }
            else{
                DisplayNumS(0,5,timeOfGreenPhase1);
            }
            if (isButton(BTN_ADD)){
                timeOfGreenPhase1++;
                DisplayNumS(0,5,timeOfGreenPhase1);
            }
            
            if (isButton(BTN_SUB)){
                timeOfGreenPhase1--;
                DisplayNumS(0,5,timeOfGreenPhase1);
            } 
            isPressLong = isButtonPressKeep(BTN_ENTER);
            if (isPressLong == BTN_PRESS_FAST){
                statusofSETTING=SET_P1Y;
                DisplayNumS(0,5,timeOfGreenPhase1);
            }
            if (isPressLong == BTN_PRESS_LONG){
                statusofSETTING= DONE;
            }
            break;
        case SET_P1Y:            
            if (timecount>10 && key_code[BTN_ADD] == 0 && key_code[BTN_SUB] == 0){
                LcdPrintStringS(0,11,"  ");
            }
            else{ 
                DisplayNumS(0,11,timeOfYellowPhase1);
            }
            
            if (isButton(BTN_ADD)){
                timeOfYellowPhase1++;
            }
            if (isButton(BTN_SUB)){
                timeOfYellowPhase1--;
            }
            isPressLong = isButtonPressKeep(BTN_ENTER);
            if (isPressLong == BTN_PRESS_FAST){
                statusofSETTING=SET_P2G;
                DisplayNumS(0,11,timeOfYellowPhase1);
            }
            if (isPressLong == BTN_PRESS_LONG){
                statusofSETTING= DONE;
            }
            break; 
        case SET_P2G:
            if (timecount>10 && key_code[BTN_ADD] == 0 && key_code[BTN_SUB] == 0){
                LcdPrintStringS(1,5,"  ");
            }
            else{
                DisplayNumS(1,5,timeOfGreenPhase2);
            }
            
            if (isButton(BTN_ADD)){
                timeOfGreenPhase2++;
            }
            if (isButton(BTN_SUB)){
                timeOfGreenPhase2--;
            }    
            isPressLong = isButtonPressKeep(BTN_ENTER);
            if (isPressLong == BTN_PRESS_FAST){
                DisplayNumS(1,5,timeOfGreenPhase2);
                statusofSETTING=SET_P2Y;
            }
            if (isPressLong == BTN_PRESS_LONG){
                statusofSETTING= DONE;
            }
            break;
        case SET_P2Y:
            if (timecount>10 && key_code[BTN_ADD] == 0 && key_code[BTN_SUB] == 0){
                LcdPrintStringS(1,11,"  ");
            }
            else{
                DisplayNumS(1,11,timeOfYellowPhase2);
            }
            
            if (isButton(BTN_ADD)){
                timeOfYellowPhase2++;
            }
            if (isButton(BTN_SUB)){
                timeOfYellowPhase2--;
            }
            isPressLong = isButtonPressKeep(BTN_ENTER);
            if (isPressLong == BTN_PRESS_FAST){
                DisplayNumS(1,11,timeOfYellowPhase2);
                statusofSETTING=DONE;
            }
            if (isPressLong == BTN_PRESS_LONG){
                statusofSETTING= DONE;
            }
            break; 
        case DONE:            
            UpdateNormal();
            statusofSETTING = INIT_SYSTEM;
            break;
        default:
            statusofSETTING=INIT_SYSTEM;
            break;
   } 
}
void ManualAdjustment() {
    if(k == 0 && time_waiting != 0)
    {  
        time_waiting -- ;        
    }
    LcdPrintNumS(0,14,time_waiting);
    LcdDisableCursor(1);
    switch (statusManuAdj) {
        case INIT_MANUAL:
            LcdPrintStringS(0,0,"Manual Adjust  ");
            switch(statusLight_Manual)
            {
                case GREEN_LIGHT:
                    LcdPrintStringS(1,0,"INIT  || GREEN  ");
                    if (isButton(BTN_CHANGE))
                    {
                        statusLight_Manual = RED_LIGHT;
                    }
                    break;
                case RED_LIGHT:
                    LcdPrintStringS(1,0,"INIT  || RED    ");
                    if (isButton(BTN_CHANGE))
                    {
                        statusLight_Manual = GREEN_LIGHT;
                    }
                    break;
            }
            if (isButton(BTN_START_STOP) || (flagControlByWeb == 1)){
                if(flagControlByWeb == 0)
                    UpdateManualToWeb(1,statusLight_Manual);
                flagControlByWeb = 0;
                statusManuAdj = START_MANUAL;
            }
            if(isButton(BTN_ENTER))
                backToPrePage();
            break;
        case START_MANUAL:
            LcdPrintStringS(1,0,"WAITING...     ");
            Phase1_GreenOff();
            Phase1_RedOff();
            Phase2_GreenOff();
            Phase2_RedOff();
            flag_ManualAdj = 1 ;
            flag_EnLed7SEG = 0 ;
            time_waiting = 3;
            statusManuAdj = WAIT_MANUAL;
            break;
        case WAIT_MANUAL:
            LcdPrintStringS(1,0,"WAITING...     ");
            Phase1_YellowOn();
            Phase2_YellowOn();
            if(time_waiting == 0)
            {
                if(flag_waiting == 2)
                    statusManuAdj = STOP_MANUAL;
                else
                {
                    Phase1_YellowOff();
                    Phase2_YellowOff();
                    statusManuAdj = statusLight_Manual;
                }
            }                
            break;
        case RED_LIGHT:
            LcdPrintStringS(1,0,"START || RED       ");
            if(flag_waiting == 0)
            {
                Phase1_YellowOff();
                Phase1_RedOn();
                Phase2_RedOff();
                Phase2_GreenOn();
            }       
            if(isButton(BTN_CHANGE)|| (flagControlByWeb == 1))
            {
                if(flagControlByWeb == 0)
                    UpdateManualToWeb(2,GREEN_LIGHT);
                flagControlByWeb = 0;
                time_waiting = 3 ;                
                flag_waiting = 1;
                Phase2_GreenOff();
                Phase2_YellowOn();
            }
            if(flag_waiting == 1 && time_waiting == 0)
            {
                flag_waiting = 0;
                statusManuAdj = GREEN_LIGHT;
            }
            if (isButton(BTN_START_STOP)){
                Phase1_RedOff();
                Phase2_GreenOff();
                flag_waiting = 2;
                time_waiting = 3;
                statusManuAdj=WAIT_MANUAL;
            }
            break;
        case GREEN_LIGHT:
            LcdPrintStringS(1,0,"START || GREEN       ");
            if( flag_waiting == 0)
            {
                Phase1_RedOff();
                Phase1_GreenOn();
                Phase2_YellowOff();
                Phase2_RedOn();
            }            
             if(isButton(BTN_CHANGE)|| (flagControlByWeb == 1))
            { 
                 if(flagControlByWeb == 0)
                    UpdateManualToWeb(2,RED_LIGHT);
                flagControlByWeb = 0;
                time_waiting = 3 ;                
                flag_waiting = 1;
                Phase1_GreenOff();
                Phase1_YellowOn();
            }
            if(flag_waiting == 1 && time_waiting == 0)
            {
                flag_waiting = 0;
                statusManuAdj = RED_LIGHT;
            }            
            if (isButton(BTN_START_STOP)){
                Phase2_RedOff();
                Phase1_GreenOff();
                flag_waiting = 2;
                time_waiting = 3;
                statusManuAdj=WAIT_MANUAL;
            }
            break;        
        case STOP_MANUAL:
            LcdPrintStringS(1,0,"STOP                  ");
            flag_waiting = 0;
            time_waiting = 0;
            // OUT MANUAL ADJUSTMENT
            flag_ManualAdj = 0 ;
            flag_EnLed7SEG = 1 ;
            statusOfLight = INIT_SYSTEM;
            statusManuAdj=INIT_MANUAL;
            if(flagControlByWeb == 0)
                UpdateManualToWeb(0,statusLight_Manual);
            if(flagControlByWeb){                
                flagControlByWeb = 0;
                backToPrePage();
            }
            break;
        default:
            break;         
    }
}
void SlowMode(){
     if(k == 0 && time_waiting != 0)
    {  
        time_waiting -- ;        
    }
    LcdPrintStringS(0,0,"SLOW MODE:         ");
    LcdPrintStringS(1,0,"                   ");
    LcdDisableCursor(1);
    switch(statusSlowMode){
        case SLOW_OFF:
            LcdPrintStringS(0,11,"OFF     ");
            if (isButton(BTN_SWITCH) || (flagControlByWeb == 1)){
                if(flagControlByWeb == 0)
                    UpdateSlowModeToWeb(1);
                flagControlByWeb = 0;
                Phase1_GreenOff();
                Phase1_RedOff();
                Phase1_YellowOff();
                Phase2_GreenOff();
                Phase2_RedOff();
                Phase2_YellowOff();
                flag_ManualAdj = 1;
                flag_EnLed7SEG = 0 ;
                
                statusSlowMode=SLOW_ON;
                break;
            }
            if(isButton(BTN_ENTER))
            {
                backToPrePage();
            }
                
            break;
        case SLOW_ON:
            LcdPrintStringS(0,11,"ON     ");
            if(k < 5)
            {
                Phase1_YellowOff();
                Phase2_YellowOff();
            }
            else
            {
                Phase1_YellowOn();
                Phase2_YellowOn();
            }
            if (isButton(BTN_SWITCH)|| (flagControlByWeb == 1)){
                if(flagControlByWeb == 0)                    
                   UpdateSlowModeToWeb(0);
                Phase1_YellowOn();
                Phase2_YellowOn();
                time_waiting = 3 ;                
                flag_waiting = 1;
                statusSlowMode= WAIT;
            }
            break;
        case WAIT:
            LcdPrintNumS(0,11,time_waiting);
            if(flag_waiting == 1 && time_waiting == 0)
            {               
                flag_waiting = 0;
                Phase1_YellowOff();
                Phase2_YellowOff();
                flag_ManualAdj = 0 ;
                flag_EnLed7SEG = 1 ;
                statusOfLight = INIT_SYSTEM;
                
                statusSlowMode = SLOW_OFF;
                if(flagControlByWeb == 1)
                {
                    flagControlByWeb = 0;
                    backToPrePage();
                }                    
            }            
            break;
           
    }
}
void SettingMode(){
    char isBack = 0;
    switch(statusMode)
    {        
        case DISPLAY_LIST:
            LcdSetNumRow(5);
            LcdPrintStringS(0,0,"1.Set TimeLight");
            LcdPrintStringS(1,0,"2.Set Schedule ");
            LcdPrintStringS(2,0,"3.Manual Adj   ");
            LcdPrintStringS(3,0,"4.Slow mode    ");
            LcdPrintStringS(4,0,"5.Terminal     ");
            if(isButtonPressKeep(BTN_ENTER)==BTN_PRESS_FAST)
            {
                statusMode = DISPLAY_LIST + cursorSelect + 1;
                cursorSelect = 0;
                LcdClearS();
                delay_ms(10);
            }
            break;
        case SET_TIME_LIGHT:
            LcdSetNumRow(2);
            SettingByButton();
            break;
        case SET_SCHEDULE:
            ScheduleTime(&isBack);
            if(isBack == 1)
            {
                LcdSetNumRow(5);
                flagDisableCursorLcd = 0;
                cursorSelect = statusMode - 1;
                statusMode = DISPLAY_LIST;
            }
            break;
        case MANUAL_ADJUSTMENT:
            LcdSetNumRow(2);
            ManualAdjustment();
            break;
        case SLOW_MODE:
            LcdSetNumRow(2);
            SlowMode();
            break;
        case SET_BY_UART:
            LcdSetNumRow(2);
            LcdDisableCursor(1);
            LcdPrintStringS(0,0,"Set by Terminal      ");
            LcdPrintStringS(1,0,"...                   ");
            setByTerminal();
            if(isButton(BTN_ENTER))
            {
                statusTerminal = INIT;
                backToPrePage();
            }
            break;            
    }
}


void UpdateDataToWeb()
{
    schedule tmp={0};
    char cnt_i = 0 ;
    char index = 0;
    flagNewCmdRec = UartGetString(cmdRec,&cmdRec_length);
    if(flagNewCmdRec == 1)
    {
        flagNewCmdRec = 0 ; 
        
        switch(statusUpdateDataWeb)
        {
            case PARSE_DATA:
                // !set-normal-[40,5,35,3]!
            if(str_cmp(cmdRec,cmd_SET_NOR,12) == 0)
            {
//                UartSendString("!OK!\n");
                UartSendString("!");
                UartSendString("OK");
                UartSendString("!");
                UartSendString("\n");
                timeOfGreenPhase1 = (cmdRec[13]-'0')*10+(cmdRec[14]-'0');
                timeOfYellowPhase1 = (cmdRec[16]-'0');
                timeOfGreenPhase2 = (cmdRec[18]-'0')*10+(cmdRec[19]-'0');
                timeOfYellowPhase2 = (cmdRec[21]-'0');
            }
            // !start-manual-green!
            if(str_cmp(cmdRec,cmd_STA_MAN,12) == 0)
            {
                UartSendString("!OK!\n");
                flagControlByWeb = 1 ;
                statusMode = MANUAL_ADJUSTMENT;
                if(cmdRec[14] == 'r')
                    statusLight_Manual = RED_LIGHT;
                if(cmdRec[14] == 'g')
                    statusLight_Manual = GREEN_LIGHT;
            }
            // !set-manual-green!
            if(str_cmp(cmdRec,cmd_SET_MAN,12) == 0)
            {
                UartSendString("!OK!\n");
                flagControlByWeb = 1 ;
                statusMode = MANUAL_ADJUSTMENT;
                if(cmdRec[12] == 'r')
                    statusLight_Manual = RED_LIGHT;
                if(cmdRec[12] == 'g')
                    statusLight_Manual = GREEN_LIGHT;
            }
            //!stop-manual!
             if(str_cmp(cmdRec,cmd_STO_MAN,13) == 0)
            {
                UartSendString("!OK!\n");
                flagControlByWeb = 1 ;
                Phase1_RedOff();
                Phase2_GreenOff();
                Phase2_RedOff();
                Phase1_GreenOff();
                flag_waiting = 2;
                time_waiting = 3;
                statusManuAdj=WAIT_MANUAL;
            }
            // !start-slow!
             if(str_cmp(cmdRec,cmd_STA_SLO,12) == 0)
            {
                UartSendString("!OK!\n");
                statusMode = SLOW_MODE;
                flagControlByWeb = 1 ;
            }
            // !stop-slow!
             if(str_cmp(cmdRec,cmd_STO_SLO,11) == 0)
            {
                UartSendString("!OK!\n");
                flagControlByWeb = 1 ;
            }
            // !add-sch-[1,"00:00","00:00","00","00","00","00"]!
            // !add-sch-[2,"00:00"","00:00","00","00","00","00"]!
             if(str_cmp(cmdRec,cmd_ADD_SCH,9) == 0)
            {
                UartSendString("!OK1!\n");
                flagAddOrEditSCH = ADD;
                statusUpdateDataWeb = WAIT_DATA;
                break;
            }
             // !edit-sch-[0,"00:30","6:00","00","00","00","00"]!
             if(str_cmp(cmdRec,cmd_EDI_SCH,10) == 0)
            {
                 UartSendString("!OK1!\n");
                flagAddOrEditSCH = EDIT;
                statusUpdateDataWeb = WAIT_DATA;
                break;               
            }
            // !delete-sch-[0]!
             if(str_cmp(cmdRec,cmd_DEL_SCH,11) == 0)
            {
                UartSendString("!OK!\n");
                index = cmdRec[12]-'0';           
                SCH_Delete(index);
            }
            if(str_cmp(cmdRec,cmd_UPD,8) == 0)
            {
                for (cnt_i = 0; cnt_i < 8; cnt_i++) {
                    cmdRec[cnt_i] = cmdRec[8+cnt_i];
                }
                if(str_cmp(cmdRec,cmd_UPD_ALL,4) == 0)
                {
                    UartSendString("!OK!\n");
                }
                if(str_cmp(cmdRec,cmd_UPD_STA,7) == 0)
                {
                    UpdateStatusToWeb(statusMode);
                }
//                !update-normal!
                if(str_cmp(cmdRec,cmd_UPD_NOR,7) == 0)
                {
                    UpdateNormal();
                }
                //!update-sch-0!
                if(str_cmp(cmdRec,cmd_UPD_SCH,4) == 0)
                {
                    index = cmdRec[12]-'0'; 
                    UpdateSchToWeb(index);
                }

            }
//            //!update-all!
//            if(str_cmp(cmdRec,cmd_UPD_ALL,12) == 0)
//            {
//                UartSendString("!OK!\n");
//            }
//            
//            //!update-normal!
//            if(str_cmp(cmdRec,cmd_UPD_NOR,15) == 0)
//            {
//                UpdateNormal();
//            }
//            //!update-sch-0!
//            if(str_cmp(cmdRec,cmd_UPD_SCH,12) == 0)
//            {
//                index = cmdRec[12]-'0'; 
//                UpdateSchToWeb(index);
//            }
            
                break;
            case WAIT_DATA:                
                tmp.time_start_hour = (cmdRec[4]-'0')*10 + cmdRec[5]-'0';
                tmp.time_start_minute = (cmdRec[7]-'0')*10 + cmdRec[8]-'0';
                tmp.time_finish_hour = (cmdRec[12]-'0')*10+ cmdRec[13]-'0';
                tmp.time_finish_minute = (cmdRec[15]-'0')*10 + cmdRec[16]-'0';
                tmp.time_green1 = (cmdRec[19]-'0')*10 + cmdRec[20]-'0';
                tmp.time_yellow1 = cmdRec[22]-'0';
                tmp.time_green2 = (cmdRec[24]-'0')*10 + cmdRec[25]-'0';
                tmp.time_yellow2 = cmdRec[27]-'0';   
                
//                UartSendString("!OK!\n");
                //[1,"00:00","00:00",0,0,0,0]
                if(flagAddOrEditSCH == ADD)
                {       
                    flagAddOrEditSCH = 0;
                    SCH_AddTime(tmp);     
                    UartSendString("!OK!\n");
                }
                 if(flagAddOrEditSCH == EDIT)
                {
                    flagAddOrEditSCH = 0;
                    index = cmdRec[1]-'0';
                    SCH_EditTime(tmp,index);     
                    UartSendString("!OK!\n");
                }
                statusUpdateDataWeb = PARSE_DATA;
            break;
        }
    }
}

void UpdateNormal()
{
    //{"cmd":"update","check":0,"status":"normal","paras":1,"valueTime":[40,3,35,3]}
    UartSendString("{\"cmd\":\"update\",\"check\":0,\"status\":\"normal\",\"paras\":1,\"valueTime\":[");
    UartSendNum(timeOfGreenPhase1);
    UartSendString(",");
    UartSendNum(timeOfYellowPhase1);
    UartSendString(",");
    UartSendNum(timeOfGreenPhase2);
    UartSendString(",");
    UartSendNum(timeOfYellowPhase2);
    UartSendString("]}");
    UartSendString("\n");
}
void UpdateSchToWeb(char index)
{
    //{"cmd":"update","check":0,"status":"sch","paras":1,"valueSch":[4,"12:30","14:30",40,3,35,3]}
    UartSendString("{\"cmd\":\"update\",\"check\":");
    UartSendNum(index+1);
    UartSendString(",\"status\":\"sch\",\"paras\":1,\"valueSch\":[");
    UartSendNum(SCH_count);
    UartSendString(",\"");
    UartSendNum(SCH_Time[index].time_start_hour/10);
    UartSendNum(SCH_Time[index].time_start_hour%10);
    UartSendString(":");
    UartSendNum(SCH_Time[index].time_start_minute/10);
    UartSendNum(SCH_Time[index].time_start_minute%10);
    UartSendString("\",\"");
    UartSendNum(SCH_Time[index].time_finish_hour/10);
    UartSendNum(SCH_Time[index].time_finish_hour%10);
    UartSendString(":");
    UartSendNum(SCH_Time[index].time_finish_minute/10);
    UartSendNum(SCH_Time[index].time_finish_minute%10);
    UartSendString("\",");
    UartSendNum(SCH_Time[index].time_green1/10);
    UartSendNum(SCH_Time[index].time_green1%10);
    UartSendString(",");
    UartSendNum(SCH_Time[index].time_yellow1/10);
    UartSendNum(SCH_Time[index].time_yellow1%10);
    UartSendString(",");
    UartSendNum(SCH_Time[index].time_green2/10);
    UartSendNum(SCH_Time[index].time_green2%10);
    UartSendString(",");
    UartSendNum(SCH_Time[index].time_yellow2/10);
    UartSendNum(SCH_Time[index].time_yellow2%10);
    UartSendString("]}");
    UartSendString("\n");
}

void UpdateStatusToWeb(char status)
{
    // {"status":"status","paras":1,"valueStatus":"normal"}
    UartSendString("{\"cmd\":\"update\",\"check\":0,\"status\":\"status\",\"paras\":1,\"valueStatus\":\"");
    if(status == DISPLAY_LIST)
        UartSendString("normal");
    if(status == SCHEDULE)
        UartSendString("sch");
    if(status == MANUAL_ADJUSTMENT)
        UartSendString("manual");
    if(status == SLOW_MODE)
        UartSendString("slow");
    UartSendString("\"}");
    UartSendString("\n");
}
void UpdateSlowModeToWeb(char status)
{
    // {"status":"slow","paras":1,"valueStatus":"start"}
    UartSendString("{\"status\":\"slow\",\"paras\":1,\"valueStatus\":\"");
    if(status == 0)
        UartSendString("stop");
    if(status == 1)
        UartSendString("start");
    
    UartSendString("\"}");
    UartSendString("\n");
}

void UpdateManualToWeb(char mode, char color)
{
    UartSendString("{\"status\":\"manual\",\"paras\":2,\"valueStatus\":\"");
    
    if(mode == 0)
        UartSendString("stop");
    if(mode == 1)
        UartSendString("start");
    if(mode == 2)
        UartSendString("set");
    UartSendString("\",\"valueColor\":\"");
    if(color == GREEN_LIGHT)
        UartSendString("green");
    if(color == RED_LIGHT)
        UartSendString("red");
    UartSendString("\"}");
    UartSendString("\n");
}
//void UpdateAdjSchToWeb(char mode, char index)
//{
//    //{"status":"sch","paras":2,"valueMode":"add","valueSch":[3,"12:30","14:30",40,3,25,3]}
//    UartSendString("\"status\":\"sch\",\"paras\":2,\"valueMode\":\"");
//    if(mode == ADD)
//        UartSendString("add");
//    if(mode == EDIT)
//        UartSendString("edit");
//    if(mode == DELETE)
//    {
//        UartSendString("delete");
//        UartSendString("\",\"valueSch\":[");
//        UartSendNum(index);
//    }
//    else
//    {
//        UartSendString("\",\"valueSch\":[");
//        UartSendNum(index);
//        UartSendString(",\"");
//        UartSendNum(SCH_Time[index].time_start_hour/10);
//        UartSendNum(SCH_Time[index].time_start_hour%10);
//        UartSendString(":");
//        UartSendNum(SCH_Time[index].time_start_minute/10);
//        UartSendNum(SCH_Time[index].time_start_minute%10);
//        UartSendString("\",\"");
//        UartSendNum(SCH_Time[index].time_finish_hour/10);
//        UartSendNum(SCH_Time[index].time_finish_hour%10);
//        UartSendString(":");
//        UartSendNum(SCH_Time[index].time_finish_minute/10);
//        UartSendNum(SCH_Time[index].time_finish_minute%10);
//        UartSendString("\",");
//        UartSendNum(SCH_Time[index].time_green1/10);
//        UartSendNum(SCH_Time[index].time_green1%10);
//        UartSendString(",");
//        UartSendNum(SCH_Time[index].time_yellow1/10);
//        UartSendNum(SCH_Time[index].time_yellow1%10);
//        UartSendString(",");
//        UartSendNum(SCH_Time[index].time_green2/10);
//        UartSendNum(SCH_Time[index].time_green2%10);
//        UartSendString(",");
//        UartSendNum(SCH_Time[index].time_yellow2/10);
//        UartSendNum(SCH_Time[index].time_yellow2%10);
//    }
//    UartSendString("]}");
//    UartSendString("\n");
//}
