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
#include "McuSemihost.h"
#include "leds.h"

void __assertion_failed(char *_Expr) {
  for(;;) {
    __asm("nop");
  }
}

static void AppTask(void *pv) {
  for(;;) {
    Leds_Neg(LEDS_BLUE);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void APP_Run(void) {
  PL_Init(); /* init modules */

  McuLog_info("Hello from the Log module");
  #if McuSemihost_CONFIG_IS_ENABLED
  #if McuSemihost_CONFIG_DEBUG_CONNECTION==McuSemihost_DEBUG_CONNECTION_LINKSERVER
    McuSemihost_printf("Hello from semihosting with NXP LinkServer!\n");
  #elif McuSemihost_CONFIG_DEBUG_CONNECTION==McuSemihost_DEBUG_CONNECTION_SEGGER
    McuSemihost_printf("Hello from semihosting with SEGGER J-Link!\n");
  #else
    #error
  #endif
#endif

#if PL_CONFIG_USE_UNIT_TESTS
  McuLog_info("Running Unit Tests.");
  /* test stopping the runner */
  #if McuSemihost_CONFIG_DEBUG_CONNECTION==McuSemihost_DEBUG_CONNECTION_LINKSERVER
  //McuSemihost_printf("*STOP*\n"); /* stop LinkServer */
  #endif
#else
  McuSemihost_printf("NOT running tests.\n");
#endif
  //McuSemihost_printf("*** FAILED ***\n");
  //McuSemihost_printf("*STOP*\n"); /* stop runner */

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
  for(;;) { /* should not get here */ }
}
