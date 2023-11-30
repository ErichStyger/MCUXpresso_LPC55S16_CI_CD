/*
 * Copyright (c) 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform.h"
#if PL_CONFIG_USE_UNIT_TESTS
#include "tests.h"
#include "unity.h"
#include "McuRTOS.h"
#include "McuShell.h"
#include "McuRTT.h"
#include "McuLog.h"
#include "test_sensor.h"
#include "test_arg.h"

static void TestArgFailed(void) {
  TEST_ASSERT_MESSAGE(false, "wrong test_arg value, check JLinkScript");
}

static void TestTask(void *pv) {
  int nofFailures;
  uint32_t test_arg;

  McuLog_info("starting test task");
  test_arg = TestArg_GetArgument();
  vTaskDelay(pdMS_TO_TICKS(100)); /* give sensor reading some time */
  UNITY_BEGIN();
  switch(test_arg) {
    case 1: RUN_TEST(TestTemperature); break;
    case 2: RUN_TEST(TestHumidity);    break;
    case 3: RUN_TEST(TestSensor);      break;
    default:
      RUN_TEST(TestArgFailed); break;
  }
  nofFailures = UNITY_END();

  if (nofFailures==0) {
    McuLog_info("*** PASSED ***\n");
    McuShell_SendStr("*** PASSED ***\n", McuRTT_stdio.stdOut);
  } else {
    McuLog_info("*** FAILED ***\n");
    McuShell_SendStr("*** FAILED ***\n", McuRTT_stdio.stdOut);
  }
  McuShell_SendStr("*STOP*\n", McuRTT_stdio.stdOut); /* stop JRun */
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