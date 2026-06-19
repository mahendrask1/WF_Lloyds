/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2014 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        13. June 2014
 * $Revision:    V1.00
 *
 * Project:      PIN Driver for NXP LPC17xx
 * -------------------------------------------------------------------------- */

#include "LPC17xx.h"
#include "PIN_LPC17xx.h"

#define PIN_Func(port,regidx) (*((volatile uint32_t *) (&(LPC_PINCON->PINSEL0)     + 2*port + regidx)))
#define PIN_Mode(port,regidx) (*((volatile uint32_t *) (&(LPC_PINCON->PINMODE0)    + 2*port + regidx)))
#define PIN_ModeOp(port)      (*((volatile uint32_t *) (&(LPC_PINCON->PINMODE_OD0) + port)))

// I2C Pin Configuration register bit description
#define PIN_I2CPADCFG_SDADRV0   ((uint32_t)(1 << 0))
#define PIN_I2CPADCFG_SDAI2C0   ((uint32_t)(1 << 1))
#define PIN_I2CPADCFG_SCLDRV0   ((uint32_t)(1 << 2))
#define PIN_I2CPADCFG_SCLI2C0   ((uint32_t)(1 << 3))

/**
  \fn          int32_t PIN_Configure (uint32_t function) {
  \brief       Set pin function and electrical characteristics
  \param[in]   port       port number (0..3)
  \param[in]   pin        pin number  (0..31)
  \param[in]   function   port pin function
  \param[in]   mode       port pin input mode
  \param[in]   open_drain port pin open drain mode
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t PIN_Configure (uint8_t port, uint8_t pin, uint8_t function, uint8_t mode, uint8_t open_drain) {
  uint32_t regidx = 0;
  uint8_t pinnum = pin;

  if (pin >= 16) {
    pinnum   = pin - 16;
    regidx = 1;
  }
  // Configure Pin function
  PIN_Func(port, regidx) &= ~(0x03UL << (pinnum * 2));
  PIN_Func(port, regidx) |= ((uint32_t)function) << (pinnum * 2);

  // Configure Register mode
  PIN_Mode(port, regidx) &= ~(0x03UL << (pinnum * 2));
  PIN_Mode(port, regidx) |= ((uint32_t)mode) << (pinnum * 2);
  
  // Configure Open drain mode
  if (open_drain == PIN_PINMODE_OPENDRAIN) {
    PIN_ModeOp(port) |=  (0x01UL << pin);
  }
  else {
    PIN_ModeOp(port) &= ~(0x01UL << pin);
  }
  return(0);  
}

/**
  \fn          int32_t PIN_ConfigureTPIU (bool enable)
  \brief       Configure trace function
  \param[in]   enable Enable or disable
                - true (1): enable
                - false(0): disable
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t PIN_ConfigureTPIU (bool enable)
{
  if (enable == true) {
    LPC_PINCON->PINSEL10 |=  (0x01UL << 3);
  } else {
    LPC_PINCON->PINSEL10 &= ~(0x01UL << 3);
  }
  return(0);
}

/**
  \fn          PIN_ConfigureI2C0Pins(uint8_t i2cPinMode, bool enableFilterSlewRate)
  \brief       Configure I2C0 pins
  \param[in]   i2cPinMode           I2C pin mode
                - PIN_I2C_Normal_Mode
                - PIN_I2C_Fast_Mode
                - PIN_I2C_Normal_Mode
  \param[in]   enableFilterSlewRate Enable or disable filter and slew rate
                - true (1): enable
                - false(0): disable
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t PIN_ConfigureI2C0Pins (uint8_t i2cPinMode, bool enableFilterSlewRate)
{
  uint32_t regVal = 0;

  if (i2cPinMode == PIN_I2C_Fast_Plus_Mode){
    regVal |= PIN_I2CPADCFG_SCLDRV0 | PIN_I2CPADCFG_SDADRV0;
  }

  if (enableFilterSlewRate == false){
    regVal |= PIN_I2CPADCFG_SCLI2C0 | PIN_I2CPADCFG_SDAI2C0;
  }
  LPC_PINCON->I2CPADCFG = regVal;
  return(0);
}
