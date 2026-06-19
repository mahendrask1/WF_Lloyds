///*keys.h*/
#ifndef KEYS_H
#define KEYS_H
#include <stdbool.h>

/*New Key Function    val         `VT_Corp_W     VT_Corp_P                                        letters*/
#define Setpoint        1      //Setpoint           //Zero_Key
#define k_Load          2      //k_Load             //k_Discharge
#define k_Total1        3      //k_Total1           //k_Fast
#define Feed_rate       4      //Feed_rate          //Tare_Key
#define k_Speed     5      //k_Speed             //No_Function
#define k_Total2        6      //k_Total2           //k_Medium
#define k_Slow          9      //k_Slow             //k_Slow
#define HOR                     10          //HOR                     //HOR
#define K_DEC           11      //VER                     //VER
#define CL                      12      //CL                     //CL
#define K_INC           13      //ESC                    //ESC
#define ESC             14      //F_Key                 //F_Key
#define Auto_Man        15      //Zero_Key          //PRINT
#define INT_DE_INT      16      //INT_DE_INT         //Net_Gross   INT/DE-INT
#define Zero_Key        17          //Auto_Man          //Auto_Man
#define Start_Stop      18      //Start_Stop         //Start_Stop
#define k_SET       19      //Not Used            //To Enter into Menu
#define NO_KEY          0xFF    //NO_KEY             //NO_KEY

#define Key0 0
#define Key1 Setpoint
#define Key2 k_Load
#define Key3 k_Total1
#define Key4 Feed_rate
#define Key5 k_Speed
#define Key6 k_Total2
#define Key7 7
#define Key8 8
#define Key9 k_Slow
#define ENTER HOR

//  Status
#define START                   0
#define STOP                    1

/*Mode selction*/
#define Int    0   //Internal
#define Ext    1   //External

/* Mode status*/
//#define Interlock    0 //Interlock
//#define De_Interlock 1 //De_Interlock
//#define Manual       2 //Manual
//#define Auto         3 //Aauto    
//#define Local        5 //Local
//#define Remote       6 //Remote

#define Int_Gra_Dio 10  //Interlocked Gravimetric Digital Input Mode
#define DeInt_Gra_Dio 11  //De-Interlocked Gravimetric Digital Input Mode
#define Int_Vol_Dio 12  //Interlocked Volumetric Digital Input Mode
#define DeInt_Vol_Dio 13  //De-Interlocked Volumetric Digital Input Mode
#define Int_Gra_Com 14  //Interlocked Gravimetric Communication Mode
#define Int_Vol_Com 15  //Interlocked Volumetric Communication Mode 
#define FieldMode 16  //Field Mode

extern unsigned char Operating_Mode;
void key_check_routine(void);
void latch_control(char on);
//bool TripStatus; //Trip Status
//bool ErrorStatus; //Error Status    
extern char Start_Stop_status;
extern char Error_No;
extern unsigned char Zero_Tacho_Feed_Buffer_Flag,CM_Negative;
extern char Start_Stop_status, Error_No;
extern unsigned char Display_Tacho_Buffer, Display_Tacho_Pointer, Zero_Tacho_Feed_Buffer_Flag;
#endif /* KEYS_H */
