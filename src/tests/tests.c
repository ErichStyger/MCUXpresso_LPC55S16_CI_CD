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
#if PL_CONFIG_USE_GCOV
  #include "McuCoverage.h"  
#endif

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
#if USE_TEST_ARGUMENTS && PL_CONFIG_USE_RUNNER_LINKSERVER
  #if PL_CONFIG_USE_SHELL_UART
    vTaskDelay(pdMS_TO_TICKS(1000)); /* give UART time to receive */
    McuLog_info("Shell suspend");
    SHELL_Suspend();
    McuShell_printfIO(McuShellUart_GetStdio(), "*ARGS*\r\n");
    vTaskDelay(pdMS_TO_TICKS(500)); /* \todo give UART time to receive */
    nofBytes = McuUnity_UART_GetArgs(buf, sizeof(buf), McuShellUart_GetStdio()->stdIn);
    McuLog_info("Shell to be resumed");
    SHELL_Resume();
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
    McuSemihost_printf("*ARGS*");
    nofBytes = McuUnity_Semihost_GetArgs(buf, sizeof(buf)); /* if using debugger here, skip this as no data comming from test runner */
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
#elif USE_TEST_ARGUMENTS && PL_CONFIG_USE_RUNNER_JLINK /* use JRun */  
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
  /* report failed or pass. LinkServer is configured with 
    --pass-mark "*** PASSED ***" --fail-mark "*** FAILED ***" 
   */
  if (nofFailures==0) {
    McuLog_info("*** PASSED ***");
  } else {
    McuLog_error("*** FAILED ***");
  }
#if PL_CONFIG_USE_GCOV
  vTaskSuspendAll(); /* suspend all tasks, so that we can write coverage files uninterrupted */
  McuCoverage_WriteFiles(); /* write coverage data files */
  (void)xTaskResumeAll();
#endif
#if PL_CONFIG_USE_RTT
  McuUnity_Exit_JRun_RTT(nofFailures==0);
#else
  McuUnity_Exit_LinkServer_Log(nofFailures==0);
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