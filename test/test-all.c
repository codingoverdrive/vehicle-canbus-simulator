/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
 */

#include <stdio.h>
#include "test_canlog.h"
#include "test_utils.h"
#include "test_simulation.h"

int main(int argc, char **argv) {
  // canlog tests
  char *result = all_canlog_tests();
  if (result != 0) {
    printf("%s\n", result);
    return result != 0;
  }

  result = all_util_tests();
  if (result != 0) {
    printf("%s\n", result);
    return result != 0;
  }

  result = all_simulation_tests();
  if (result != 0) {
    printf("%s\n", result);
    return result != 0;
  }
  
  printf("ALL TESTS PASSED\n");

  return result != 0;
}