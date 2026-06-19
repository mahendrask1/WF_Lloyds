/*     Dac.h    */

#define dac_1_cs_pin    22  //22
#define dac_1_cs_port    0

#define dac_CLK_pin    15    //15
#define dac_CLK_port    0

#define dac_DOUT_pin    18  //18
#define dac_DOUT_port    0

#define dac_2_cs_pin    26  //25
#define dac_2_cs_port    3

#define dac_3_cs_pin    25  //26
#define dac_3_cs_port    3

void init_dac(void);
void dac1_cs(int);
void dac2_cs(int);
void dac3_cs(int);
void dac_CLK(char);
void dac_DOUT(char);
void dac1_write(unsigned int);
void dac2_write(unsigned int);
void dac3_write(unsigned int);
