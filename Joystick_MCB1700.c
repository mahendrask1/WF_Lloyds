/*-----------------------------------------------------------------------------
 * Name:    Joystick_MCB1700.c
 * Purpose: Joystick interface for MCB1700 evaluation board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2013 - 2014 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include "LPC17xx.h"

#include "PIN_LPC17xx.h"
#include "GPIO_LPC17xx.h"

#include "Board_Joystick.h"

#define JOYSTICK_COUNT                 (5)

/* Joystick pins:
   - center: P1_20 = GPIO1[20]
   - up:     P1_23 = GPIO1[23]
   - down:   P1_25 = GPIO1[25]
   - left:   P1_26 = GPIO1[26]
   - right:  P1_24 = GPIO1[24] */


/* Joystick pin definitions */
const PIN JOYSTICK_PIN[] = {
  { 1, 20},
  { 1, 23},
  { 1, 25},
  { 1, 26},
  { 1, 24}
};


/**
  \fn          int32_t Joystick_Initialize (void)
  \brief       Initialize joystick
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Joystick_Initialize (void) {
  uint32_t n;

  /* Enable GPIO clock */
  GPIO_PortClock     (1);

  /* Configure pins */
  for (n = 0; n < JOYSTICK_COUNT; n++) {
    PIN_Configure (JOYSTICK_PIN[n].Portnum, JOYSTICK_PIN[n].Pinnum, PIN_FUNC_0, 0, 0);
    GPIO_SetDir   (JOYSTICK_PIN[n].Portnum, JOYSTICK_PIN[n].Pinnum, GPIO_DIR_INPUT);
  }
  return 0;
}

/**
  \fn          int32_t Joystick_Uninitialize (void)
  \brief       De-initialize joystick
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Joystick_Uninitialize (void) {
  uint32_t n;

  /* Unconfigure pins */
  for (n = 0; n < JOYSTICK_COUNT; n++) {
    PIN_Configure (JOYSTICK_PIN[n].Portnum, JOYSTICK_PIN[n].Pinnum, 0, 0, 0);
  }
  return 0;
}

/**
  \fn          uint32_t Joystick_GetState (void)
  \brief       Get joystick state
  \returns     Joystick state
*/
uint32_t Joystick_GetState (void) {
  uint32_t val;

  val  = 0;
  if (!(GPIO_PinRead (JOYSTICK_PIN[0].Portnum, JOYSTICK_PIN[0].Pinnum))) val |= JOYSTICK_CENTER;
  if (!(GPIO_PinRead (JOYSTICK_PIN[1].Portnum, JOYSTICK_PIN[1].Pinnum))) val |= JOYSTICK_UP;
  if (!(GPIO_PinRead (JOYSTICK_PIN[2].Portnum, JOYSTICK_PIN[2].Pinnum))) val |= JOYSTICK_DOWN;
  if (!(GPIO_PinRead (JOYSTICK_PIN[3].Portnum, JOYSTICK_PIN[3].Pinnum))) val |= JOYSTICK_LEFT;
  if (!(GPIO_PinRead (JOYSTICK_PIN[4].Portnum, JOYSTICK_PIN[4].Pinnum))) val |= JOYSTICK_RIGHT;

  return val;
}
