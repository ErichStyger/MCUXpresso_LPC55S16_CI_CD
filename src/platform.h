/*
 * Copyright (c) 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#define PL_CONFIG_USE_SHELL               (1)
#define PL_CONFIG_USE_SHELL_UART          (1)

//#define PL_CONFIG_USE_UNIT_TESTS        (1 && defined(ENABLE_UNIT_TESTS) && ENABLE_UNIT_TESTS==1) /* if using unit tests. ENABLE_UNIT_TESTS is set by the CMake file */
#define PL_CONFIG_USE_UNIT_TESTS       1
#if PL_CONFIG_USE_UNIT_TESTS && !defined(UNITY_OUTPUT_CHAR)
   #error "needs to be defined in IncludeMcuLibConfig.h!"
#endif

//#define PL_CONFIG_USE_RTT               (1 || PL_CONFIG_USE_UNIT_TESTS) /* if using SEGGER RTT, needed for unit testing */
#define PL_CONFIG_USE_RTT (0)

/*!
 * \brief Module initialization
 */
void PL_Init(void);

#endif /* PLATFORM_H_ */
