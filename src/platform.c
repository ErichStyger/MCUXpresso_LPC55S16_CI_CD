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
#if McuRdimon_CONFIG_IS_ENABLED
  #include "rdimon/McuRdimon.h"
#endif
#if PL_CONFIG_USE_GCOV
  #include "McuCoverage.h"
#endif
#include "McuHardFault.h"
#include "McuSemihost.h"

static void SetLoggers(void) {
  int idx = 0;

  if (idx<McuLog_CONFIG_NOF_CONSOLE_LOGGER && PL_CONFIG_USE_RTT) {
    McuLog_set_console(McuRTT_GetStdio(), idx);
    idx++;
  }
  if (idx<McuLog_CONFIG_NOF_CONSOLE_LOGGER && PL_CONFIG_USE_SEMIHOSTING) {
    McuLog_set_console(McuSemihost_GetStdio(), idx);
    idx++;
  }
  if (idx<McuLog_CONFIG_NOF_CONSOLE_LOGGER && PL_CONFIG_USE_SHELL_UART) {
    McuLog_set_console(McuShellUart_GetStdio(), idx);
    idx++;
  }
}

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
  McuGPIO_Init();
  McuLED_Init();
#if PL_CONFIG_USE_SHELL_UART
  McuShellUart_Init();
#endif
  McuShell_Init();
  Leds_Init();
  SHELL_Init();
#if PL_CONFIG_USE_SEMIHOSTING
  McuSemiHost_Init();
#endif
#if McuRdimon_CONFIG_IS_ENABLED
  McuRdimon_Init();
#endif
#if PL_CONFIG_USE_GCOV
  McuCoverage_Init();  /* initialize library */
  //McuCoverage_Check(); /* testing file I/O */
#endif
#if PL_CONFIG_USE_UNIT_TESTS
  Tests_Init();
#endif
  McuLog_Init();
  SetLoggers();
}
