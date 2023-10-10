/*
 * Copyright (c) 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "McuUnity.h"
#include "McuRTT.h"

void McuUnity_putc(char c) {
  McuRTT_StdIOSendChar(c);
}

void McuUnity_flush(void) {
}

void McuUnity_start(void) {
}

void McuUnity_complete(void) {
}