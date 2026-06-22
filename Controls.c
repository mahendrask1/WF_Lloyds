#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "plc.h"
//#include "ram_values.h"
#include "keys.h"
#undef Start_Stop /* key code 18; must not prefix-expand onto Start_Stop_status */
#include "menu.h"
#include "parameters.h"
#include "Errors.h"
#include "functions.h"
void After_AutoCAL(void);
extern uint8_t ram_values[RAM_SIZE];
extern uint16_t New_menu_parameters[105];
extern unsigned int                     External_Setpoint,
										Common_Setpoint,
										Current_Setpoint,
										Deviation,
										PF_Drive_Setpoint,
										WF_Drive_Setpoint_per,
										Belt_Speed,
										PF_Drive_Setpoint_Per,
										Hopper_val,
										Gate_Postion,
										HOpper_total,
										FDR_Total,
										CM_Difference;

extern double Totaliser1,Totaliser2, Totaliser1_Update;
extern unsigned int Totaliser1_Display,Totaliser2_Display;
extern signed int Belt_Load;
extern float WF_Drive_Setpoint,CM_Display;
extern unsigned char Mode_Status, ACAL_Status, BIN_PF_Start_Status;
extern unsigned char INV2_Error_Flag;
/* Error_No, Start_Stop_status: declared in keys.h */
bool CurTripStatus, CurErrorStatus;

/* WF drive healthy (M019/DIO) or VFD Modbus/status fault (field/local operation). */
static bool WfDriveOrVfdFaultActive(void)
{
    uint16_t wf_inv_type;

    if (M019 == 0)
        return true;
    wf_inv_type = Inv;
    if ((wf_inv_type != 0) && (INV_Error_Flag == 1))
        return true;
    /* ATV31 Altivar: status word bit 3 (fault) -> I027 */
    if ((wf_inv_type == 1) && (I027 == 1))
        return true;
    return false;
}

char DriveFaultErrorCode(void)
{
    if ((Inv != 0) && (INV_Error_Flag == 1))
        return ERR_DRIVE_STSWD;
    /* ATV31: status fault bit mapped via VFD DI read */
    if ((Inv == 1) && (I027 == 1))
        return ERR_DRIVE_STSWD;
    return ERR_DRIVE_M019;
}

static void ApplyWfDriveFaultTrip(void)
{
    M066 = 0;
    Start_Stop_status = STOP;
    M044 = 1;
    M042 = 1;
    Zero_Tacho_Feed_Buffer_Flag = 2;
    if (INV2_Error_Flag == 1)
        Error_No = 15;
    else
        Error_No = DriveFaultErrorCode();
}
/*Controls.c
M000: Start remote (modbus command)*(verified correct)
M001: Stop remote (modbus command)*(verified correct)
M002: Remote (modbus command) (1=remote on,0=remote off)*(verified correct)
M007: Gravimetric Mode (1-Gravimetric, 0-Volumetric)*(verified correct)
M008: Local remote selection (1-Rmote, 0-local) (Verified reverse as per manual)
M015: Int/De-Interlock Mode Select (1-De-Interlock, 0-Int)*(verified correct)
M010: Emergency Fault (1-Healthy,0-Error)*(verified correct)
M011: Local start WF(1-start)*(verified correct)
M012: Local stop WF(1-stop)*(verified correct)
M016: (PLC Command) Remote Start/Stop   (1=Start,0=Stop)
M017: Downstrem Interlock (1=healthy, 0=fault)*(verified reverse as per manual)
M018: Error Reset (1=reset,0=no reset)*(verified correct)
M019: Drive Healthy-WF(1=healthy,0=fault)*(verified correct)

-------------O/P Markers---------------
M051: lOCAL MODE STATUS (1=Local,0=Remote)*(verified correc)
M052: Gravimetric Mode Status (1=Gravimetric,0=Volumetric)*(verified correct)
M053: Interlock Status (1=Interlocked,0=De-Interlocked)*(verified correct)
M064: WF-Start/Stop/Fault (1=start,0=stop,Blink=fault)*(verified correct)
M066: WF-Start/Stop (1=start,0=stop)*(verified correct)
M068: System Healthy (1=Healthy,0=Fault) not used anywhere in sw

// mode selection macros
#define Int_Gra_Dio 10
#define DeInt_Gra_Dio 11
#define Int_Vol_Dio 12
#define DeInt_Vol_Dio 13
#define Int_Gra_Com 14
#define Int_Vol_Com 15
#define FieldMode 16

*/
//extern unsigned char Operating_Mode;
/*
m007 -> M052  gr/vol mode 
m015 -> M053  int/de-int mode, m53= int/de-int mode, 1=de-int, 0=int
m008 -> M051  local/remote mode, m066= wf-start/stop/fault, 1=start
*/
//------------------------------------------------------------------
unsigned char External_Operating_Modes(unsigned char MyMode)
{
    if(Mode_Select == Ext)
    {
        if(M015==0)
        {
            M053 = 1; // M053 set for interlock mode
        }
        else if(M015==1)
        {
            M053 = 0; // M053 set for de-interlock mode
        }
    }
    if((M002==1) && (M053 == 1))//Interlocked(M053=1) and  SIO(M002=1)
    {
        if(M007==1)//Gravimetric Mode (1-Gravimetric, 0-Volumetric)
        {
            MyMode=Int_Gra_Com;   //Remote (modbus command) Interlock Gravimetric Mode
        }
        else
        {
            MyMode=Int_Vol_Com;   //Remote (modbus command) Interlock Volumetric Mode
        }
    }
    else if((M002==0) && (M053==1)) //SIO(M002=0) and Interlocked(M053=1) is set
    {
        if(M007==1)//Gravimetric Mode (1-Gravimetric, 0-Volumetric)
        {
            MyMode=Int_Gra_Dio;   //Interlock Gravimetric Mode
        }
        else
        {
            MyMode=Int_Vol_Dio;   //Interlock Volumetric Mode
        }
    }
    else if((M002==0) && (M053==0)) //De-Interlocked(M053=0) is set
    {
        //De-Interlocked(M053=0) is set
        if(M007==1)//Gravimetric Mode (1-Gravimetric, 0-Volumetric)
        {
            MyMode=DeInt_Gra_Dio;   //De-Interlock Gravimetric Mode
        }
        else 
        {
            MyMode=DeInt_Vol_Dio;   //De-Interlock Volumetric Mode
        }
    }
    else if((M002==1) && (M053==0)) // De-interlock + Modbus start/stop (M000/M001)
    {
        if(M007==1)
        {
            MyMode=DeInt_Gra_Dio;
        }
        else
        {
            MyMode=DeInt_Vol_Dio;
        }
    }
    return MyMode;  
}
//---------------------------------------------------------------------
unsigned char GetOperatingMode(unsigned char MyMode){
    //unsigned char MyMode=Operating_Mode;
    if(M008==1)//Reemote Mode (1-Rmote(field operated), 0-Local(panel operated))
    {
        M051 = 0; // Output Marker M051;0-Remote ; 1-Local
        if (Mode_Select == Ext) // EXT = Mode selection from Digital Inputs or PLC/DCS
                                //INT = mode selection through Keyboard
        {
            MyMode = External_Operating_Modes(MyMode);
        } 
        else if((Mode_Select == Int)&&(MyMode==FieldMode))//msk 03-may-26
            {   
                //if controller is transferred from field mode to remote mode then
                //initialise the mode accordingly. this if statement will be executed only once
                //when transition occurs Fient -> Remote mode
               // M052 = 0; // 1= gravemetric, 0 =Volumetric Mode
               // M053 = 1;//(1=Interlocked,0=De-Interlocked)
                if((M052==0)&&(M053==0))
                {
                    MyMode = DeInt_Vol_Dio;
                }
                else if((M052==0)&&(M053==1))
                {
                    MyMode = Int_Vol_Dio;
                }
                else if((M052==1)&&(M053==0))   
                {
                    MyMode = DeInt_Gra_Dio; 
                }
                else if((M052==1)&&(M053==1))
                {

                    MyMode = Int_Gra_Dio;
                }           
            }
        /* M052 tracks M007 (Ext/PLC) or Operating_Mode (Int/panel).
         * Digital_Input_Action() used to set M052 from M007 every scan; restore that here. */
        if (Mode_Select == Ext)
            M052 = (M007 != 0) ? 1 : 0;
        else if ((MyMode == Int_Gra_Dio) || (MyMode == DeInt_Gra_Dio) || (MyMode == Int_Gra_Com))
            M052 = 1;
        else
            M052 = 0;
    }//End of Remote Mode
    else if(M008==0 )//Local remote selection(1-Rmote, 0-local) 
    {
        MyMode=FieldMode;   //Field Mode
        M051 = 1; // Output Marker M051;0-Remote ; 1-Local
        M052 = 0; // Field mode status: volumetric
        M053 = 0; // Field mode status: de-interlock
    }
    return MyMode;
}//End of OperatingModeAction()
//-------------------------------------------------------
bool GetDeIntGraMode()
{
    if(Operating_Mode==DeInt_Gra_Dio)
    {
        return true; //Gravimetric Mode
    }
    else
    {
        return false; //Gravimetric Mode
    }
}
//-------------------------------------------------------
bool GetIntGraMode()
{
    if(Operating_Mode==Int_Gra_Dio)
    {
        return true; //Gravimetric Mode
    }
    else
    {
        return false; //Gravimetric Mode
    }
}
//-------------------------------------------------------   
bool GetIntVolMode()
{
    if(Operating_Mode==Int_Vol_Dio)
    {
        return true; //Volumetric Mode
    }
    else
    {
        return false; //Volumetric Mode
    }
}
//-------------------------------------------------------
bool GetDeIntVolMode()
{
    if(Operating_Mode==DeInt_Vol_Dio)
    {
        return true; //Volumetric Mode
    }
    else
    {
        return false; //Volumetric Mode
    }
}
//-------------------------------------------------------
//-------------------------------------------------------
bool GetVolMode()
{
    if((Operating_Mode==Int_Vol_Dio) || (Operating_Mode==DeInt_Vol_Dio) || 
    (Operating_Mode==Int_Vol_Com))
    {
        return true; //Volumetric Mode
    }
    else
    {
        return false; //Volumetric Mode
    }
}
//-------------------------------------------------------
bool GetGraMode()
{
    if((Operating_Mode==Int_Gra_Dio) || (Operating_Mode==DeInt_Gra_Dio) || 
    (Operating_Mode==Int_Gra_Com))
    {
        return true; //Gravimetric Mode
    }
    else
    {
        return false; //Volumetric Mode
    }
}
//-------------------------------------------------------
bool GetIntMode( )
{
    if((Operating_Mode==Int_Gra_Dio) || (Operating_Mode==Int_Vol_Dio) || 
    (Operating_Mode==Int_Gra_Com) || (Operating_Mode==Int_Vol_Com))
    {
        return true; //Interlock Mode
    }
    else
    {
        return false; //De-Interlock Mode
    }
}
//-------------------------------------------------------
bool GetDeIntMode()
{
    if((Operating_Mode==DeInt_Gra_Dio) || (Operating_Mode==DeInt_Vol_Dio))
    {
        return true; //De-Interlock Mode
    }
    else
    {
        return false; //Interlock Mode
    }
}
//-------------------------------------------------------
bool GetComMode()
{
    if((Operating_Mode==Int_Gra_Com) || (Operating_Mode==Int_Vol_Com))
    {
        return true; //Interlocked Communication Mode
    }
    else
    {
        return false; //Not a Communication Mode
    }
}
//-------------------------------------------------------
bool GetFieldMode()
{
    if(Operating_Mode==FieldMode)
    {
        return true; //Local Mode (Field operated)
    }
    else
    {
        return false;//panel or remote operated mode
    }
}
//-----------------------------------------------------
/* Same marker/error clear as M018 fault ack (also used after boot VFD warmup). */
void ApplyFaultResetMarkers(void)
{
    Error_No = 0;
    M056 = 0;
    M057 = 0;
    M058 = 0;
    M059 = 0;
    M060 = 0;
    M040 = 0;
    M041 = 0;
    M042 = 0;
    M043 = 0;
    M044 = 0;
    M045 = 0;
    M009 = 1;
    M017 = 1;
    M019 = 1;
    M020 = 1;
    M077 = 0;
    M046 = 0;
    ErrorLatch_ClearAll();
    ResetInvCommState();
}

/* After warmup + VfdBootSendFaultResetBeforeSuperloop: clear WF fault sources. */
void VfdBootPostWarmupInit(void)
{
    if (Inv == 0)
        return;
    Error_No = 0;
    M056 = 0;
    M057 = 0;
    M058 = 0;
    M059 = 0;
    M060 = 0;
    M040 = 0;
    M041 = 0;
    M042 = 0;
    M043 = 0;
    M044 = 0;
    M045 = 0;
    M009 = 1;
    M017 = 1;
    M010 = 1;
    M019 = 1;
    M020 = 1;
    M077 = 0;
    M046 = 0;
    ErrorLatch_ClearAll();
    ClearInvCommFaultFlags();
    VfdDiSanitizeStartMarkers();
}

//-----------------------------------------------------
bool UpdateTripStatus()
{
    bool flag = false;
    if ((Start_Stop_status == START) && ((T000 == 1) || (T001 == 1)) 
    && (GetFieldMode() != true)) 
    {//Zero Speed Switch faulr Alarm
        Error_No = 13; // Zero Speed Switch faulr Alarm
        M066 = 0;
        M046 = 1; // ZSS faulr Marker
        M044 = 1; // General Fault Markler
        Start_Stop_status = STOP;
        Zero_Tacho_Feed_Buffer_Flag = 2;
        flag = true;
    }
    if ((Start_Stop_status == START) && (M017 == 0) && (GetFieldMode() != true)) // Down Stream Interlock
    //if ((M017 == 0) && (GetFieldMode() != true)) // Down Stream Interlock 
    {
        M066 = 0;
        Start_Stop_status = STOP;
        M044 = 1; // General Fault Marker
        Error_No = 12; // Down Stream Interlock
        Zero_Tacho_Feed_Buffer_Flag = 2;
        flag = true;
    }    
    if ((Start_Stop_status == START) && (M010 == 0)) //m10= emergency fault
    //if ((M010 == 0)) //m10= emergency fault
    {
        M066 = 0;
        Start_Stop_status = STOP;
        M044 = 1; // General Fault
        Error_No = 9; // Emergency faulr Alarm
        Zero_Tacho_Feed_Buffer_Flag = 2;
        flag = true;
    }
    if (((Start_Stop_status == START) || (GetFieldMode() && (M066 == 1)) || (M008 == 0)) 
        && WfDriveOrVfdFaultActive()) 
    { // DIO healthy, VFD Modbus fault, or ATV status fault
        M066 = 0; // STOP
        Start_Stop_status = STOP;
        M044 = 1; // Gneral Fault Marker
        M042 = 1; // Set WF Drive Fault Marker
        Zero_Tacho_Feed_Buffer_Flag = 2;

        Error_No = DriveFaultErrorCode();
        flag = true;
    }
    if (((Start_Stop_status == START) || (GetFieldMode() && (M066 == 1))) && (M020 == 0)) 
    { //m20= drive healthy-pf, 0=fault, PF Drive Fault
        M067 = 0; // Start/Stop  Marker PF
        M066 = 0; // Start/Stop  Marker WF
        Start_Stop_status = STOP;
        M044 = 1; // General Fault
        M045 = 1; // PF Drive Fault Marker
        Error_No = ERR_DRIVE_M019; /* PF drive healthy DIO */
        Zero_Tacho_Feed_Buffer_Flag = 2;
        flag = true;
    }
    if (M018 == 1) // Fault ack_i/p — only when emg input healthy
    {
        if (M010 == 1)
        {
            ApplyFaultResetMarkers();
            M018 = 0;
        }
        flag = false;
    }
    return flag;
}
//-------------------------------------------------------------------------------------
bool UpdateErrorStatus()
{       
    bool flag = false;
    //if (GetFieldMode() == true) // check only if local(field) mode
    //msk removed field mode condition check 10-05-2026
    //{
    //      return false; //No Error Status
    //}
    if ((Start_Stop_status == START) && ((T000 == 1) || (T001 == 1)) 
    && (GetFieldMode() != true)) 
    {//Zero Speed Switch faulr Alarm
        Error_No = 13; // Zero Speed Switch faulr Alarm
        M066 = 0;
        M046 = 1; // ZSS faulr Marker
        M044 = 1; // General Fault Markler
        Zero_Tacho_Feed_Buffer_Flag = 2;
        flag = true;
    }
 //   if ((Start_Stop_status == START) && (M017 == 0) && (GetFieldMode() != true)) // Down Stream Interlock
    if ((M017 == 0) && (GetFieldMode() != true)) // Down Stream Interlock
    //msk removed start condition condition check 10-05-2026
    {
        M066 = 0;
        M044 = 1; // General Fault Marker
        Error_No = 12; // Down Stream Interlock
        Zero_Tacho_Feed_Buffer_Flag = 2;
        flag = true;
    }
    //if ((Start_Stop_status == START) && (M010 == 0)) //m10= emergency fault
    if ((M010 == 0)) //m10= emergency fault
     //msk removed start condition condition check 10-05-2026
    {
        M066 = 0;
        M044 = 1; // General Fault
        Error_No = 9; // Emergency faulr Alarm
        Start_Stop_status = STOP;
        Zero_Tacho_Feed_Buffer_Flag = 2;
        flag = true;
    }

    if (WfDriveOrVfdFaultActive()) 
     //msk DIO + VFD Modbus/status fault (field/local) 17-may-2026
    {
        ApplyWfDriveFaultTrip();
        flag = true;
        flag = true;

    }

    if ((M020 == 0)) 
     //msk PF drive fault in field/stop like WF 17-may-2026
    { //m20= drive healthy-pf, 0=fault
        M067 = 0;
        M066 = 0;
        Start_Stop_status = STOP;
        M044 = 1;
        M045 = 1; // PF Drive Fault Marker
        Zero_Tacho_Feed_Buffer_Flag = 2;
        Error_No = ERR_DRIVE_M019;
        flag = true;
    }
    if (M042 == 1)    //msk keep WF drive error latched 17-may-2026
    {
        Error_No = (INV_Error_Flag == 1) ? ERR_DRIVE_STSWD : ERR_DRIVE_M019;
        M044 = 1;
        flag = true;
    }
    if (M045 == 1)    //msk keep PF drive error latched 17-may-2026
    {
        Error_No = ERR_DRIVE_M019;
        M044 = 1;
        flag = true;
    }
    if (M041 == 1)    //msk keep belt track error latched 17-may-2026
    {
        Error_No = 4;
        M044 = 1;
        flag = true;
    }
    return flag;
}
//-------------------------------------------------------------------------------------
void Ext_WF_Controls()
{
    static bool PrvRemoteLocalMode;
    static char rl_mode_init = 0;
    bool CurrentRemoteLocalMode;

    CurrentRemoteLocalMode = (M008 == 1) ? true : false;
    if (rl_mode_init == 0)
    {
        PrvRemoteLocalMode = CurrentRemoteLocalMode;
        rl_mode_init = 1;
    }
    else if (PrvRemoteLocalMode != CurrentRemoteLocalMode)
    {
        //msk 22-jun-26: force stop when rem/local/rem transition; fresh start required
        PrvRemoteLocalMode = CurrentRemoteLocalMode;
        Start_Stop_status = STOP;
        M066 = 0;
        M011 = 0;   //local start =0 (Stopped)
        M012 = 1;   //local stop =1 (stopped)
        M013 = 0;   //local pf start =0 (Stopped)
        M014 = 1;   //local pf stop =1 (stopped)
        M016 = 0;   //remote start/stop =0 (Stopped)    
        M067 = 0;   //pre-feeder start =0 (Stopped)
        Zero_Tacho_Feed_Buffer_Flag = 2;
        return;
    }

//    if(M044 == 1)
//   {
//       return;//General Fault
//    }
    if(M008==1)//Remote mode
    {
        if((Operating_Mode == Int_Gra_Com)||(Operating_Mode == Int_Vol_Com))
        {
            if(M002 == 1)   //Remote communication mode in interlocked state
            {
                if((M000 == 1) && (M044 == 0))//Start remote (Communication mode)
                {
                    Start_Stop_status = START;
                    M066 = 1;//START 
                }
                else if(M001 == 1)//Stop remote (Communication mode)
                {
                    Start_Stop_status = STOP;
                    M066 = 0;//Stop 
                    Zero_Tacho_Feed_Buffer_Flag = 2;
                }
                Current_Setpoint = Common_Setpoint;
            }// if(M002 == 1) //Remote communication mode
        }// if((Operating_Mode == Int_gra_Com)||(Operating_Mode == Int_vol_Com))
        else if(((Operating_Mode == Int_Gra_Dio) || (Operating_Mode == Int_Vol_Dio)) &&
         (M002 == 0))//SIO(M002=0): interlock start via M016, setpoint from analog AI
        {
            if((M016 == 1) && (M044 == 0))//Start remote interlocked
            {
                Start_Stop_status = START;
                M066 = 1;//START 
            }
            else
            {
                Start_Stop_status = STOP;
                M066 = 0;// remote Stop 
                Zero_Tacho_Feed_Buffer_Flag = 2;

            }
            Current_Setpoint = External_Setpoint;//
            //Interlocked mode volometric or gravimetric will
            //follow external setpoint
        }// if((Operating_Mode == Int_gra_Dio)||(Operating_Mode == Int_vol_Dio))

        if(GetDeIntGraMode() == true)
        {
            Current_Setpoint = Internal_Setpoint;
            //Deinterlocked gravimetric mode will have 
            // internal setpoint (Setpoint_Int)
        }
        else if(GetDeIntVolMode() == true)
        {
            Current_Setpoint = Manual_Setpoint;
            //deinterlocked volumetric mode will have 
            // internal manual setpoint (Setpoint_Man)
        }
        if (M003 == 1)
        {
            Totaliser1_Display = 0;
            Totaliser1 = 0;
            Totaliser1_Update = 0;
            M003 = 0;//Clear Totaliser1 Clear Marker
        }
        if (Current_Setpoint > Capacity)
            Current_Setpoint = Capacity;
        M051 = 0; // Output Marker M051;0-Remote ; 1-Local(Field mode)
    }// if(M008==1)//Remote mode
    else if(M008 == 0)    // Local/field panel (M008: 0=local, 1=remote; selector typically I026)
    {
       // Mode_Status = Local;
        M051 = 1; // Output Marker M051;0-Remote ; 1-Local
        M052 = 0; // Volumetric Mode
        if ((M011 == 1) && (M010 == 1) && 
        !WfDriveOrVfdFaultActive() && (M020 == 1) && (M044 == 0))
        {
            //M011= set for Field Start, and M012 set for Field Stop
            //M010= emergency fault, 1=healthy, 0=error
            //M019= drive healthy-wf, 1=healthy, 0=error
            //M020= drive healthy-pf, 1=healthy, 0=error        
            M066 = 1;//m066= wf-start/stop/fault, 1=start
            Start_Stop_status = START;
            M012 = 0;//m12= local  wf, 1=stop
        }
        else if (WfDriveOrVfdFaultActive() || (M020 == 0) || (M010 == 0))
        { //msk local/field: DIO, VFD fault, PF drive, emergency
            if (M010 == 0)
            {
                M066 = 0;
                Start_Stop_status = STOP;
                M044 = 1;
                Zero_Tacho_Feed_Buffer_Flag = 2;
                Error_No = 9;
            }
            else if (WfDriveOrVfdFaultActive())
            {
                ApplyWfDriveFaultTrip();
                if (M020 == 0)
                    M045 = 1;
            }
            else if (M020 == 0)
            {
                M067 = 0;
                M066 = 0;
                Start_Stop_status = STOP;
                M044 = 1;
                M045 = 1;
                Zero_Tacho_Feed_Buffer_Flag = 2;
                Error_No = ERR_DRIVE_M019;
            }
        }
        //M011 set for Field Start, and M012 set for Field Stop
        else if (M012 == 1) {//M12= local stop wf, 1=stop
            M066 = 0;   //m66= wf-start/stop/fault, 0=stop
            Start_Stop_status = STOP;
            M011 = 0; //m11= local start wf, 1=start, 0=stop
            Zero_Tacho_Feed_Buffer_Flag = 2;
        }

        if (M028 == 1) // Local_Vibrator On_Off(1-On/0-Off)
        {
            M069 = 1; // Vibrator Motor On-Off(1-On/0-Off)
        } else {
            M069 = 0; // Vibrator Motor On-Off(1-On/0-Off)
        }

        if (Prefeeder == 1) {
            if (M013 == 1) // Push button Switch Field Start
            {   //M013 set for Pre-feeder Field Start, and
                //M014 set for Pre-feeder Field Stop, 
                M067 = 1;
                // Start_Stop_status = START;
                M014 = 0;
            }

            if (M014 == 1) // Push button Switch Field Stop
            {   //M013 set for Pre-feeder Field Start, and
                //M014 set for Pre-feeder Field Stop, if M013 is 1 and M014 is 0
                M067 = 0;
                // Start_Stop_status = STOP;
                M013 = 0;
            }
        }// if (Prefeeder == 1) {
    }// if(M008 == 0) local/field panel
    else
    {// Not any known operating mode msk 22 march 2026
        M051 = 0; // Output Marker M051;0-Remote ; 1-Local
        M066 = 0;   //m66= wf-start/stop/fault, 0=stop
        Start_Stop_status = STOP;
        M011 = 0; //m11= local start wf, 1=start, 0=stop
        Zero_Tacho_Feed_Buffer_Flag = 2;
    }
    if (Pre_Hopper == 1) // If Prehopper is selected
    {
        //if ((Start_Stop_status == START) && (M025 == 1) && ((Mode_Status == Interlock) || (Mode_Status == De_Interlock)))
				if ((Start_Stop_status == START) && (M025 == 1) && (GetIntMode() || GetDeIntMode()))
				{
						ACAL_Status = 1;
				}
        if ((M026 == 1) && (ACAL_Status == 2)) {
            ACAL_Status = 0;
            M077 = 1; // Auto cal aborted
        }
        if (M027 == 1) // Accept Auto cal Val marker
        {
            if (ACAL_Status == 3) // Accept auto cal
            {
                ACAL_Status = 0;
                After_AutoCAL();
            }

            if (ACAL_Status == 4) // Greater than max limit
            {
                ACAL_Status = 0;
                M078 = 1; // Limit Exeeded
            }
        }//if (M027 == 1) ends
        if (M024 == 1) // BIN_PF Start/stop Feedback
            BIN_PF_Start_Status = 1;
        else
            BIN_PF_Start_Status = 0;
    }//if (Pre_Hopper == 1) ends
}//End of Ext_WF_Controls()

//-------------------------------------------------------------------------------------
void ControlActions()
{
    Update_External_Setpoint();
    Operating_Mode = GetOperatingMode(Operating_Mode);
    CurTripStatus = UpdateTripStatus();
    CurErrorStatus = UpdateErrorStatus();
    Ext_WF_Controls();
    /* Last: VFD fault must win over field start held (M011) / remote run */
    if (WfDriveOrVfdFaultActive())
        ApplyWfDriveFaultTrip();
}
//-------------------------------------------------------------------------------------

///-----
/*
Test SPSS


*/