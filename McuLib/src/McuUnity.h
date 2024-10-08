/*
 * Copyright (c) 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCU_UNITY_H_
#define MCU_UNITY_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void McuUnity_putc(int c);
void McuUnity_flush(void);
void McuUnity_start(void);
void McuUnity_complete(void);

/*!
  \brief Get memory mapped argument number.
*/
uint32_t McuUnity_GetArgument(void);


//#include "McuShell.h"
/*!
  \brief Get UART arguments passed
  \param buffer Pointer to memory where to store the argument string
  \param bufSize Size of buffer in bytes
  \param io Standard I/O descriptor
  \return Number of bytes in buffer
*/
//int McuUnity_UART_GetArgs(unsigned char *buffer, size_t bufSize, McuShell_ConstStdIOTypePtr io);

/*!
  \brief Get J-Run arguments passed with RTT and --args
  \param buffer Pointer to memory where to store the argument string
  \param bufSize Size of buffer in bytes
  \return Number of bytes in buffer
*/
int McuUnity_RTT_GetArgs(unsigned char *buffer, size_t bufSize);

/*!
  \brief Get Semihosting arguments passed to application
  \param buffer Pointer to memory where to store the argument string
  \param bufSize Size of buffer in bytes
  \return Number of bytes in buffer
*/
int McuUnity_Semihost_GetArgs(unsigned char *buffer, size_t bufSize);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCU_UNITY_H_ */
