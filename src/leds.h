/*
 * Copyright (c) 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MY_LEDS_H_
#define MY_LEDS_H_

#include <stdbool.h>

typedef enum LEDS_Leds_e {
  LEDS_RED,
  LEDS_GREEN,
  LEDS_BLUE,
} LEDS_Leds_e;

/* turn LED on */
void Leds_On(LEDS_Leds_e led);

/* turn LED off */
void Leds_Off(LEDS_Leds_e led);

/* toggle LED */
void Leds_Neg(LEDS_Leds_e led);

/*!
 * \brief Return the status (on/off) of the LED
 * \param led LED handle
 * \return true if LED is on, false otherwise
 */
bool Leds_Get(LEDS_Leds_e led);

/* call it to initialize the driver */
void Leds_Init(void);

/* use this after the driver is not needed any more */
void Leds_Deinit(void);

#endif /* MY_LEDS_H_ */
