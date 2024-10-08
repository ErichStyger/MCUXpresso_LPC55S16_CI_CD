/*
 * Copyright (c) 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform.h"
#if PL_CONFIG_USE_UNIT_TESTS
#include "tests.h"
#include "unity.h"
#include "McuUnity.h"
#include "McuRTOS.h"
#include "McuShell.h"
#include "McuShellUart.h"
#include "McuSemihost.h"
#include "McuRTT.h"
#include "McuUnity.h"
#include "McuUtility.h"
#include "McuLog.h"
#include "test_leds.h"
#include "shell.h"

#define USE_TEST_ARGUMENTS  (1)  /* if using test arguments */

#if USE_TEST_ARGUMENTS
static void TestArgFailed(void) {
  TEST_ASSERT_MESSAGE(false, "wrong test arg value");
}
#endif

static void TestTask(void *pv) {
  int nofFailures;
#if USE_TEST_ARGUMENTS
  uint32_t test_arg = -1;
  int nofBytes;
  unsigned char buf[32];
#endif

  McuLog_info("starting test task");
#if USE_TEST_ARGUMENTS && McuSemihost_CONFIG_DEBUG_CONNECTION==McuSemihost_DEBUG_CONNECTION_LINKSERVER
  #if PL_CONFIG_USE_SHELL_UART
    #if PL_CONFIG_USE_EXPERIMENTAL

    vTaskDelay(pdMS_TO_TICKS(3000)); /* give UART time to receive */

    McuLog_info("Shell suspend");
    SHELL_Suspend();

    McuShell_printfIO(McuShellUart_GetStdio(), "*ARGS*\r\n");
    vTaskDelay(pdMS_TO_TICKS(500)); /* give UART time to receive */
    #if 0
    for(int i=0; i<20; i++) {
      McuShell_printfIO(McuShellUart_GetStdio(), ".");
      vTaskDelay(pdMS_TO_TICKS(100));
    }
    McuShell_printfIO(McuShellUart_GetStdio(), "\r\n");
    #endif
    extern int McuUnity_UART_GetArgs(unsigned char *buffer, size_t bufSize, McuShell_ConstStdIOTypePtr io); /* \todo */
    nofBytes = McuUnity_UART_GetArgs(buf, sizeof(buf), McuShellUart_GetStdio());

    McuLog_info("Shell to be resumed");
    SHELL_Resume();

    #endif
    McuLog_info("uart nof: %d, buf: %s", nofBytes, buf);
    if (nofBytes>0) {
      if (McuUtility_strcmp((char*)buf, "led1")==0) {
        test_arg = 1;
      } else if (McuUtility_strcmp((char*)buf, "led2")==0) {
        test_arg = 2;
      }
    } else {
      test_arg = 1; /*! \TODO */
    }
  #else
    #if PL_CONFIG_USE_EXPERIMENTAL
    McuSemihost_printf("*ARGS*"); /* somehow, this will block the following semihosting SYS_READC? */
    #endif
    nofBytes = McuUnity_Semihost_GetArgs(buf, sizeof(buf));
    McuLog_info("semihost nof: %d, buf: %s", nofBytes, buf);
    if (nofBytes>0) {
      if (McuUtility_strcmp((char*)buf, "led1")==0) {
        test_arg = 1;
      } else if (McuUtility_strcmp((char*)buf, "led2")==0) {
        test_arg = 2;
      }
    } else {
      test_arg = 1; /*! \TODO */
    }
  #endif
#elif PL_CONFIG_USE_RTT && USE_TEST_ARGUMENTS /* new JRun */  
  nofBytes = McuUnity_RTT_GetArgs(buf, sizeof(buf));
  SEGGER_RTT_printf(0, "RTT args = %s, nofBytes = %d\n", buf, nofBytes);
  if (nofBytes>0) {
     if (McuUtility_strcmp((char*)buf, "led1")==0) {
      test_arg = 1;
     } else if (McuUtility_strcmp((char*)buf, "led2")==0) {
      test_arg = 2;
     }
  } else {
    test_arg = -1;
  }
#elif USE_TEST_ARGUMENTS
  test_arg = McuUnity_GetArgument(); /* get test arguments */
#endif

#if USE_TEST_ARGUMENTS
  McuLog_info("Test arg: %d", test_arg);
#endif
  UNITY_BEGIN();
  #if USE_TEST_ARGUMENTS
    switch(test_arg) {
      case 1:   RUN_TEST(TestLeds_OnOff); break;
      case 2:   RUN_TEST(TestLeds_Toggle); break;
      default:  RUN_TEST(TestArgFailed); break;
    }
  #else
    RUN_TEST(TestLeds_OnOff);
    RUN_TEST(TestLeds_Toggle);
  #endif
  nofFailures = UNITY_END();
  /* report failed or pass */
  if (nofFailures==0) {
    McuLog_info("*** PASSED ***");
  } else {
    McuLog_error("*** FAILED ***");
  }
#if PL_CONFIG_USE_RTT
  /* Note: \r\n does not work, need to use \n in v8.10! */
  if (nofFailures==0) {
    McuLog_info("Success: sending stop with 0");
    McuLog_info("*STOP*0\n"); /* stop test runner with exit code 0 (ok) */
  } else {
    McuLog_info("Failed: sending stop with 1");
    McuLog_info("*STOP*1\n"); /* stop test runner with exit code 1 (failed), negative error codes are J-Run error codes */
  }
#else /* LinkServer */
  McuLog_info("*STOP*"); /* stop test runner */
#endif
  vTaskDelete(NULL); /* terminate task */
}

void Tests_Init(void) {
  if (xTaskCreate(
      TestTask,  /* pointer to the task */
      "Test", /* task name for kernel awareness debugging */
      1500/sizeof(StackType_t), /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY,  /* initial priority */
      (TaskHandle_t*)NULL /* optional task handle to create */
    ) != pdPASS)
  {
    McuLog_fatal("Failed creating task");
    for(;;){} /* error! probably out of memory */
  }
}
#endif /* PL_CONFIG_USE_UNIT_TESTS */