/*Error_list.c*/
#include "lpc_types.h"
#include "lpc17xx_rtc.h"
#include "main_routine.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "LPC17xx.h"
#include "OLED_2_7_12864.h"
#include "keys.h"
#include "parameters.h"
#include "plc.h"
#include "functions.h"
#include "Errors.h"

extern char Start_Stop_status;
extern char Error_No, Status_No;
extern unsigned long Count_Limit1,
    Count_Limit2,
    Count_Max,
    Count_Min,
    Scan_Count_Load_Monitor,
    Scan_Count_Limit1,
    Scan_Count_PF_Monitor,
    Scan_Count_Limit2,
    Scan_Count_MinLimit,
    Scan_Count_MaxLimit,
    scan_count_belt_track,
    scan_count_belt_track_monitor,
    scan_count_deviation_monitor,
    scan_count_deviation,
    scan_count_Tacho_monitor,
    scan_count_Tacho,
    scan_count_PF,
    scan_count_PF_Monitor,
    Scan_Count_BIN_PF_Monitor,
    Scan_Count_BIN_PF_Monitor_Reg,
    Scan_Count_Min_Hop,
    Scan_Count_Max_Hop,
    Scan_Count_Nom_Hop,
    Count_Min_Hop,
    Count_Max_Hop,
    Count_Nom_Hop;

extern unsigned long Count_BeltLoad, Count_Hop_Load, Scan_Count_Min_FD;
extern char ram_values[RAM_SIZE];
extern unsigned int Deviation_Value;
extern uint16_t New_menu_parameters[105];
extern unsigned int Belt_Speed;
extern unsigned int Tacho_Counter;
extern uint16_t scan_count;
extern char *P_String;
extern char A_String[200];
extern unsigned long Present_Count, Present_Count_ch2;
extern signed int Belt_Load;
extern unsigned char Mode_Status, ZERO_STATUS, Zero_Tacho_Feed_Buffer_Flag;
extern char New_Start, PF_Status, OF_Flag_ch1, OF_Flag_ch2;
extern unsigned char ACAL_Status;
extern char Check_Start, total_status;

static void ErrorLine_Refresh(void);

const char *ACAL_Display[4][16] = {
    {"ACAL In Progress"}, /* ACAL_Status = 2 */
    {"ACAL OK "},         /* ACAL_Status = 3 */
    {"ACAL > Limit"},     /* ACAL_Status = 4 */
    {"ACAL Abort"},       /* if M072 == 1 */
};

const char *error_msg[21][20] = {
    {" "},                /* Err_no = 0 */
    {"DIO Com Error"},    /* Err_no = 1 */
    {"Modbus Com Error"}, /* Err_no = 2 */
    {"Tacho Fault"},      /* Err_no = 3 */
    {"Belt Track Fault"}, /* Err_no = 4 */
    {"Overload Alarm"},   /* Err_no = 5 */
    {"Underload Alarm"},  /* Err_no = 6 */
    {"Loadcell 1 Fault"}, /* Err_no = 7 */
    {"Drive Fault(M019)"}, /* Err_no = 8 — panel/PLC M019/M020 DIO */
    {"Emergency Fault"},  /* Err_no = 9 */
    {"Deviation Fault"},  /* Err_no = 10 */
    {"No PLC File"},      /* Err_no = 11 */
    {"Interlock Fault"},  /* Err_no = 12 */
    {"ZSS ERROR"},        /* Err_no = 13 */
    {"Inv1 Com Error"},   /* Err_no = 14 */
    {"Inv2 Com Error"},   /* Err_no = 15 */
    {"Loadcell 2 Fault"}, /* Err_no = 16 */
    {"AO Comm Fail"},     /* Err_no = 17 */
    {"Drive Fault(Sts Wd)"}, /* Err_no = 18 — VFD Modbus status word */
};

#define ERROR_MIN_CODE 1
#define ERROR_MAX_CODE 18
#define ERROR_ROTATE_SCANS 400U /* 2s at 200 scans/sec */

static unsigned char error_latch[21];
static unsigned char error_display_index = 0;
static uint16_t error_rotate_elapsed_scans = 0;
static uint16_t error_rotate_last_scan = 0;

static void ErrorLatch_Add(unsigned char err)
{
    if ((err >= ERROR_MIN_CODE) && (err <= ERROR_MAX_CODE))
        error_latch[err] = 1;
}

void ErrorLatch_SetFault(unsigned char err)
{
    ErrorLatch_Add(err);
}

static unsigned char ErrorLatch_FindNext(unsigned char start_idx)
{
    unsigned char step;
    unsigned char idx = start_idx;

    if ((idx < ERROR_MIN_CODE) || (idx > ERROR_MAX_CODE))
        idx = ERROR_MIN_CODE;
    for (step = 0; step < ERROR_MAX_CODE; step++)
    {
        if ((idx > ERROR_MAX_CODE) || (idx < ERROR_MIN_CODE))
            idx = ERROR_MIN_CODE;
        if (error_latch[idx] == 1)
            return idx;
        idx++;
    }
    return 0;
}

static void ErrorLatch_Collect(void)
{
    /* Latch from fault sources only; Error_No is the rotating display output. */
    if (Modbus_Comm_Fault == 1)
        ErrorLatch_Add(2);
    else
        error_latch[2] = 0;
    if (M040 == 1)
        ErrorLatch_Add(3);
    if (M041 == 1)
        ErrorLatch_Add(4);
    if (M058 == 1)
        ErrorLatch_Add(5);
    else
        error_latch[5] = 0;
    if (M059 == 1)
        ErrorLatch_Add(6);
    else
        error_latch[6] = 0;
    if (M060 == 1)
        ErrorLatch_Add(7);
    if (M019 == 0)
        ErrorLatch_Add(ERR_DRIVE_M019);
    if (M045 == 1)
        ErrorLatch_Add(ERR_DRIVE_M019);
    if (INV_Error_Flag == 1)
        ErrorLatch_Add(ERR_DRIVE_STSWD);
    if (M010 == 0)
        ErrorLatch_Add(9);
    else if (M044 == 0)
        error_latch[9] = 0; /* sticky until M018 clears M044 (emg ack) */
    if (M043 == 1)
        ErrorLatch_Add(10);
    if ((M017 == 0) && (!GetFieldMode()))
        ErrorLatch_Add(12);
    else if (M044 == 0)
        error_latch[12] = 0; /* sticky until M018 clears M044 */
    if (M046 == 1)
        ErrorLatch_Add(13);
    /* Err14/15: Inv comm timeout/CRC only — not INV_Error_Flag (status word uses ERR_DRIVE_STSWD) */
    if (INV2_Error_Flag == 1)
        ErrorLatch_Add(15);
    if (M062 == 1)
        ErrorLatch_Add(16);
    if (AO_Comm_Fault == 1)
        ErrorLatch_Add(17);
    else
        error_latch[17] = 0;
}

static void ErrorLatch_UpdateDisplay(void)
{
    unsigned char any_latched = 0;
    unsigned char i;
    uint16_t now = scan_count;
    uint16_t delta;

    if ((error_rotate_last_scan > 199U) || (now > 199U))
        delta = 0;
    else if (now >= error_rotate_last_scan)
        delta = now - error_rotate_last_scan;
    else
        delta = (uint16_t)(200U - error_rotate_last_scan + now);
    error_rotate_last_scan = now;

    for (i = ERROR_MIN_CODE; i <= ERROR_MAX_CODE; i++)
    {
        if (error_latch[i] == 1)
        {
            any_latched = 1;
            break;
        }
    }

    if (any_latched == 0)
    {
        Error_No = 0;
        error_display_index = 0;
        error_rotate_elapsed_scans = 0;
        return;
    }

    if ((error_display_index == 0) || (error_latch[error_display_index] == 0))
    {
        unsigned char first = ErrorLatch_FindNext(error_display_index);
        if (first == 0)
        {
            Error_No = 0;
            error_display_index = 0;
            return;
        }
        error_display_index = first;
    }

    error_rotate_elapsed_scans = (uint16_t)(error_rotate_elapsed_scans + delta);
    while (error_rotate_elapsed_scans >= ERROR_ROTATE_SCANS)
    {
        unsigned char next_start = (unsigned char)(error_display_index + 1);
        unsigned char next;
        if (next_start > ERROR_MAX_CODE)
            next_start = ERROR_MIN_CODE;
        next = ErrorLatch_FindNext(next_start);
        if (next == 0)
            break;
        error_display_index = next;
        error_rotate_elapsed_scans = (uint16_t)(error_rotate_elapsed_scans - ERROR_ROTATE_SCANS);
    }
    Error_No = (char)error_display_index;
}

/* Refresh line 6 when rotation index changes (online display, ACAL rules match Online.c). */
static void ErrorLine_Refresh(void)
{
    if ((ZERO_STATUS != 0) || (Check_Start == 20) || (total_status >= 1))
        return;

    if ((ACAL_Status > 1) && (ACAL_Status < 5) && (Pre_Hopper == 1))
        return;

    if (ACAL_Status == 0)
    {
        if ((M077 == 1) && (Error_No == 0) && (Pre_Hopper == 1))
            Print_String2(ACAL_Display[3], line6, 0, font10, 0);
        else
            Print_String2(error_msg[(unsigned char)Error_No], line6, 0, font10, 0);
    }
}

void ErrorLatch_ClearAll(void)
{
    unsigned char i;

    for (i = 0; i <= ERROR_MAX_CODE; i++)
        error_latch[i] = 0;
    error_display_index = 0;
    error_rotate_elapsed_scans = 0;
    error_rotate_last_scan = scan_count;
    Modbus_Comm_Fault = 0;
    AO_Comm_Fault = 0;
    Error_No = 0;
}

/* msk29-may-26: clear latched Inv1/Inv2 com errors on comm reset */
void ErrorLatch_ClearInvComm(void)
{
    error_latch[14] = 0;
    error_latch[15] = 0;
    error_latch[ERR_DRIVE_STSWD] = 0;
    if ((error_display_index == 14) || (error_display_index == 15)
        || (error_display_index == ERR_DRIVE_STSWD))
    {
        unsigned char next = ErrorLatch_FindNext(ERROR_MIN_CODE);
        if (next == 0)
        {
            Error_No = 0;
            error_display_index = 0;
        }
        else
        {
            error_display_index = next;
            Error_No = (char)next;
        }
    }
}

uint16_t ErrorLatch_GetBitmapLow(void)
{
    uint16_t bitmap = 0;
    unsigned char err;

    for (err = 1; err <= 16; err++)
    {
        if (error_latch[err] == 1)
            bitmap = (uint16_t)(bitmap | (uint16_t)(1U << (err - 1U)));
    }
    return bitmap;
}

uint16_t ErrorLatch_GetBitmapHigh(void)
{
    uint16_t bitmap = 0;

    if (error_latch[17] == 1)
        bitmap = 0x0001;
    if (error_latch[ERR_DRIVE_STSWD] == 1)
        bitmap |= 0x0002;
    return bitmap;
}

void Display_Error(void)
{
    if (OF_Flag_ch1 == 1)
    {
        Error_No = 7; /* Loadcell 1 Fault */
        if (!GetFieldMode())
        {
            Start_Stop_status = STOP;
            M066 = 0;
            Zero_Tacho_Feed_Buffer_Flag = 2;
        }
        M060 = 1; /* Loadcell ch1 error marker */
        M044 = 1; /* General Fault Marker */
        if (((GetDeIntVolMode()) || (GetFieldMode())) && (ZERO_STATUS == 2))
            ZERO_STATUS = 5;
    }
    if ((Error_No == 7) && (OF_Flag_ch1 != 1))
    {
        Error_No = 0;
        M060 = 0;
    }

    if (Pre_Hopper == 1)
    {
        if (OF_Flag_ch2 == 1)
        {
            Error_No = 16; /* Loadcell 2 Fault */
            if (!GetFieldMode())
            {
                Start_Stop_status = STOP;
                M066 = 0;
                Zero_Tacho_Feed_Buffer_Flag = 2;
            }
            M062 = 1; /* Loadcell ch2 error marker */
            M044 = 1; /* General Fault Marker */
        }
        if ((Error_No == 16) && (OF_Flag_ch2 != 1))
        {
            Error_No = 0;
            M062 = 0;
        }
    }

    Count_BeltLoad = Present_Count;
    if ((Count_BeltLoad > Count_Limit1) && (!GetFieldMode()))
    {
        --Scan_Count_Limit1;
        if (Scan_Count_Limit1 == 0)
        {
            M056 = 1;
            Scan_Count_Limit1 = Scan_Count_Load_Monitor;
        }
    }
    else
    {
        Scan_Count_Limit1 = Scan_Count_Load_Monitor;
        M056 = 0;
    }

    if ((Count_BeltLoad > Count_Limit2) && (!GetFieldMode()))
    {
        --Scan_Count_Limit2;
        if (Scan_Count_Limit2 == 0)
        {
            M057 = 1;
            Scan_Count_Limit2 = Scan_Count_Load_Monitor;
        }
    }
    else
    {
        Scan_Count_Limit2 = Scan_Count_Load_Monitor;
        M057 = 0;
    }

    if ((Count_BeltLoad > Count_Max) && (!GetFieldMode()))
    {
        --Scan_Count_MaxLimit;
        if (Scan_Count_MaxLimit == 0)
        {
            M058 = 1;
            Error_No = 5; /* Overload Alarm */
            Scan_Count_MaxLimit = Scan_Count_Load_Monitor;
        }
    }
    else
    {
        Scan_Count_MaxLimit = Scan_Count_Load_Monitor;
        M058 = 0;
    }

    if ((Count_BeltLoad < Count_Min) && (!GetFieldMode()))
    {
        --Scan_Count_MinLimit;
        if (Scan_Count_MinLimit == 0)
        {
            Scan_Count_Min_FD = 0;
            M059 = 1;
            Error_No = 6; /* Underload Alarm */
            Scan_Count_MinLimit = Scan_Count_Load_Monitor;
        }
    }
    else
    {
        Scan_Count_MinLimit = Scan_Count_Load_Monitor;
        M059 = 0;
    }

    /* msk changed belt track check before STOP return 17-may-2026 */
    if (M009 == 1)
        scan_count_belt_track = scan_count_belt_track_monitor;

    if (M009 == 0)
    {
        --scan_count_belt_track;
        if (scan_count_belt_track == 0)
        {
            Error_No = 4; /* Belt track fault Alarm */
            M041 = 1;
            M044 = 1;
            scan_count_belt_track = scan_count_belt_track_monitor;
            if (!GetFieldMode())
            {
                Start_Stop_status = STOP;
                M066 = 0;
                Zero_Tacho_Feed_Buffer_Flag = 2;
            }
        }
    }
    if (M041 == 1)
        Error_No = 4;

    if (Start_Stop_status == STOP)
    {
        scan_count_deviation = scan_count_deviation_monitor;
        scan_count_Tacho = scan_count_Tacho_monitor;
        ErrorLatch_Collect();
        ErrorLatch_UpdateDisplay();
        ErrorLine_Refresh();
        return;
    }

    if ((Dev_Limit < Deviation_Value) && (!GetFieldMode()))
    {
        --scan_count_deviation;
        if (scan_count_deviation == 0)
        {
            Error_No = 10; /* Deviation fault Alarm */
            M044 = 1;
            M043 = 1;
            scan_count_deviation = scan_count_deviation_monitor;
            if (!GetFieldMode())
            {
                Start_Stop_status = STOP;
                M066 = 0;
                Zero_Tacho_Feed_Buffer_Flag = 2;
            }
        }
    }

    if (Fixed_Frequency == 1)
        Belt_Speed = Tacho_Freq;

    if ((Belt_Speed == 0) && (!GetFieldMode()))
    {
        --scan_count_Tacho;
        if (scan_count_Tacho == 0)
        {
            Error_No = 3; /* Tacho Fault Alarm */
            M040 = 1;
            //M042 = 1; //msk commented 21-jun-26
            M044 = 1;
            scan_count_Tacho = scan_count_Tacho_monitor;
            if (!GetFieldMode())
            {
                if ((GetDeIntVolMode()) && (ZERO_STATUS == 2))
                    ZERO_STATUS = 5;
                Start_Stop_status = STOP;
                M066 = 0;
                Zero_Tacho_Feed_Buffer_Flag = 2;
            }
        }
    }

    if (PF_Status == 1)
    {
        --scan_count_PF;
        if ((scan_count_PF == 0) || (Belt_Load > BL_Limit))
        {
            PF_Status = 0;
            scan_count_PF = scan_count_PF_Monitor;
        }
    }

    if (Pre_Hopper == 1)
    {
        Count_Hop_Load = Present_Count_ch2;
        if ((Count_Hop_Load > Count_Max_Hop) && (!GetFieldMode()))
        {
            --Scan_Count_MaxLimit;
            if (Scan_Count_MaxLimit == 0)
            {
                M082 = 1;
                Scan_Count_MaxLimit = Scan_Count_BIN_PF_Monitor_Reg;
            }
        }
        else
        {
            Scan_Count_MaxLimit = Scan_Count_BIN_PF_Monitor_Reg;
            M082 = 0;
        }

        if ((Count_Hop_Load < Count_Min_Hop) && (!GetFieldMode()))
        {
            --Scan_Count_MinLimit;
            if (Scan_Count_MinLimit == 0)
            {
                M080 = 1;
                Scan_Count_MinLimit = Scan_Count_BIN_PF_Monitor_Reg;
            }
        }
        else
        {
            Scan_Count_MinLimit = Scan_Count_BIN_PF_Monitor_Reg;
            M080 = 0;
        }

        if ((Count_Hop_Load < Count_Nom_Hop) && (!GetFieldMode()))
        {
            --Scan_Count_Nom_Hop;
            if (Scan_Count_Nom_Hop == 0)
            {
                M081 = 1;
                Scan_Count_Nom_Hop = Scan_Count_BIN_PF_Monitor_Reg;
            }
        }
        else
        {
            Scan_Count_Nom_Hop = Scan_Count_BIN_PF_Monitor_Reg;
            M081 = 0;
        }
    }
    ErrorLatch_Collect();
    ErrorLatch_UpdateDisplay();
    ErrorLine_Refresh();
}

/*
30/03/2018
    if(( Mode_Status == Manual )&&(ZERO_STATUS == 2) ) //Zeroing is in progress
        ZERO_STATUS = 5;
14/07/2018
M044 = 1;     //Added general fault marker if loadcell error
*/
