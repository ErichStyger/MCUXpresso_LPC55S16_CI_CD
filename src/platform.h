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

#define PL_CONFIG_USE_RTT           (1)   /* using J-Run and SEGGER RTT for log and test output */
#define PL_CONFIG_USE_SEMIHOSTING   (0)   /* using semihosting for log and test output */
#define PL_CONFIG_USE_SHELL_UART    (0)   /* using UART for log and test output */

/*!
 * \brief Module initialization
 */
void PL_Init(void);

#endif /* PLATFORM_H_ */
