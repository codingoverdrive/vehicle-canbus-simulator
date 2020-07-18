/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
 */

#include <stdint.h>

int byteCompare_uint8(uint8_t *this, uint8_t *other, int len);
void debugBytes(char *msg, uint8_t *bytes, int len);

#ifndef testArrayEquality
#define testArrayEquality(failureMsg, bytes, expected, len)                    \
  if (!byteCompare_uint8(bytes, expected, len)) {                              \
    debugBytes("Expected ", expected, len);                                    \
    debugBytes("Got      ", bytes, len);                                       \
    mu_assert(failureMsg, 0);                                                  \
  }
#endif