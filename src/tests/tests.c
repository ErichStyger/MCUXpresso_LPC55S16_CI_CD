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
#endif

  McuLog_info("starting test task");
#if McuSemihost_CONFIG_DEBUG_CONNECTION==McuSemihost_DEBUG_CONNECTION_LINKSERVER
  test_arg = 1;
#else /* J-Link */

#if 1 && USE_TEST_ARGUMENTS /* new JRun */  
  int nofBytes;
  unsigned char buf[32];

  nofBytes = McuUnity_RTT_GetArgs((const char*)buf, sizeof(buf));
  SEGGER_RTT_printf(0, "args = %s, nofBytes = %d\n", buf);
  if (nofBytes>0) {
     if (McuUtility_strcmp((char*)buf, "Led_1")==0) {
      test_arg = 1;
     } else if (McuUtility_strcmp((char*)buf, "Led_2")==0) {
      test_arg = 2;
     }
  } else {
    test_arg = -1;
  }
#elif USE_TEST_ARGUMENTS
  test_arg = McuUnity_GetArgument(); /* get test arguments */
#endif

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