/*
 * Copyright (c) 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform.h"
#include "McuLib.h"

#include "McuWait.h"
#include "McuGPIO.h"
#include "McuLED.h"
#include "McuLog.h"
#include "McuRTOS.h"
#include "McuUtility.h"
#include "McuLog.h"
#include "McuShellUart.h"
#include "McuShell.h"
#include "McuRTT.h"
#include "leds.h"
#include "shell.h"
#if PL_CONFIG_USE_UNIT_TESTS
  #include "tests/tests.h"
#endif
#if PL_CONFIG_USE_RTT
  #include "McuRTT.h"
#endif
#include "McuHardFault.h"

void PL_Init(void) {
  CLOCK_EnableClock(kCLOCK_Iocon); /* ungate clock for IOCON */
  CLOCK_EnableClock(kCLOCK_Gpio0); /* for button on P0_7 */
  GPIO_PortInit(GPIO, 0); /* Initialize GPIO button */
  CLOCK_EnableClock(kCLOCK_Gpio1); /* LEDs and user buttons, plus I2C */
  GPIO_PortInit(GPIO, 1); /* Initialize GPIO for LEDs and User Button */

  McuLib_Init();
  McuHardFault_Init();
  McuRTOS_Init();
  McuWait_Init();
#if PL_CONFIG_USE_RTT
  McuRTT_Init();
#endif
  McuLog_Init();
  McuGPIO_Init();
  McuLED_Init();
#if PL_CONFIG_USE_SHELL_UART
  McuShellUart_Init();
#endif
  McuShell_Init();

  Leds_Init();
  SHELL_Init();
#if PL_CONFIG_USE_UNIT_TESTS
  Tests_Init();
#endif
}
