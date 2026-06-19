/* adc.c */
#include "adc.h"
#include "GPIO_LPC17xx.h"
#include "lpc17xx_uart.h"
#include "LPC17xx.h"                    // Device header
#include "timer.h"
#include "OLED_2_7_12864.h"
#include "spi.h"
#include "parameters.h"  //#include "menu.h"
#include "functions.h"
#include "RAM512KB_Ext.h"

#define ADC_READY_TIMEOUT_TICKS  200U  //msk ~1 s cal ready max 26-may-2026

    char conv_comp;
volatile uint8_t adc_spi_recovery_count = 0; //msk 26-may-2026
static uint8_t adc_in_spi_recovery = 0;
double   *filterconst = NULL;
double   xv_wf[5]={0};
double   yv_wf[5]={0};
long temp3lc;
double   xv_h[5]={0};
double   yv_h[5]={0};

int LCcounts,pfcount;

char adc_conv_l,adc_conv_h,adc_conv_2nd_ch_l,adc_conv_2nd_ch_h,OF_Flag_ch1,OF_Flag_ch2;
unsigned long rx_data;
extern uint16_t New_menu_parameters[105];
extern unsigned long adc_final_val,bsf_adc_final_val,bsf_adc_final_val_2ch;
extern unsigned long Present_Count,Present_Count_ch2,Count_DL,Count_DL_ch2,Span_Count,Span_Count_ch2,zero_load,zero_load_ch2,full_load,full_load_ch2;
extern char Status_No,Error_No;
unsigned char ch_id_flag,adc_conv_counter_ch1,adc_conv_counter_ch2;
void     load_cell_avg_processing(void);
//------------------------------------------------------------------------
double* getfilterpara( int rate,int frequency)
{
    static double q[6];
    switch(rate)
    {
            case (20):
            switch(frequency)
            {
                case 1: q[0]=2.400388646e+03; q[1]=-0.4382651423; q[2]= 2.1121553551; q[3]=  -3.8611943490; q[4]= 3.1806385489;break;
                case 2: q[0]=2.072820954e+02; q[1]= -0.1873794924 ; q[2]= 1.0546654059; q[3]= -2.3139884144 ; q[4]= 2.3695130072;break;
                case 3: q[0]=5.387057197e+01; q[1]=-0.0761970646; q[2]= 0.4844033683; q[3]= -1.2756133250; q[4]= 1.5703988512;break;
                case 5: q[0]=1.064046542e+01; q[1]=-0.0176648009; q[2]= 0.0000000000; q[3]= -0.4860288221; q[4]= 0.0000000000;break;
                default: q[0]=1.064046542e+01; q[1]=-0.0176648009; q[2]= 0.0000000000; q[3]= -0.4860288221; q[4]= 0.0000000000;break;//5hz
            }    //done
        break;
            case (40):
            switch(frequency)
            {
                case 1:q[0]=3.201129162e+04 ;q[1]=-0.6630104844 ;q[2]=2.9240526562 ;q[3]=-4.8512758825 ;q[4]=3.5897338871 ;break;//d
                case 2:q[0]=2.400388646e+03 ;q[1]=-0.4382651423 ;q[2]=2.1121553551 ;q[3]=-3.8611943490 ;q[4]=3.1806385489 ;break;//d
                case 3:q[0]=5.609752018e+02 ;q[1]=-0.2879304294 ;q[2]=1.5047650514 ;q[3]=-3.0190386994 ;q[4]=2.7736823175 ;break;//dd/
                case 5:q[0]=9.794817390e+01 ;q[1]=-0.1203895999 ;q[2]=0.7244708295 ;q[3]=-1.7358607092 ;q[4]=1.9684277869 ;break;//d
                case 10:q[0]=1.064046542e+01 ;q[1]=-0.0176648009 ;q[2]=0.0000000000 ;q[3]=-0.4860288221 ;q[4]=0.0000000000 ;break;//d
                case 15:q[0]=2.883325031e+00 ;q[1]=-0.1203895999 ;q[2]=-0.7244708295 ;q[3]=-1.7358607092 ;q[4]=-1.9684277869 ;break;//d
                //case 15://mah0320_1
                default:q[0]=2.883325031e+00 ;q[1]=-0.1203895999 ;q[2]=-0.7244708295 ;q[3]=-1.7358607092 ;q[4]=-1.9684277869 ;break;//d
            }
        break;
        case (100):
            switch(frequency)
            {
                case 1:q[0]=1.112983215e+06;q[1]=-0.8485559993 ;q[2]=3.5335352195 ;q[3]=-5.5208191366 ;q[4]=3.8358255406  ;break;//d
                case 2:q[0]=7.522343863e+04 ;q[1]=-0.7199103273  ;q[2]=3.1159669252 ;q[3]=-5.0679983867 ;q[4]=3.6717290892 ;break;    //d
                case 3:q[0]=1.602898462e+04 ;q[1]=-0.6105348076 ;q[2]=2.7426528211 ;q[3]=-4.6409024127 ;q[4]=3.5077862074 ;break;    //d
                case 5:q[0]=2.400388646e+03 ;q[1]=-0.4382651423 ;q[2]=2.1121553551 ;q[3]=-3.8611943490 ;q[4]=3.1806385489 ;break;//d
                case 10:q[0]=2.072820954e+02 ;q[1]=-0.1873794924 ;q[2]=1.0546654059 ;q[3]=-2.3139884144 ;q[4]=2.3695130072 ;break;//d
                case 15:q[0]=5.387057197e+01 ;q[1]=-0.0761970646 ;q[2]=0.4844033683 ;q[3]=-1.2756133250  ;q[4]=1.5703988512 ;break;//d
                case 20:q[0]=2.146710182e+01 ;q[1]=-0.0301188750 ;q[2]=0.1826756978 ;q[3]=-0.6799785269 ;q[4]=0.7820951980 ;break;//d
              //    case 20://mah0320_1
                default:q[0]=2.146710182e+01 ;q[1]=-0.0301188750 ;q[2]=0.1826756978 ;q[3]=-0.6799785269 ;q[4]=0.7820951980 ;break;//d
            }
        }
        filterconst =&q[0];
    return(&filterconst[0]);
    }
//------------------------------------------------------------------------
//-----------------------------------------------------------------------------------//
double apply_filter_WF(double adccounts,int rate,int frequency)
{
      volatile double filterop;
    filterconst=getfilterpara( rate, frequency);

    xv_wf[0] = xv_wf[1]; xv_wf[1] = xv_wf[2]; xv_wf[2] = xv_wf[3]; xv_wf[3] = xv_wf[4];
    xv_wf[4] = adccounts / filterconst[0];
    yv_wf[0] = yv_wf[1]; yv_wf[1] = yv_wf[2]; yv_wf[2] = yv_wf[3]; yv_wf[3] = yv_wf[4];
    yv_wf[4] =   (xv_wf[0] + xv_wf[4]) + 4 * (xv_wf[1] + xv_wf[3]) + 6 * xv_wf[2]
                             + ( filterconst[1] * yv_wf[0]) + (  filterconst[2] * yv_wf[1])
                             + ( filterconst[3] * yv_wf[2]) + (  filterconst[4] * yv_wf[3]);
    filterop = yv_wf[4];
    return(filterop);
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------//
double apply_filter_Hopper(double adccounts,int rate,int frequency)
{
  volatile double filterop;
        filterconst=getfilterpara( rate, frequency);
    xv_h[0] = xv_h[1]; xv_h[1] = xv_h[2]; xv_h[2] = xv_h[3]; xv_h[3] = xv_h[4];
    xv_h[4] = adccounts / filterconst[0];
    yv_h[0] = yv_h[1]; yv_h[1] = yv_h[2]; yv_h[2] = yv_h[3]; yv_h[3] = yv_h[4];
    yv_h[4] =   (xv_h[0] + xv_h[4]) + 4 * (xv_h[1] + xv_h[3]) + 6 * xv_h[2]
                             + ( filterconst[1] * yv_h[0]) + (  filterconst[2] * yv_h[1])
                             + ( filterconst[3] * yv_h[2]) + (  filterconst[4] * yv_h[3]);
    filterop = yv_h[4];
    return(filterop);
}
//-----------------------------------------------------------------------------------//
//----------------------------------------------------------------------
//    ADC_CHIP SELECT
void adc_cs(int on)
{
int i;
  GPIO_PinWrite (adc_cs_port,adc_cs_pin, on);
    for(i=0;i < 100;i++){;}
}

static char adc_wait_cal_ready(void) //msk replace infinite conv_comp wait 26-may-2026
{
    uint32_t start_tick;

    start_tick = spi_wait_tick;
    do {
        super_delay(1);
        conv_comp = GPIO_PinRead(0, 17);
        if ((spi_wait_tick - start_tick) >= ADC_READY_TIMEOUT_TICKS) { //msk 26-may-2026
            return 0;
        }
    } while (conv_comp == 1);
    return 1;
}
//------------------------------------------------------------------------------
//    Initialize 24-Bit ADC CS5532
void init_adc_cs5532(void)
{
    unsigned char x;
  GPIO_SetDir (adc_cs_port,adc_cs_pin, 1);            ////Initialize Chip Select for 24-Bit ADC
    super_delay(20);

    adc_cs(0);
    super_delay(1);
//re_initialisation of serial port to set serial port in command mode
for(x =0;x<15;x++)
    spi_write(0xff);                //  15 SYNC1 bytes
    spi_write(0xfe);                  //   1 SYNC0 bytes

//systemreset sequence

spi_write(0x03);                  //write to config word
spi_write(0x20);                  //send 0x20000000    set rs bit
spi_write(0x00);                  //send 0x00 to remaining 24 bits
spi_write(0x00);
spi_write(0x00);

super_delay(10);    //msk 19-aug-25 20 micro sec delay after RS bit

spi_write(0x0B);                 //read config reg
rx_data=adc_read();

spi_write(0x03);                 //write config reg
spi_write(0x00);                //clear all 32 bytes i.e send 0x00000000 data to reset RS bit
spi_write(0x08);                //vrs=0,frs=1
spi_write(0x00);
spi_write(0x00);

    adc_cs(1);

    AD_Conversion_Rate= 5; //msk 22-aug-25 single conversion mode is forced

switch(AD_Conversion_Rate)
{
    case 0:    adc_conv_l=0x40;                                //6.25//15gain//unipolar
                    adc_conv_h=0x32;                                //0100b
                    adc_conv_2nd_ch_l =    0x40;                //400
                    adc_conv_2nd_ch_h = 0x72;                //1011b
                    break;
    case 1:    adc_conv_l=0xc0;                                //12.5
                    adc_conv_h=0x31;                                //0011b
                    adc_conv_2nd_ch_l =    0xc0;                //400
                    adc_conv_2nd_ch_h = 0x71;                //1011b
                    break;
    case 2: adc_conv_l=0x40    ;                                //25
                    adc_conv_h=0x31;                                //0010b
                    adc_conv_2nd_ch_l =    0x40;                //400
                    adc_conv_2nd_ch_h = 0x71;                //1011b
                    break;
    case 3: adc_conv_l=0xC0;                                //50
                    adc_conv_h=0x30;                                //0001b
                    adc_conv_2nd_ch_l =    0xc0;                //400
                    adc_conv_2nd_ch_h = 0x70;                //1011b
                    break;

    case 4:    adc_conv_l=    0x40;                                //100
                    adc_conv_h= 0x30;                                //0000b
                    adc_conv_2nd_ch_l =    0x40;                //400
                    adc_conv_2nd_ch_h = 0x70;                //1011b
                    break;
    case 5:    adc_conv_l=    0x40;                                //200
                    adc_conv_h= 0x36;                                //1100b
                    adc_conv_2nd_ch_l =    0x40;                //400
                    adc_conv_2nd_ch_h = 0x76;                //1011b
                    break;
    case 6:    adc_conv_l=    0xc0;                                //400
                    adc_conv_h= 0x35;                                //1011b
                    adc_conv_2nd_ch_l =    0xc0;                //400
                    adc_conv_2nd_ch_h = 0x75;                //1011b
                    break;
    default:break;
}

    adc_cs(0);
    super_delay(1);

spi_write(0x05);                          //write to channel setup register
spi_write(adc_conv_h);                    //setup1 wordrate = 240/200 sps and unipolar output
spi_write(adc_conv_l);                    //0x3640 Gain = 64,(Input Span = [(VREF+)-(VREF-)]/64*A for unipolar)
spi_write(adc_conv_2nd_ch_h);                   //0x0000 for setup 2
spi_write(adc_conv_2nd_ch_l);                //2D40 - Gain 32, 800sps  //2CC0 - 1600sps, gain 32
adc_cs(1);
super_delay(1);
adc_cs(0);
super_delay(1);
spi_write(0x15);        //ch setup reg no 2
spi_write(0x40);        //ch setup reg pointer = 010b slowest speed for self offset and gain calibration
spi_write(0x32);        //ch setup reg pointer = 010b
spi_write(0x40);        //ch setup reg pointer = 011b slowest speed for self offset and gain calibration
spi_write(0x72);        //ch setup reg pointer = 011b
adc_cs(1);
super_delay(1);

//-------------------------

adc_cs(0);
spi_write(0x91);        //self offset msk 19-aug-25
(void)adc_wait_cal_ready();

adc_cs(1);
super_delay(1);

adc_cs(0);
spi_write(0x92);        //self offset msk 19-aug-25
(void)adc_wait_cal_ready();

adc_cs(1);
super_delay(1);

//--------------------------------------------
adc_cs(0);
//Pre_Hopper=0;        //msk made zero 10-aug-25
spi_write(0x80);    //    Start_Loop single conversion
    adc_cs(1);
}

void adc_spi_fault_recovery(void) //msk SPI bus hang recovery 26-may-2026
{
    if (adc_in_spi_recovery != 0U) {
        return;
    }

    adc_in_spi_recovery = 1U;

    adc_cs(1);
    eeprom_cs(1);
    ram_cs(1);

    SPI_Initialize();

    spi_timeout_enable(0U);
    init_adc_cs5532(); //msk full ADC reset + internal cal 26-may-2026
    spi_timeout_enable(1U);

    adc_spi_recovery_count++;
    adc_in_spi_recovery = 0U;
}
//-----------------------------------------------------------------------
//        Read ADC Conversion value if Ready
void read_adc_conversion(void)
{
    //char conv_comp;

    adc_cs(0);
    GPIO_PinWrite(0,17,1);
    conv_comp =GPIO_PinRead(0,17);
    adc_cs(1);
    super_delay(1);

if(conv_comp==0)  //adc data ready
    {
        adc_cs(0);
//        super_delay(15);
        rx_data=0;
            rx_data = adc_read_value();                   //read adc data
          rx_data = rx_data >> 4;
        adc_cs(1);

        if(bsf_adc_final_val > 16777999)
             bsf_adc_final_val = 0;

        if(Pre_Hopper==1)
            {
                if(ch_id_flag==1)    //1 value means the reading is of hopper
                {
                        //filter_calculation();                    //Bessel FIlter
                    bsf_adc_final_val_2ch=(unsigned long) apply_filter_Hopper((double) rx_data, 20,10);
                        //bsf_filter_calculation_2ch();    //Band Stop Filter

                    if(bsf_adc_final_val_2ch > 16777999)
                                bsf_adc_final_val_2ch = 0;

                        Present_Count_ch2 = bsf_adc_final_val_2ch;
                    }
                else    // if ch_id_flag = 0 means read value is for WF  mainloadcelll
                    {
                        //filter_calculation();            //Bessel FIlter
                        //sf_filter_calculation();    //Band Stop Filter
                        bsf_adc_final_val =(unsigned long) apply_filter_WF((double) rx_data, 20,5);
                        load_cell_avg_processing();
                        if(bsf_adc_final_val > 16777999)
                                bsf_adc_final_val = 0;
                        Present_Count = bsf_adc_final_val;
                    }
            }
        else
            {
                //filter_calculation();            //Bessel FIlter
                //bsf_filter_calculation();    //Band Stop Filter
                bsf_adc_final_val =(unsigned long) apply_filter_WF((double) rx_data, 40 ,5);
                load_cell_avg_processing();
                //bsf_adc_final_val = bsf_adc_final_val >>  4;
                if(bsf_adc_final_val > 16777999)
                                bsf_adc_final_val = 0;
                Present_Count = bsf_adc_final_val;
            }
        }
return;
}
//--------------------------------------------------------
//    READ 24-Bit ADC Value
long adc_read_value(void)
{

     if(Pre_Hopper==0)        //ch1 Main loadcell
            {
             adc_cs(0);
                spi_write(0xFF);
             temp3lc=adc_read();
                 adc_cs(1);
                        adc_cs(0);
                    spi_write(0x80);                   //start single (MSK) conversion with ch1_setup as adc in single read mode
                    adc_cs(1);
                adc_conv_counter_ch1++;
                OF_Flag_ch1 = ((temp3lc & 0x00000004)>>2);
                temp3lc=temp3lc>>8;               //shift right by 8 to get 24 bit adc data
                temp3lc=temp3lc&0xFFFFFF;       //clear last 8 unused bits (24 bit output from adc)
             LCcounts=temp3lc;
                return temp3lc;
            }
     else
         {
             adc_cs(0);
                spi_write(0xFF);
             temp3lc=adc_read();
                 adc_cs(1);

                if(temp3lc & 0x00000001)//ch2  Prehopper loadcell
                {
                    adc_cs(0);
                    spi_write(0x80);                   //start single (MSK) conversion with ch1_setup as adc in single read mode
                    adc_cs(1);
                     ch_id_flag = 1;                        //1 value means the reading is of hopper
                     adc_conv_counter_ch2++;
                     OF_Flag_ch2 = ((temp3lc & 0x00000004)>>2);
                     temp3lc=temp3lc>>8;               //shift right by 8 to get 24 bit adc data
                     temp3lc=temp3lc&0xFFFFFF;       //clear last 8 unused bits (24 bit output from adc)
                    pfcount=temp3lc;
                     return temp3lc;
                }
                else if((temp3lc & 0x00000000)==0) //ch1 Main loadcell
                {
                    adc_cs(0);
                    spi_write(0x88);                   //start SINGLE (MSK) conversion with ch2_setup as adc in single read mode
                        adc_cs(1);
                        ch_id_flag = 0;                    // if ch_id_flag = 0 means read value is for WF  main loadcelll
                        adc_conv_counter_ch1++;
                        OF_Flag_ch1 = ((temp3lc & 0x00000004)>>2);
                        temp3lc=temp3lc >>8;               //shift right by 8 to get 24 bit adc data
                            temp3lc=temp3lc &0xFFFFFF;       //clear last 8 unused bits (24 bit output from adc)
                                LCcounts=temp3lc;
                        return temp3lc;
                }
         }

    // Defensive fallback: in case no branch returned (prevents undefined return value).
    return 0;
}
//----------------------------------------------------------------
//    Read 32-Bit ADC Data Register
long adc_read()
{
    unsigned char y,temp2=0,*p;            // store 8 bit of ACC data
    long temp3=0;            //store 32 bits of input data from slave adc

        p = (unsigned char*)&temp3;
        OLED_uDelay2();

    for(y=0;y<4;y++)                    //reads 32 bits of data from conversion register............
        {
        *(p+3-y) = spi_read();
        }
    return temp3;
}

//End of File
