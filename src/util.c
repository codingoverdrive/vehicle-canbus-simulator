/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
 */

#include "util.h"

// calculates the time delta between two times normalised to milliseconds
// the delta can be positive or negative
// has a margin of error of 1ms due to truncation of the usec -> ms calculation
unsigned long deltaTimeMs(struct timeval recentTime, struct timeval olderTime) {
  return 1000 * (recentTime.tv_sec - olderTime.tv_sec) +
         (recentTime.tv_usec - olderTime.tv_usec) / 1000;
}