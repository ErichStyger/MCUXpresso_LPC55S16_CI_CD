/*
 * Copyright (c) 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "leds_config.h"
#include "leds.h"
#include "McuLED.h"

static McuLED_Handle_t ledRed, ledGreen, ledBlue;

void Leds_On(LEDS_Leds_e led) {
  switch(led) {
    case LEDS_RED:        McuLED_On(ledRed); break;
    case LEDS_GREEN:      McuLED_On(ledGreen); break;
    case LEDS_BLUE:       McuLED_On(ledBlue); break;
    default:
      break; /* error */
  }
}

void Leds_Off(LEDS_Leds_e led) {
  switch(led) {
    case LEDS_RED:        McuLED_Off(ledRed); break;
    case LEDS_GREEN:      McuLED_Off(ledGreen); break;
    case LEDS_BLUE:       McuLED_Off(ledBlue); break;
    default:
      break; /* error */
  }
}

void Leds_Neg(LEDS_Leds_e led) {
  switch(led) {
    case LEDS_RED:        McuLED_Toggle(ledRed); break;
    case LEDS_GREEN:      McuLED_Toggle(ledGreen); break;
    case LEDS_BLUE:       McuLED_Toggle(ledBlue); break;
    default:
      break; /* error */
  }
}

bool Leds_Get(LEDS_Leds_e led) {
  switch(led) {
    case LEDS_RED:        return McuLED_Get(ledRed);
    case LEDS_GREEN:      return McuLED_Get(ledGreen);
    case LEDS_BLUE:       return McuLED_Get(ledBlue);
    default:
      break; /* error */
  }
  return false;
}

#include "fsl_iocon.h"
#include "pin_mux.h"

void Leds_Init(void) {
  McuLED_Config_t config;

  McuLED_GetDefaultConfig(&config);
  config.isOnInit = false;
  config.isLowActive = false;
  config.hw.gpio = LEDS_LED_RED_GPIO;
  config.hw.port = LEDS_LED_RED_PORT;
  config.hw.pin = LEDS_LED_RED_PIN;
  ledRed = McuLED_InitLed(&config);
  if (ledRed==NULL) {
    for(;;) {}
  }

  config.isOnInit = false;
  config.isLowActive = false;
  config.hw.gpio = LEDS_LED_GREEN_GPIO;
  config.hw.port = LEDS_LED_GREEN_PORT;
  config.hw.pin = LEDS_LED_GREEN_PIN;
  ledGreen = McuLED_InitLed(&config);
  if (ledGreen==NULL) {
    for(;;) {}
  }

  config.isOnInit = false;
  config.isLowActive = false;
  config.hw.gpio = LEDS_LED_BLUE_GPIO;
  config.hw.port = LEDS_LED_BLUE_PORT;
  config.hw.pin = LEDS_LED_BLUE_PIN;
  ledBlue = McuLED_InitLed(&config);
  if (ledBlue==NULL) {
    for(;;) {}
  }
}

void LED_Deinit(void) {
  ledRed = McuLED_DeinitLed(ledRed);
  ledGreen = McuLED_DeinitLed(ledGreen);
  ledBlue = McuLED_DeinitLed(ledBlue);
}