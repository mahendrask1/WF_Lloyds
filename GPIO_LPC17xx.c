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
 * $Date:        05. May 2014
 * $Revision:    V1.00
 *
 * Project:      GPIO Device Driver for LPC17xxx
 * -------------------------------------------------------------------------- */

#include "LPC17xx.h"
#include "GPIO_LPC17xx.h"


#define LPC_GPIO(n)             ((LPC_GPIO_TypeDef *)(LPC_GPIO0_BASE + 0x00020*n))

/**
  \fn          void GPIO_PortClock (uint32_t clock)
  \brief       Port Clock Control
  \param[in]   clock  Enable or disable clock
*/
void GPIO_PortClock (uint32_t clock) {

  if (clock) {
    LPC_SC->PCONP |=  (1 << 15);
  }
  else {
    LPC_SC->PCONP &= ~(1 << 15);
  }
}

/**
  \fn          void GPIO_SetDir (uint32_t port_num,
                                 uint32_t pin_num,
                                 uint32_t dir)
  \brief       Configure GPIO pin direction
  \param[in]   port_num   GPIO number (0..4)
  \param[in]   pin_num    Port pin number
  \param[in]   dir        GPIO_DIR_INPUT, GPIO_DIR_OUTPUT
*/
void GPIO_SetDir (uint32_t port_num, uint32_t pin_num, uint32_t dir) {

  dir  ? (LPC_GPIO(port_num)->FIODIR |=  (1UL << pin_num)) : \
         (LPC_GPIO(port_num)->FIODIR &= ~(1UL << pin_num));
}

/**
  \fn          void GPIO_PinWrite (uint32_t port_num,
                                   uint32_t pin_num,
                                   uint32_t val);
  \brief       Write port pin
  \param[in]   port_num   GPIO number (0..4)
  \param[in]   pin_num    Port pin number
  \param[in]   val        Port pin value (0 or 1)
*/
void GPIO_PinWrite (uint32_t port_num, uint32_t pin_num, uint32_t val) {

  val ? (LPC_GPIO(port_num)->FIOSET = (1UL << pin_num)) : \
        (LPC_GPIO(port_num)->FIOCLR = (1UL << pin_num));
}

/**
  \fn          uint32_t  GPIO_PinRead (uint32_t port_num, uint32_t pin_num,)
  \brief       Read port pin
  \param[in]   port_num   GPIO number (0..4)
  \param[in]   pin_num    Port pin number
  \return      pin value (0 or 1)
*/
uint32_t GPIO_PinRead (uint32_t port_num, uint32_t pin_num) {
  return ((LPC_GPIO(port_num)->FIOPIN & (1UL << pin_num)) ? (1) : (0));
}

/**
  \fn          void GPIO_PortWrite (uint32_t port_num,
                                    uint32_t mask,
                                    uint32_t val)
  \brief       Write port pins
  \param[in]   port_num   GPIO number (0..4)
  \param[in]   mask       Selected pins
  \param[in]   val        Pin values
*/
void GPIO_PortWrite (uint32_t port_num, uint32_t mask, uint32_t val) {
  LPC_GPIO(port_num)->FIOMASK = ~mask;
  LPC_GPIO(port_num)->FIOPIN  =  val;
}

/**
  \fn          uint32_t  GPIO_PortRead (uint32_t port_num)
  \brief       Read port pins
  \param[in]   port_num   GPIO number (0..4)
  \return      port pin inputs
*/
uint32_t GPIO_PortRead (uint32_t port_num) {
  return (LPC_GPIO(port_num)->FIOPIN);
}
