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
#include "McuLog.h"
#include "test_leds.h"

#define USE_TEST_ARGUMENTS  (0)

static void TestArgFailed(void) {
  TEST_ASSERT_MESSAGE(false, "wrong test_arg value, check JLinkScript file");
}

static void TestTask(void *pv) {
  int nofFailures;

  McuLog_info("starting test task");
  UNITY_BEGIN();
  #if USE_TEST_ARGUMENTS
    uint32_t test_arg;
    test_arg = McuUnity_GetArgument(); /* get test arguments */
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
    McuShell_SendStr((unsigned char*)"*** PASSED ***\n", McuRTT_stdio.stdOut);
  } else {
    McuShell_SendStr((unsigned char*)"*** FAILED ***\n", McuRTT_stdio.stdOut);
  }
  McuShell_SendStr((unsigned char*)"*STOP*\n", McuRTT_stdio.stdOut); /* stop JRun */
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