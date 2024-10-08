/*
 * Copyright (c) 2023-2024, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "McuLib.h"
#include "McuRTT.h"
#include "McuLog.h"
#include "McuSemihost.h"
#include "McuShellUart.h"
#if McuLib_CONFIG_CPU_IS_RPxxxx
  #include "pico/platform.h"
#endif

/* The variable below is not initialized during statup, and set by JRun using a test JLinkScript,
 * writing the variable during HandleAfterFlashProg().
 */
#if McuLib_CONFIG_CPU_IS_RPxxxx
  static uint32_t __uninitialized_ram(program_arg);
#else /* put variable into a no-init section */
  uint32_t program_arg __attribute__((section (".uninit_RESERVED")));
#endif

uint32_t McuUnity_GetArgument(void) {
  McuLog_info("program_arg: value 0x%x @0x%x", program_arg, &program_arg);
  return program_arg;
}

int McuUnity_UART_GetArgs(unsigned char *buffer, size_t bufSize, McuShell_ConstStdIOTypePtr io) {
  int nof = 0;

  McuLog_info("getting UART arguments...");
  buffer[0] = '\0';

#if 0
  unsigned char ch;
  for(;;) { /* breaks */
    io->stdIn(&ch);
    if (ch!='\0' && ch!='\n' && nof<bufSize-1) { /* -1 for the zero byte */
      McuLog_trace("c: %c", ch);
      *buffer = ch;
      buffer++;
      nof++;
    } else {
      *buffer = '\0'; /* terminate buffer */
      break;
    }
  }
#else
  int i = McuShellUart_PollChar();
  McuLog_info("i: %d", i);
  if (i!=EOF) {
    *buffer = i;
    nof++;
    buffer++;
    *buffer = '\0'; /* terminate buffer */
    i = McuShellUart_PollChar();
    McuLog_info("i1: %d", i);
  } else {
    *buffer = '\0'; /* terminate buffer */
  }
#endif
  return nof;
}

int McuUnity_Semihost_GetArgs(unsigned char *buffer, size_t bufSize) {
  int c, nof;

  McuLog_info("getting semihosting arguments...");
  buffer[0] = '\0';
#if 0
  McuLog_info("reading first char");
  if (McuSemihost_Read(McuSemihost_STDIN, buffer, 1)!=0) { /* blocking! */
    McuLog_error("Failed reading from stdin!");
  } else {
    McuLog_info("have read: %c", buffer[0]);
  }
#endif
  nof = 0;
  for(;;) { /* breaks */
    c = McuSemihost_SysReadC(); /* blocking! */
    if (c!=EOF && c!='\n' && nof<bufSize-1) { /* -1 for the zero byte */
      //McuLog_trace("c: %d %c", c, c);
      *buffer = c;
      buffer++;
      nof++;
    } else {
      *buffer = '\0'; /* terminate buffer */
      break;
    }
  }
  return nof;
}

int McuUnity_RTT_GetArgs(unsigned char *buffer, size_t bufSize) {
  /* from https://wiki.segger.com/Passing_Command-line_arguments_in_C_for_Embedded_Targets */
  int NumBytes;

  SEGGER_RTT_printf(0, "*ARGS*\n"); /* send special command to J-Run to get the arguments */
  NumBytes = 0;
  do {
    NumBytes += SEGGER_RTT_Read (0, (void*)&buffer[NumBytes], bufSize - NumBytes);
  } while ((NumBytes == 0 || buffer[NumBytes-1] != '\n') && (NumBytes < bufSize));
  return NumBytes;
}

void setUp(void) {
  /* run before each test*/
}

void tearDown(void) { 
  /* run after each test */
}

void McuUnity_putc(int c) {
  McuRTT_StdIOSendChar(c); /* using JRun with RTT */
}

void McuUnity_flush(void) {
  /* used to flush the communication channel */
}

void McuUnity_start(void) {
  /* used to open/start the communicatino channel */
}

void McuUnity_complete(void) {
  /* used to close/stop the communication channel */
}
