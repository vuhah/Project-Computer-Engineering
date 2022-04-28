#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include <p18f4620.h>

#define     SCH_MAX_NUM     10

typedef struct schedule {
    unsigned int time_start_hour ;
    unsigned int time_start_minute ;
    unsigned int time_finish_hour ;
    unsigned int time_finish_minute ;
    unsigned int time_yellow1 ;
    unsigned int time_green1 ;
    unsigned int time_yellow2 ;
    unsigned int time_green2 ;
    
} schedule ;

void SCH_AddTime(schedule time);
void SCH_EditTime(schedule time, char index);
void ScheduleTime(char* isBack);
void SCH_SetTime();
void setTime(char row, char col1, char col2,unsigned int* num1,unsigned int* num2, char maxNum1, char maxNum2);
void SCH_Update();
void SCH_SendData();
void SCH_Delete(char index);

extern unsigned char SCH_count ;
extern schedule SCH_Time[SCH_MAX_NUM];

#endif

