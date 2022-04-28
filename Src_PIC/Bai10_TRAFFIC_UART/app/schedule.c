#include "schedule.h"
#include "..\lcd\lcd.h"
#include "..\button_matrix\button.h"
#include "..\app\time.h"
#include "..\app\traffic_led.h"
#include "..\uart\uart.h"

#define     INIT        0
#define     DISPLAY     1
#define     SET_TIME    2
#define     SAVE        3
#define     DELETE      4


#define     BTN_ENTER   6
#define     BTN_BACK    6
#define     BTN_INC     5
#define     BTN_DEC     4


#define     ADD     0
#define     EDIT    1
void UpdateAdjSchToWeb(char mode, char index);

unsigned char statusSCH = INIT ;
unsigned char SCH_count = 0;
unsigned char idx_time_set = 0;
unsigned char flagAddNewTime = 0;
char cnt = 0;

// Set time
#define     START_TIME          0
#define     FINISH_TIME         1
#define     PHASE1_TIME         2
#define     PHASE2_TIME         3

unsigned char statusSetTime = START_TIME;

#define     SET_NUM_1          0
#define     SET_NUM_2          1

unsigned char statusSetNum = SET_NUM_1;

schedule SCH_Time[SCH_MAX_NUM]={0};
schedule timeTemp ={0};

unsigned char flagSCH_Run = 0;
unsigned char flagSCH_Wait = 0;
unsigned char indexSCH_select = 11;
schedule scheDefault = {0};

unsigned char flagChangeData = 1 ;


void Format_time(char row, char col, char num1, char num2)
{
    LcdPrintNumS(row,col,num1/10);
    LcdPrintNumS(row,col+1,num1%10);
    LcdPrintStringS(row,col+2,":");
    LcdPrintNumS(row,col+3,num2/10);
    LcdPrintNumS(row,col+4,num2%10);
}

void blinkNum(char row, char col, char num)
{
    cnt = (cnt + 1)% 20;
    if(cnt < 10 && key_code[BTN_INC] == 0 &&  key_code[BTN_DEC] == 0)
    {
        LcdPrintStringS(row, col, "  ");
    }
    else
    {
        LcdPrintNumS(row,col,num/10);
        LcdPrintNumS(row,col+1,num%10);
    }
}
void SCH_Delete(char index)
{
    int i = 0;
    schedule tmp = {0};
    if(index < SCH_count)
    {
        for(i = index ; i <SCH_count - 1; i ++)
        {
            SCH_Time[i] = SCH_Time[i+1];
        }
        SCH_Time[i] = tmp;
        SCH_count -- ;
        flagChangeData = 1;
    }
}

void SCH_AddTime(schedule time)
{
    SCH_Time[SCH_count].time_start_hour = time.time_start_hour;
    SCH_Time[SCH_count].time_finish_hour = time.time_finish_hour;
    SCH_Time[SCH_count].time_start_minute = time.time_start_minute;
    SCH_Time[SCH_count].time_finish_minute = time.time_finish_minute;
    SCH_Time[SCH_count].time_green1 = time.time_green1;
    SCH_Time[SCH_count].time_green2 = time.time_green2;
    SCH_Time[SCH_count].time_yellow1 = time.time_yellow1;
    SCH_Time[SCH_count].time_yellow2 = time.time_yellow2;
    SCH_count ++;
    flagChangeData = 1;
}
void SCH_EditTime(schedule time, char index)
{
    SCH_Time[index].time_start_hour = time.time_start_hour;
    SCH_Time[index].time_finish_hour = time.time_finish_hour;
    SCH_Time[index].time_start_minute = time.time_start_minute;
    SCH_Time[index].time_finish_minute = time.time_finish_minute;
    SCH_Time[index].time_green1 = time.time_green1;
    SCH_Time[index].time_green2 = time.time_green2;
    SCH_Time[index].time_yellow1 = time.time_yellow1;
    SCH_Time[index].time_yellow2 = time.time_yellow2;
    flagChangeData = 1;
}
void ScheduleTime(char* isBack)
{
    int i = 0;
    char isLongPress = 0 ;
    *isBack = 0;
    switch(statusSCH)
    {
        case INIT:
            LcdDisableCursor(0);
            LcdSetNumRow(SCH_count + 1);
            if(flagChangeData == 1)
            {
                flagChangeData = 0;
                cursorSelect = idx_time_set;
                for(i = 0;  i < SCH_count; i++)
                {
                    LcdPrintNumS(i,0,i+1);
                    LcdPrintStringS(i,1,".");
                    Format_time(i,2,SCH_Time[i].time_start_hour,SCH_Time[i].time_start_minute);
                    LcdPrintStringS(i,7,"->");
                    Format_time(i,9,SCH_Time[i].time_finish_hour,SCH_Time[i].time_finish_minute);
                }      
            }                      
            LcdPrintStringS(SCH_count,0,"Add new time...   ");
            isLongPress = isButtonPressKeep(BTN_ENTER);
            if(isLongPress == BTN_PRESS_FAST)
            {
                idx_time_set = cursorSelect;
                timeTemp = SCH_Time[idx_time_set];
                if(cursorSelect == SCH_count)
                    flagAddNewTime = 1;
                statusSCH = DISPLAY ;
                LcdClearS();                
            }            
            if(isLongPress == BTN_PRESS_MID && cursorSelect != SCH_count )
            {
                idx_time_set = cursorSelect;
                statusSCH = DELETE ;
            }
            if(isLongPress == BTN_PRESS_LONG)
            {
                *isBack = 1;
                flagChangeData = 1;
            }
            break;
        case DISPLAY:
            LcdDisableCursor(0);
            LcdSetNumRow(4);
            LcdPrintStringS(0,0,"From: ");
            Format_time(0,6,timeTemp.time_start_hour,timeTemp.time_start_minute);
            LcdPrintStringS(1,0,"To: ");
            Format_time(1,6,timeTemp.time_finish_hour,timeTemp.time_finish_minute);
            LcdPrintStringS(2,0,"P1: G:   Y:    ");
            LcdPrintNumS(2,6,timeTemp.time_green1/10);
            LcdPrintNumS(2,7,timeTemp.time_green1%10);
            LcdPrintNumS(2,11,timeTemp.time_yellow1/10);
            LcdPrintNumS(2,12,timeTemp.time_yellow1%10);
            
            LcdPrintStringS(3,0,"P2: G:   Y:    " );
            LcdPrintNumS(3,6,timeTemp.time_green2/10);
            LcdPrintNumS(3,7,timeTemp.time_green2%10);
            LcdPrintNumS(3,11,timeTemp.time_yellow2/10);
            LcdPrintNumS(3,12,timeTemp.time_yellow2%10);
            
            isLongPress = isButtonPressKeep(BTN_ENTER);
            if(isLongPress == BTN_PRESS_FAST)
            {
                statusSetTime = cursorSelect ;
                statusSCH = SET_TIME ;
            }     
            if(isLongPress == BTN_PRESS_LONG)
            {
                statusSCH = SAVE ;
                LcdClearS();
            }     
            break;
        case SET_TIME:
            LcdDisableCursor(1);
            SCH_SetTime();
            isLongPress = isButtonPressKeep(BTN_ENTER);
            if(isLongPress == BTN_PRESS_LONG)
            {
                LcdDisableCursor(0);
                statusSCH = SAVE ;
                LcdClearS();
            }     
            break;
        case SAVE:
            LcdSetNumRow(2);
            LcdDisableCursor(1);
            LcdPrintStringS(0,0,"Save time ?     ");
            LcdPrintStringS(1,0,"Yes(5) No(4)    ");
            if(isButton(5))
            {
                if(flagAddNewTime == 1)
                {
                    flagAddNewTime = 0;
                    SCH_AddTime(timeTemp);
                    UpdateAdjSchToWeb(ADD,SCH_count-1);
                }
                else
                {
                    SCH_Time[idx_time_set] = timeTemp;
                    UpdateAdjSchToWeb(EDIT,idx_time_set);
                }
                flagChangeData = 1;
                LcdDisableCursor(0);
                statusSCH = INIT ;
            }
            if(isButton(4))
            {
                flagChangeData = 1;
                LcdDisableCursor(0);
                statusSCH = INIT ;
            }
            break;
        case DELETE:
            LcdSetNumRow(2);
            LcdDisableCursor(1);            
            LcdPrintStringS(0,0,"Delete time ?    ");
            LcdPrintStringS(1,0,"Yes(5) No(4)     ");
            if(isButton(5)){
                UpdateAdjSchToWeb(DELETE,idx_time_set);
                SCH_Delete(idx_time_set);
                flagChangeData = 1;
                LcdDisableCursor(0);    
                statusSCH = INIT ;
            }
            if(isButton(4))
            {
                LcdDisableCursor(0);    
                flagChangeData = 1;
                statusSCH = INIT ;
            }
            break;
    }
}

void SCH_SetTime()
{
    switch(statusSetTime)
    {
        case START_TIME:
            setTime(0,6,9,&timeTemp.time_start_hour,&timeTemp.time_start_minute,23,59);
            break;
        case FINISH_TIME:
            setTime(1,6,9,&timeTemp.time_finish_hour,&timeTemp.time_finish_minute,23,59);
            break;
        case PHASE1_TIME:
            setTime(2,6,11,&timeTemp.time_green1,&timeTemp.time_yellow1,99,99);            
            break;
        case PHASE2_TIME:
            setTime(3,6,11,&timeTemp.time_green2,&timeTemp.time_yellow2,99,99);
            break;
    }
    if(timeTemp.time_finish_hour < timeTemp.time_start_hour )
        timeTemp.time_finish_hour = timeTemp.time_start_hour ;
    if(timeTemp.time_finish_hour == timeTemp.time_start_hour )
    {
        if(timeTemp.time_finish_minute < timeTemp.time_start_minute )
            timeTemp.time_finish_minute = timeTemp.time_start_minute ;
    }
}

void setTime(char row, char col1, char col2,unsigned int *num1,unsigned int *num2, char maxNum1, char maxNum2 )
{
    
    LcdDisableCursor(1);
    
    switch(statusSetNum)
    {
        case SET_NUM_1:            
            if(isButton(BTN_INC))
            {
                *num1 = (*num1 + 1 )% (maxNum1 + 1);
            }
            if(isButton(BTN_DEC))           
            {
                if(*num1 == 0)
                    *num1 = maxNum1;
                else
                    (*num1) -= 1;           
            }
            blinkNum(row, col1,*num1);
            if(isButtonPressKeep(BTN_ENTER) == BTN_PRESS_FAST)
            {
                LcdPrintNumS(row, col1,(*num1)/10);
                LcdPrintNumS(row, col1+1,(*num1)%10);
                statusSetNum = SET_NUM_2;
            }
            break;
        case SET_NUM_2:         
            if(isButton(BTN_INC))
            {
                *num2 = (*num2 + 1 )% (maxNum2 + 1);
            }                
            if(isButton(BTN_DEC))           
            {
                if(*num2 == 0)
                    *num2 = maxNum2;
                else
                    (*num2) -= 1;                
            }
            blinkNum(row, col2, *num2);
            if(isButtonPressKeep(BTN_ENTER) == BTN_PRESS_FAST)
            {
                LcdPrintNumS(row, col2,(*num2)/10);
                LcdPrintNumS(row, col2+1,(*num2)%10);
                statusSetNum = SET_NUM_1;
                statusSCH = DISPLAY ;
            }
            break;
        break;
    }
}

void SCH_Update()
{
    char i  = 0;
    char tmp_hour = TIME_hour;
    char tmp_minute = TIME_minute;
    char tmp_index = indexSCH_select;
    char firstSCH = 1 ;
    if(flagSCH_Run == 0)
    {
        for(i = 0; i < SCH_count; i++)
        {
            // Neu hour vuot qua time hien tai
            if(SCH_Time[i].time_start_hour < TIME_hour )
                continue;
            if((SCH_Time[i].time_start_hour == TIME_hour)
                && (SCH_Time[i].time_start_minute < TIME_minute ))
                    continue ;
            // Gan gia tr? dau tien tim thay
            if(firstSCH == 1)
            { 
                firstSCH = 0;
                tmp_hour = SCH_Time[i].time_start_hour;
                tmp_minute = SCH_Time[i].time_start_minute;
            }
            if( SCH_Time[i].time_start_hour < tmp_hour )
            {
                tmp_hour = SCH_Time[i].time_start_hour;
                tmp_minute = SCH_Time[i].time_start_minute;
                indexSCH_select = i ;
            }
            else
            {
                 if( SCH_Time[i].time_start_minute < tmp_minute )
                 {
                    tmp_hour = SCH_Time[i].time_start_hour;
                    tmp_minute = SCH_Time[i].time_start_minute;
                    indexSCH_select = i ;
                 }
            }
        }
        if(indexSCH_select != tmp_index )
        {
            flagSCH_Wait = 1;
        }
    }
}

void SCH_Dispatch()
{
    // Co su kien sap toi
    if(flagSCH_Wait  == 1)
    {
        // Neu dung thoi gina
        if((SCH_Time[indexSCH_select].time_start_hour == TIME_hour)
                && (SCH_Time[indexSCH_select].time_start_minute == TIME_minute ))
        {
            // Luu gia tri cu
            scheDefault.time_green1 = timeOfGreenPhase1;
            scheDefault.time_yellow1 = timeOfYellowPhase1;
            scheDefault.time_green2 = timeOfGreenPhase2;
            scheDefault.time_yellow2 = timeOfYellowPhase2;         
            // Cap nhat gia tri moi
            timeOfGreenPhase1 = SCH_Time[indexSCH_select].time_green1;
            timeOfYellowPhase1 = SCH_Time[indexSCH_select].time_yellow1;
            timeOfGreenPhase2 = SCH_Time[indexSCH_select].time_green2;
            timeOfYellowPhase2 = SCH_Time[indexSCH_select].time_yellow2;
            
            flagSCH_Wait = 0 ;
            flagSCH_Run = 1;
        }
    }
    // Su kien sap ket thuc
    if( flagSCH_Run == 1)
    {
         // Neu dung thoi gina
        if((SCH_Time[indexSCH_select].time_finish_hour == TIME_hour)
                && (SCH_Time[indexSCH_select].time_finish_minute == TIME_minute ))
        {      
            // Cap nhat gia tri moi
            timeOfGreenPhase1 = scheDefault.time_green1;
            timeOfYellowPhase1 =scheDefault.time_yellow1;
            timeOfGreenPhase2 =scheDefault.time_green2;
            timeOfYellowPhase2 = scheDefault.time_yellow2;
            
            flagSCH_Run = 0;
        }
    }

}
void SCH_SendData()
{
    char i = 0;
     for(i = 0;  i < SCH_count; i++)
    {
        UartSendNum(i+1);
        UartSendString(". Schedule ");
        UartSendNum(i);
        UartSendString(" || ");
        UartSendNum(SCH_Time[i].time_start_hour);
        UartSendString(":");
        UartSendNum(SCH_Time[i].time_start_minute);
        UartSendString("->");
        UartSendNum(SCH_Time[i].time_finish_hour);
        UartSendString(":");
        UartSendNum(SCH_Time[i].time_finish_minute);
        UartSendString(" || Phase 1: Green = ");
        UartSendNum(SCH_Time[i].time_green1);
        UartSendString("  Yellow = ");
        UartSendNum(SCH_Time[i].time_yellow1);
        UartSendString(" || Phase 2: Green = ");
        UartSendNum(SCH_Time[i].time_green2);
        UartSendString("  Yellow = ");
        UartSendNum(SCH_Time[i].time_yellow2);
        UartSendString("\n\r");
    }      
}
void UpdateAdjSchToWeb(char mode, char index)
{
    //{"status":"sch","paras":2,"valueMode":"add","valueSch":[3,"12:30","14:30",40,3,25,3]}
    UartSendString("{\"status\":\"sch\",\"paras\":2,\"valueMode\":\"");
    if(mode == ADD)
        UartSendString("add");
    if(mode == EDIT)
        UartSendString("edit");
    if(mode == DELETE)
    {
        UartSendString("delete");
        UartSendString("\",\"valueSch\":[");
        UartSendNum(index);
    }
    else
    {
        UartSendString("\",\"valueSch\":[");
        UartSendNum(index);
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
    }
    UartSendString("]}");
    UartSendString("\n");
}
