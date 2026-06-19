/*    plc.h    */
#include <stdlib.h>
#include <stdint.h>

#define RAM_SIZE 200
//#define feedrate_buffer 140

//#define TIMER1     184
//#define TIMER2     185
//#define TIMER3     186
//#define TIMER4     187
//#define TIMER5     188
//#define TIMER6     189
//#define TIMER7     190
//#define TIMER8     191
//#define TIMER9     192
//#define TIMER10 193

//#define T000     184
//#define T001     185
//#define T002  186
//#define T003  187
//#define T004  188
//#define T005     189
//#define T006    190
//#define T007    191
//#define T008    192
//#define T009    193

typedef struct{            //structure for PLC Assembled program
    uint8_t inst;
    uint8_t addr;
}plc_instruction_struct;

typedef struct{            //structure for PLC Editor Program
    char *inst[3];
    int addr;
}plc_edit_inst;

/*
typedef struct{
    char ram_type;
    char ram_addr;
}ram_struct;
//should be used as address to ram_values[250]

*/

void    plc_execute(void );

    // ;INPUTS (IO_ADDRESS+0...IO_ADDRESS+39)
#define    I000    *(    ram_values+0        )
#define    I001    *(    ram_values+1        )
#define    I002    *(    ram_values+2        )
#define    I003    *(    ram_values+3        )
#define    I004    *(    ram_values+4        )
#define    I005    *(    ram_values+5        )
#define    I006    *(    ram_values+6        )
#define    I007    *(    ram_values+7        )
#define    I008    *(    ram_values+8        )
#define    I009    *(    ram_values+9        )
#define    I010    *(    ram_values+10        )
#define    I011    *(    ram_values+11        )
#define    I012    *(    ram_values+12        )
#define    I013    *(    ram_values+13        )
#define    I014    *(    ram_values+14        )
#define    I015    *(    ram_values+15        )
#define    I016    *(    ram_values+16        )
#define    I017    *(    ram_values+17        )
#define    I018    *(    ram_values+18        )
#define    I019    *(    ram_values+19        )
#define    I020    *(    ram_values+20        )
#define    I021    *(    ram_values+21        )
#define    I022    *(    ram_values+22        )
#define    I023    *(    ram_values+23        )
#define    I024    *(    ram_values+24        )
#define    I025    *(    ram_values+25        )
#define    I026    *(    ram_values+26        )
#define    I027    *(    ram_values+27        )
#define    I028    *(    ram_values+28        )
#define    I029    *(    ram_values+29        )
#define    I030    *(    ram_values+30        )
#define    I031    *(    ram_values+31        )
#define    I032    *(    ram_values+32        )
#define    I033    *(    ram_values+33        )
#define    I034    *(    ram_values+34        )
#define    I035    *(    ram_values+35        )
#define    I036    *(    ram_values+36        )
#define    I037    *(    ram_values+37        )
#define    I038    *(    ram_values+38        )
#define    I039    *(    ram_values+39        )

//    ;OUTPUTS (IO_ADDRESS+40...IO_ADDRESS+79)
#define    O000    *(    ram_values+40        )
#define    O001    *(    ram_values+41        )
#define    O002    *(    ram_values+42        )
#define    O003    *(    ram_values+43        )
#define    O004    *(    ram_values+44        )
#define    O005    *(    ram_values+45        )
#define    O006    *(    ram_values+46        )
#define    O007    *(    ram_values+47        )
#define    O008    *(    ram_values+48        )
#define    O009    *(    ram_values+49        )
#define    O010    *(    ram_values+50        )
#define    O011    *(    ram_values+51        )
#define    O012    *(    ram_values+52        )
#define    O013    *(    ram_values+53        )
#define    O014    *(    ram_values+54        )
#define    O015    *(    ram_values+55        )
#define    O016    *(    ram_values+56        )
#define    O017    *(    ram_values+57        )
#define    O018    *(    ram_values+58        )
#define    O019    *(    ram_values+59        )
#define    O020    *(    ram_values+60        )
#define    O021    *(    ram_values+61        )
#define    O022    *(    ram_values+62        )
#define    O023    *(    ram_values+63        )
#define    O024    *(    ram_values+64        )
#define    O025    *(    ram_values+65        )
#define    O026    *(    ram_values+66        )
#define    O027    *(    ram_values+67        )
#define    O028    *(    ram_values+68        )
#define    O029    *(    ram_values+69        )
#define    O030    *(    ram_values+70        )
#define    O031    *(    ram_values+71        )
#define    O032    *(    ram_values+72        )
#define    O033    *(    ram_values+73        )
#define    O034    *(    ram_values+74        )
#define    O035    *(    ram_values+75        )
#define    O036    *(    ram_values+76        )
#define    O037    *(    ram_values+77        )
#define    O038    *(    ram_values+78        )
#define    O039    *(    ram_values+79        )

//;MEMORY BITS-INPUTS (IO_ADDRESS+80...IO_ADDRESS+127)
#define    M000    *(    ram_values+80        )  //start remote
#define    M001    *(    ram_values+81        )  //stop remote
#define    M002    *(    ram_values+82        )  //remote on off
#define    M003    *(    ram_values+83        )  //reset totaliser1
#define    M004    *(    ram_values+84        )  //free
#define    M005    *(    ram_values+85        )  //free
#define    M006    *(    ram_values+86        )  //free
#define    M007    *(    ram_values+87        )     //Gravimetric Mode (1-Gravimetric, 0-Volumetric)

#define    M008    *(    ram_values+88        )  //local remote selection(0-local,1-Rmote )
#define    M009    *(    ram_values+89        )    //Belt tracking input
#define    M010    *(    ram_values+90        )    //Emg off (0-Error,1-Healthy)
#define    M011    *(    ram_values+91        )    //local start WF(1-start)
#define    M012    *(    ram_values+92        ) ////local stop WF(1-stop)
#define    M013    *(    ram_values+93        ) //local start-PF
#define    M014    *(    ram_values+94        ) //local stop-PF
#define    M015    *(    ram_values+95        ) //Int/De-Interlock Mode Select (0-Int,1-De-Interlock)

#define    M016    *(    ram_values+96        ) //Remote Start/Stop
#define    M017    *(    ram_values+97        )    //Downstrem Interlock
#define    M018    *(    ram_values+98        )    //Error Reset
#define    M019    *(    ram_values+99        ) //Drive Healthy-WF(1-Healthy)
#define    M020    *(    ram_values+100        )//Drive Healthy-PF(1-Healthy)
#define    M021    *(    ram_values+101        )//Dosing_Valve_Forward (SFF)
#define    M022    *(    ram_values+102        )//Dosing_Valve_Reverse (SFF)
#define    M023    *(    ram_values+103        )//Free

#define    M024    *(    ram_values+104        )//BIN PRE-FEEDER START/STOP FEEDBACK
#define    M025    *(    ram_values+105        )//START CONTROL MEASURING
#define    M026    *(    ram_values+106        )//STOP CONTROL MEASURING
#define    M027    *(    ram_values+107        )//ACCEPT CORRECTION
#define    M028    *(    ram_values+108        )//Local Vibrator ON_Off
#define    M029    *(    ram_values+109        )//Material Detect switch
#define    M030    *(    ram_values+110        )
#define    M031    *(    ram_values+111        )
#define    M032    *(    ram_values+112        )
#define    M033    *(    ram_values+113        )
#define    M034    *(    ram_values+114        )
#define    M035    *(    ram_values+115        )
#define    M036    *(    ram_values+116        )
#define    M037    *(    ram_values+117        )
#define    M038    *(    ram_values+118        )
#define    M039    *(    ram_values+119        )

#define    M040    *(    ram_values+120        )      //Tacho Fault
#define    M041    *(    ram_values+121        )        //Belt tracking Fault
#define    M042    *(    ram_values+122        )        //WF Drive Fault
#define    M043    *(    ram_values+123        )   //Deviation Fault
#define    M044    *(    ram_values+124        )        //General Fault
#define    M045    *(    ram_values+125        )   //PF Drive Fault
#define    M046    *(    ram_values+126        )
#define    M047    *(    ram_values+127        )

#define    M048    *(    ram_values+128        )
#define    M049    *(    ram_values+129        )
#define    M050    *(    ram_values+130        )
#define    M051    *(    ram_values+131        )    //local mode(0-remote,1-local)
#define    M052    *(    ram_values+132        )    //Gravimetric Mode(1-Gravimetric,0-Volumetric)
#define    M053    *(    ram_values+133        ) //Int/De-Int Mode(1-Interlock,0-De-Interlock)
#define    M054    *(    ram_values+134        )
#define    M055    *(    ram_values+135        )

#define    M056    *(    ram_values+136        ) //Limit1
#define    M057    *(    ram_values+137        ) //Limit2
#define    M058    *(    ram_values+138        )    //overload alarm
#define    M059    *(    ram_values+139        )    //underload alarm
#define    M060    *(    ram_values+140        )    //Loadcell Fault
#define    M061    *(    ram_values+141        ) //DIO Card comm Fault //Not Used
#define    M062    *(    ram_values+142        )    //Loadcell 2 Fault
#define    M063    *(    ram_values+143        )

#define    M064    *(    ram_values+144        )//WF-Start/Stop/Fault
#define    M065    *(    ram_values+145        )//PF-Start/Stop/Fault
#define    M066    *(    ram_values+146        )//WF-Start/Stop
#define    M067    *(    ram_values+147        )//PF-Start/Stop
#define    M068    *(    ram_values+148        )//System Healthy(1-Healthy)
#define    M069    *(    ram_values+149        )//Vibrator On(1-On/0-Off)
#define    M070    *(    ram_values+150        )
#define    M071    *(    ram_values+151        )

#define    M072    *(    ram_values+152        )//
#define    M073    *(    ram_values+153        )
#define    M074    *(    ram_values+154        )
#define    M075    *(    ram_values+155        )
#define    M076    *(    ram_values+156        )
#define    M077    *(    ram_values+157        )
#define    M078    *(    ram_values+158        )
#define    M079    *(    ram_values+159        )

#define    M080    *(    ram_values+160        )
#define    M081    *(    ram_values+161        )
#define    M082    *(    ram_values+162        )
#define    M083    *(    ram_values+163        )
#define    M084    *(    ram_values+164        )
#define    M085    *(    ram_values+165        )
#define    M086    *(    ram_values+166        )
#define    M087    *(    ram_values+167        )
#define    M088    *(    ram_values+168        )
#define    M089    *(    ram_values+169        )
#define    M090    *(    ram_values+170        )
#define    M091    *(    ram_values+171        )
#define    M092    *(    ram_values+172        )
#define    M093    *(    ram_values+173        )
#define    M094    *(    ram_values+174        )
#define    M095    *(    ram_values+175        )
#define    M096    *(    ram_values+176        )
#define    M097    *(    ram_values+177        )
#define    M098    *(    ram_values+178        )
#define    M099    *(    ram_values+179        )

#define T000     *(    ram_values+184        )//184
#define T001     *(    ram_values+185        )//185
#define T002  *(    ram_values+186        )//186
#define T003  *(    ram_values+187        )//187
#define T004  *(    ram_values+188        )//188
#define T005     *(    ram_values+189        )//189
#define T006    *(    ram_values+190        )//190
#define T007    *(    ram_values+191        )//191
#define T008    *(    ram_values+192        )//192
#define T009    *(    ram_values+193        )//193
