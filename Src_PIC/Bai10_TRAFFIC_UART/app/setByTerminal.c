#include "setByTerminal.h"
#include "..\uart\uart.h"
#include "..\app\traffic_led.h"
#include "..\lcd\lcd.h"
#include "..\app\schedule.h"


#define INIT            0
#define WAIT_CMD        1
#define PARSE_DATA      2
#define TIME_LIGHT      3
#define SCHEDULE        4
#define UPDATE_DATA     5
#define SET_TIME        6
#define SET_SCHE        7

unsigned char statusTerminal = INIT;
char cmd[30] = "";
char cmd_length =  0;
unsigned char flagNewCmd = 0;
unsigned char modeCurrent = 0;

char cmd_TIME[15]="!TIME!";
char cmd_SCHE[15]="!SCHE!";
char cmd_SET_TIME[13]="!SET-TIME!";
char cmd_SET_SCHE[13]="!SET-SCHE!";
char cmd_SAVE[10]="!SAVE!";
char cmd_SCHE_NUM[10]="!SCHE-";
char cmd_SCHE_ADD[15]="!SCHE-ADD!";
char cmd_SCHE_DEL[15]="!SCHE-DEL-";
char cmd_SCHE_STA[6]="!STA-";
char cmd_SCHE_END[6]="!END-";
char cmd_SCHE_P1[6]="!P1-";
char cmd_SCHE_P2[6]="!P2-";

unsigned char timeLightTemp[4]={0,0,0,0};
schedule scheTemp = {0};
schedule scheNew = {0};
unsigned char scheNumSelect = 11 ;


void SCH_Send_Sche()
{
    UartSendString("Schedule ");
    UartSendNum(scheNumSelect);
    UartSendString(" || ");
    UartSendNum(scheTemp.time_start_hour);
    UartSendString(":");
    UartSendNum(scheTemp.time_start_minute);
    UartSendString("->");
    UartSendNum(scheTemp.time_finish_hour);
    UartSendString(":");
    UartSendNum(scheTemp.time_finish_minute);
    UartSendString(" || Phase 1: Green = ");
    UartSendNum(scheTemp.time_green1);
    UartSendString("  Yellow = ");
    UartSendNum(scheTemp.time_yellow1);
    UartSendString(" || Phase 2: Green = ");
    UartSendNum(scheTemp.time_green2);
    UartSendString("  Yellow = ");
    UartSendNum(scheTemp.time_yellow2);
    UartSendString("\n\r");
}

unsigned char str_cmp(char str1[], char str2[], char n)
{
    char i = 0;
    for(i = 0; i < n; i++)
    {
        if(str1[i] != str2[i])
            return 1 ;
    }
    return 0;
}

void setByTerminal()
{
    char i = 0;
    char t1 = 0;
    char t2 = 0;
    switch(statusTerminal)
    {
        case INIT:
            modeCurrent = INIT;
            UartSendString("Welcome! \n\r");
            UartSendString("1.Display time traffic light! (!TIME!)\n\r");
            UartSendString("2.Display schedule list! (!SCHE!) \n\r");
            UartSendString("3.Set light time (!SET-TIME!) \n\r");
            UartSendString("4.Set schedule (!SET-SCHE!) \n\r");
            statusTerminal = WAIT_CMD;
            break;
        case WAIT_CMD:
            flagNewCmd = UartGetString(cmd,&cmd_length);
            if(flagNewCmd == 1)
            {
                flagNewCmd = 0 ;
                statusTerminal = PARSE_DATA;
            }            
            break;
        case PARSE_DATA:
            if(str_cmp(cmd,cmd_TIME,6)== 0)
            {
                statusTerminal = TIME_LIGHT;
                break;
            }
            if(str_cmp(cmd,cmd_SCHE,6)== 0)
            {
                statusTerminal = SCHEDULE;
                break;
            }
            if(str_cmp(cmd,cmd_SET_TIME,10)== 0)
            {
                statusTerminal = SET_TIME;
                break;
            }
            if(str_cmp(cmd,cmd_SET_SCHE,10)== 0)
            {
                statusTerminal = SET_SCHE;
                break;
            }
            if(str_cmp(cmd,cmd_SAVE,6)== 0)
            {
                statusTerminal = UPDATE_DATA;
                break;
            }
            switch(modeCurrent)
            {
                case SET_TIME:
                    if(cmd[0] == '!' && cmd[6] == '!' && (cmd[1] == 'G' || cmd[1] == 'Y')&& (cmd[2] == '1' || cmd[2] == '2')  )
                    {
                        t1 = cmd[2]-'0';
                        t2 = (cmd[4]-'0')*10 + (cmd[5]-'0');
                        UartSendString("==========o0o=========== \n\r");
                        UartSendString("Ban vua thay doi ");
                        if( cmd[1] == 'G')
                        {
                            UartSendString("G");
                            timeLightTemp[(t1-1)*2] = t2;
                        }
                        if ( cmd[1] == 'Y')
                        {
                            UartSendString("Y");
                            timeLightTemp[(t1*2)-1] = t2;
                        }
                        UartSendNum(t1);
                        UartSendString(" = ");
                        UartSendNum(t2);
                        UartSendString("\n\r");
                        
                        UartSendString("Phase 1: Green = ");
                        UartSendNum(timeLightTemp[0]);
                        UartSendString("  Yellow = ");
                        UartSendNum(timeLightTemp[1]);
                        UartSendString("\n\r");
                        UartSendString("Phase 2: Green = ");
                        UartSendNum(timeLightTemp[2]);
                        UartSendString("  Yellow = ");
                        UartSendNum(timeLightTemp[3]);
                        UartSendString("\n\r");
                        
                    }
                    statusTerminal = WAIT_CMD;
                    break;
                case SET_SCHE:
                    if(str_cmp(cmd,cmd_SCHE_NUM,6)== 0 && (cmd_length == 8) && (cmd[cmd_length-1] == '!') )
                    {
                        scheNumSelect = cmd[6] - '0';
                        if(scheNumSelect >= SCH_count)
                        {
                            scheNumSelect = 11;
                            break;
                        }
                        UartSendString("==========o0o=========== \n\r");
                        UartSendString("Ban da chon [ schedule ");
                        UartSendNum( scheNumSelect );
                        UartSendString(" ]\n\r");
                        scheTemp = SCH_Time[scheNumSelect];
                        SCH_Send_Sche();
                    }
                    if(str_cmp(cmd,cmd_SCHE_ADD,10)== 0 )
                    {
                        UartSendString("==========o0o=========== \n\r");
                        UartSendString("Ban da THEM thanh cong [ schedule ");
                        UartSendNum(SCH_count);
                        UartSendString(" ]\n\r");
                        SCH_AddTime(scheNew);
                        SCH_SendData();
                    }
                    if(str_cmp(cmd,cmd_SCHE_DEL,10)== 0 && (cmd[cmd_length-1] == '!') )
                    {
                        scheNumSelect = cmd[10] - '0';
                        if(scheNumSelect >= SCH_count )
                        {
                            scheNumSelect = 11;
                            UartSendString("==========o0o=========== \n\r");
                            UartSendString("Hay chon schedule muon chinh sua (!SCHE-#!) \n\r");
                            break;
                        }
                        UartSendString("==========o0o=========== \n\r");
                        UartSendString("Ban da XOA thanh cong [ schedule ");
                        UartSendNum(scheNumSelect);
                        UartSendString(" ]\n\r");
                        SCH_Delete(scheNumSelect);
                        SCH_SendData();
                    }
                    if((str_cmp(cmd,cmd_SCHE_STA,5)== 0) && (cmd_length == 11) && (cmd[cmd_length-1] == '!')  )
                    {
                        if(scheNumSelect >= SCH_count )
                        {
                            scheNumSelect = 11;
                            UartSendString("==========o0o=========== \n\r");
                            UartSendString("Hay chon schedule muon chinh sua (!SCHE-#!) \n\r");
                            break;
                        }
                        UartSendString("==========o0o=========== \n\r");
                        UartSendString("Ban da THAY DOI time start = ");
                        scheTemp.time_start_hour = (cmd[5]-'0')*10 + (cmd[6]-'0');
                        scheTemp.time_start_minute = (cmd[8]-'0')*10 + (cmd[9]-'0');
                        UartSendNum( scheTemp.time_start_hour);
                        UartSendString(":");
                        UartSendNum( scheTemp.time_start_minute);
                        UartSendString(" \n\r");
                        SCH_Send_Sche();
                    }
                    if((str_cmp(cmd,cmd_SCHE_END,5)== 0) && (cmd_length == 11) && (cmd[cmd_length-1] == '!')  )
                    {
                        if(scheNumSelect >= SCH_count )
                        {
                            scheNumSelect = 11;
                            UartSendString("==========o0o=========== \n\r");
                            UartSendString("Hay chon schedule muon chinh sua (!SCHE-#!) \n\r");
                            break;
                        }
                        UartSendString("==========o0o=========== \n\r");
                        UartSendString("Ban da THAY DOI time end = ");
                        scheTemp.time_finish_hour = (cmd[5]-'0')*10 + (cmd[6]-'0');
                        scheTemp.time_finish_minute = (cmd[8]-'0')*10 + (cmd[9]-'0');
                        UartSendNum( scheTemp.time_finish_hour);
                        UartSendString(":");
                        UartSendNum( scheTemp.time_finish_minute);
                        UartSendString(" \n\r");
                        SCH_Send_Sche();
                    }
                     if((str_cmp(cmd,cmd_SCHE_P1,4)== 0) && (cmd_length == 10) && (cmd[cmd_length-1] == '!')  )
                    {
                         if(scheNumSelect >= SCH_count )
                        {
                            scheNumSelect = 11;
                            UartSendString("==========o0o=========== \n\r");
                            UartSendString("Hay chon schedule muon chinh sua (!SCHE-#!) \n\r");
                            break;
                        }
                        UartSendString("==========o0o=========== \n\r");
                        UartSendString("Ban da THAY DOI time phase 1 : ");
                        scheTemp.time_green1 = (cmd[4]-'0')*10 + (cmd[5]-'0');
                        scheTemp.time_yellow1 = (cmd[7]-'0')*10 + (cmd[8]-'0');
                        UartSendString("Green = ");
                        UartSendNum( scheTemp.time_green1);
                        UartSendString(" Yellow = ");
                        UartSendNum( scheTemp.time_yellow1);
                        UartSendString(" \n\r");
                        SCH_Send_Sche();
                    }
                      if((str_cmp(cmd,cmd_SCHE_P2,4)== 0) && (cmd_length == 10) && (cmd[cmd_length-1] == '!')  )
                    {
                          if(scheNumSelect >= SCH_count )
                        {
                            scheNumSelect = 11;
                            UartSendString("==========o0o=========== \n\r");
                            UartSendString("Hay chon schedule muon chinh sua (!SCHE-#!) \n\r");
                            break;
                        }
                        UartSendString("==========o0o=========== \n\r");
                        UartSendString("Ban da THAY DOI time phase 2 : ");
                        scheTemp.time_green2 = (cmd[4]-'0')*10 + (cmd[5]-'0');
                        scheTemp.time_yellow2 = (cmd[7]-'0')*10 + (cmd[8]-'0');
                        UartSendString("Green = ");
                        UartSendNum( scheTemp.time_green2);
                        UartSendString(" Yellow = ");
                        UartSendNum( scheTemp.time_yellow2);
                        UartSendString(" \n\r");
                        SCH_Send_Sche();
                    }                    
                    statusTerminal = WAIT_CMD;
                    break;
                default:
                    statusTerminal = WAIT_CMD;
                    break;                    
            }
            statusTerminal = WAIT_CMD;
            break;
        case UPDATE_DATA:
            UartSendString("==========o0o=========== \n\r");
            UartSendString("Cap nhat thanh cong !\r\n");
             switch(modeCurrent)
            {
                case SET_TIME:                    
                    timeOfGreenPhase1= timeLightTemp[0];
                    timeOfYellowPhase1= timeLightTemp[1];
                    timeOfGreenPhase2= timeLightTemp[2];
                    timeOfYellowPhase2= timeLightTemp[3];
                    break;
                case SET_SCHE:
                    SCH_Time[scheNumSelect] = scheTemp;
                    scheNumSelect = 11;
                    break;
             }
            modeCurrent = INIT;
            statusTerminal = WAIT_CMD;
        break;
        case TIME_LIGHT:
            modeCurrent = TIME_LIGHT;
            UartSendString("==========o0o=========== \n\r");
            UartSendString("Thoi gian hien tai cua cac den: \n\r");
            UartSendString("Phase 1: Green = ");
            UartSendNum(timeOfGreenPhase1);
            UartSendString("  Yellow = ");
            UartSendNum(timeOfYellowPhase1);
            UartSendString("\n\r");
            UartSendString("Phase 2: Green = ");
            UartSendNum(timeOfGreenPhase2);
            UartSendString("  Yellow = ");
            UartSendNum(timeOfYellowPhase2);
            UartSendString("\n\r");
                      
            
            statusTerminal = WAIT_CMD;
            break;
        case SCHEDULE:
            modeCurrent = SCHEDULE;
            UartSendString("==========o0o=========== \n\r");
            UartSendString("Danh sach cac thoi gian bieu hien co \n\r");
            SCH_SendData();
            statusTerminal = WAIT_CMD;
            break;
        case SET_TIME:
            modeCurrent = SET_TIME;
            UartSendString("==========o0o=========== \n\r");
            UartSendString("Thiet lap  thoi gian den giao thong \n\r");
            UartSendString("Cu phap: !AB-CD!. Trong do: AB la thong so, CD la gia tri \n\r");  
            UartSendString("G1,Y1,G2,Y2 la thong so thoi gian den giao thong \n\r");
            UartSendString("Vd: !G1-59! \n\r");
            
            timeLightTemp[0]= timeOfGreenPhase1;
            timeLightTemp[1]= timeOfYellowPhase1;
            timeLightTemp[2]= timeOfGreenPhase2;
            timeLightTemp[3]= timeOfYellowPhase2;
            
            statusTerminal = WAIT_CMD;
            break;
        case SET_SCHE:
            modeCurrent = SET_SCHE;
            UartSendString("==========o0o=========== \n\r");
            UartSendString("Thiet lap  thoi gian bieu \n\r");
            UartSendString("+ STA: gio, phut bat dau || END: gio, phut ket  thuc  (!STA-12:30!)\n\r");
            UartSendString("+ P1-G1-Y1 la thong so thoi gian den giao thong (!P1-25-03!)\n\r");
            UartSendString("- Them schedule moi (!SCHE-ADD!) \n\r");
            UartSendString("- Xoa schedule i (!SCHE-DEL-#!) \n\r");
            UartSendString("- Chon schedule muon chinh sua (!SCHE-#!) \n\r");
            statusTerminal = WAIT_CMD;
            break;
    }
}





