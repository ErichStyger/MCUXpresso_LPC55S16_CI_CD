/*
 * Copyright (c) 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform.h"
#include "application.h"
#include "McuRTOS.h"
#include "McuRTT.h"
#include "McuLog.h"
#include "McuShell.h"
#include "McuShellUart.h"
#include "McuSemihost.h"
#include "leds.h"
#if PL_CONFIG_USE_GCOV
  #include "McuCoverage.h"
#endif

void __assertion_failed(char *_Expr) {
  for(;;) {
    __asm("nop");
  }
}

static void AppTask(void *pv) {
  int count = 0;
  for(;;) {
    Leds_Neg(LEDS_BLUE);
    vTaskDelay(pdMS_TO_TICKS(1000));
    count++;
  #if PL_CONFIG_USE_GCOV && !PL_CONFIG_USE_UNIT_TESTS /* with unit tests, writing coverage files is done in unit test exit part */
    if (count>5) {
      vTaskEndScheduler();
    }
  #endif
  }
}

void APP_Run(void) {
  PL_Init(); /* init modules */

  McuLog_info("Hello from the Log module");
#if PL_CONFIG_USE_SEMIHOSTING && McuSemihost_CONFIG_IS_ENABLED
  #if McuSemihost_CONFIG_DEBUG_CONNECTION==McuSemihost_DEBUG_CONNECTION_LINKSERVER
    McuSemihost_printf("Hello from semihosting with NXP LinkServer!\n");
  #elif McuSemihost_CONFIG_DEBUG_CONNECTION==McuSemihost_DEBUG_CONNECTION_SEGGER
    McuSemihost_printf("Hello from semihosting with SEGGER J-Link!\n");
  #else
    #error
  #endif
#elif PL_CONFIG_USE_SHELL_UART
  McuShell_printfIO(McuShellUart_GetStdio(), "Hello from the UART!\r\n");
#endif

#if PL_CONFIG_USE_UNIT_TESTS
  McuLog_info("Running Unit Tests.");
  /* tests stopping the runner */
  #if McuSemihost_CONFIG_DEBUG_CONNECTION==McuSemihost_DEBUG_CONNECTION_LINKSERVER
    //McuSemihost_printf("*STOP*\n"); /* stop LinkServer */
  #endif
  #if 0 && PL_CONFIG_USE_RUNNER_JLINK && PL_CONFIG_USE_RTT
    SEGGER_RTT_printf(0, "sending stop:\r\n");
    SEGGER_RTT_printf(0, "*STOP*1\r\n"); /* \r\n does not work, \n works */
  #endif
  //McuSemihost_printf("*** FAILED ***\n");
  //McuSemihost_printf("*STOP*\n"); /* stop runner */
#else
  McuSemihost_printf("NOT running tests.\n");
#endif

  if (xTaskCreate(
      AppTask,  /* pointer to the task */
      "App", /* task name for kernel awareness debugging */
      1000/sizeof(StackType_t), /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY+2,  /* initial priority */
      (TaskHandle_t*)NULL /* optional task handle to create */
    ) != pdPASS) {
     for(;;){} /* error! probably out of memory */
  }
  vTaskStartScheduler();
#if PL_CONFIG_USE_GCOV && !PL_CONFIG_USE_UNIT_TESTS /* with unit tests, writing coverage files is done in unit test exit part */
  McuCoverage_WriteFiles(); /* write coverage data files */
#endif /* PL_CONFIG_USE_GCOV */
  for(;;) {
    /* do not return from main() */
  }
}
