/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
 */

#include <stdint.h>
#include <sys/time.h>

#ifndef _canmessage

// Internal representation of a Canbus message
// after parsing from log file
// supports (upto) 8 byte messages
typedef struct _canmessage {
  struct timeval time;
  uint16_t mid;
  uint16_t len;
  uint8_t data[8];
} CanMessage;

#endif

#ifndef _canframe

// Representation of a CAN frame that is transmitted
// over the CANBUS
typedef struct _canframe {
  uint32_t id; // 32 bit can ID + EFF/RTR/ERR flags
  uint8_t dlc; // Data length
  uint8_t data[8] __attribute__((aligned(8))); //Data
} CanFrame;

#endif

int asc2CanMessage(char *str, int len, CanMessage *msg);

void canMessage2Frame(CanMessage *msg, CanFrame *frame);

void debugCanFrame(CanFrame *canFrame);
