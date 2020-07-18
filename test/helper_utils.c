/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
 */

#include <stdio.h>
#include "helper_utils.h"

int byteCompare_uint8(uint8_t *this, uint8_t *other, int len) {
  for (int i = 0; i < len; i++)
    if ((uint8_t)this[i] != (uint8_t)other[i])
      return 0;

  return 1;
}

void debugBytes(char *msg, uint8_t *bytes, int len) {
  if (msg)
    printf("%s ", msg);
  for (int i = 0; i < len; i++)
    printf("%02X ", bytes[i]);
  printf("\n");
}

