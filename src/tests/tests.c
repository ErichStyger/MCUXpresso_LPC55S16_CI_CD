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

#define USE_TEST_ARGUMENTS  (1)  /* if using test arguments */

#if USE_TEST_ARGUMENTS
static void TestArgFailed(void) {
  TEST_ASSERT_MESSAGE(false, "wrong test_arg value, check JLinkScript file");
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
    extern int McuUnity_UART_GetArgs(unsigned char *buffer, size_t bufSize, McuShell_ConstStdIOTypePtr io);
    nofBytes = McuUnity_UART_GetArgs(buf, sizeof(buf), McuShellUart_GetStdio());
    McuLog_info("uart nof: %d, buf: %s", nofBytes, buf);
    if (nofBytes>0) {
      if (buf[0]=='1') {
        test_arg = 1;
      } else if (buf[0]=='2') {
        test_arg = 2;
      }
    } else {
      test_arg = 1; /*! \TODO */
    }
  #else
    nofBytes = McuUnity_Semihost_GetArgs(buf, sizeof(buf));
    McuLog_info("semihost nof: %d, buf: %s", nofBytes, buf);
    if (nofBytes>0) {
      if (buf[0]=='1') {
        test_arg = 1;
      } else if (buf[0]=='2') {
        test_arg = 2;
      }
    } else {
      test_arg = 1; /*! \TODO */
    }
  #endif
#elif 1 && USE_TEST_ARGUMENTS /* new JRun */  
  nofBytes = McuUnity_RTT_GetArgs(buf, sizeof(buf));
  SEGGER_RTT_printf(0, "RTT args = %s, nofBytes = %d\n", buf, nofBytes);
  if (nofBytes>0) {
     if (McuUtility_strcmp((char*)buf, "1")==0) {
      test_arg = 1;
     } else if (McuUtility_strcmp((char*)buf, "2")==0) {
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
  McuLog_info("*STOP*"); /* stop test runner */
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