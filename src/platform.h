/*
 * Copyright (c) 2023-2024, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#define PL_CONFIG_USE_SHELL             (1)

#define PL_CONFIG_USE_UNIT_TESTS        (1 && defined(ENABLE_UNIT_TESTS) && ENABLE_UNIT_TESTS==1) /* if using unit tests. ENABLE_UNIT_TESTS is set by the CMake file */

#if PL_CONFIG_USE_UNIT_TESTS && !defined(UNITY_OUTPUT_CHAR)
   #error "Unity hooks need to be defined in IncludeMcuLibConfig.h!"
#endif

/* enable on or the other runner: update the tests/CMakeLists.txt as well! */
#define PL_CONFIG_USE_RUNNER_JLINK        (0)
#define PL_CONFIG_USE_RUNNER_LINKSERVER   (1)

#define PL_CONFIG_USE_RTT                 (1 && PL_CONFIG_USE_RUNNER_JLINK)        /* J-Link only: using J-Run and SEGGER RTT for log and test output */
#define PL_CONFIG_USE_SEMIHOSTING         (0 && PL_CONFIG_USE_RUNNER_LINKSERVER)   /* LinkServer only: using semihosting for log and test output */
#define PL_CONFIG_USE_SHELL_UART          (1 && PL_CONFIG_USE_RUNNER_LINKSERVER)   /* LinkServer only: using UART for log and test output */

#define PL_CONFIG_USE_EXPERIMENTAL        (1 && PL_CONFIG_USE_RUNNER_LINKSERVER)    /* experimental feature with arguments */

#if PL_CONFIG_USE_RUNNER_LINKSERVER && PL_CONFIG_USE_SEMIHOSTING && McuSemihost_CONFIG_DEBUG_CONNECTION!=McuSemihost_DEBUG_CONNECTION_LINKSERVER
  #error "check LinkServer settings in IncludeMcuLibConfig.h"
#elif PL_CONFIG_USE_RUNNER_JLINK && PL_CONFIG_USE_SEMIHOSTING && McuSemihost_CONFIG_DEBUG_CONNECTION!=McuSemihost_DEBUG_CONNECTION_SEGGER
  #error "check SEGGER settings in IncludeMcuLibConfig.h"
#endif

/*!
 * \brief Module initialization
 */
void PL_Init(void);

#endif /* PLATFORM_H_ */
