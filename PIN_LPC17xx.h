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
 * Project:      PIN Driver Definitions for NXP LPC17xx
 * -------------------------------------------------------------------------- */

#ifndef __PIN_LPC17XX_H
#define __PIN_LPC17XX_H

#include <stdint.h>
#include <stdbool.h>

// Pin Configuration

typedef struct _PIN
{
  uint8_t Portnum;   // Port Number
  uint8_t Pinnum;    // Pin Number
} PIN;


//------------------------------------------------------------------------------
// PINSEL REGISTER BIT DEFINITIONS
//------------------------------------------------------------------------------

// Pin Function selection
#define PIN_FUNC_0              ((uint32_t)(0))
#define PIN_FUNC_1              ((uint32_t)(1))
#define PIN_FUNC_2              ((uint32_t)(2))
#define PIN_FUNC_3              ((uint32_t)(3))

// Pin mode
#define PIN_PINMODE_PULLUP      ((uint32_t)(0))
#define PIN_PINMODE_REPEATER    ((uint32_t)(1))
#define PIN_PINMODE_TRISTATE    ((uint32_t)(2))
#define PIN_PINMODE_PULLDOWN    ((uint32_t)(3))

// Pin mode (normal/open drain)
#define	PIN_PINMODE_NORMAL      ((uint32_t)(0))
#define	PIN_PINMODE_OPENDRAIN   ((uint32_t)(1))

// I2C mode
#define PIN_I2C_Normal_Mode     ((uint32_t)(0))
#define PIN_I2C_Fast_Mode       ((uint32_t)(1))
#define PIN_I2C_Fast_Plus_Mode  ((uint32_t)(2))

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
extern int32_t PIN_Configure (uint8_t port, uint8_t pin, uint8_t function, uint8_t mode, uint8_t open_drain);

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
extern int32_t PIN_ConfigureTPIU (bool enable);

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
extern int32_t PIN_ConfigureI2C0Pins (uint8_t i2cPinMode, bool enableFilterSlewRate);

#endif // __PIN_LPC17XX_H
