/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "minunit.h"
#include "canlog.h"
#include "helper_utils.h"

int tests_run;

static char *test_parse_small_asc_msg() {
  char *line = " 10.000704 1  1FA        Rx D 3  1A  C0  01";

  CanMessage canMsg;
  asc2CanMessage(line, strlen(line), &canMsg);
  mu_assert("Error, time.sec should be 10", canMsg.time.tv_sec == 10);
  mu_assert("Error, time.usec should be 704", canMsg.time.tv_usec == 704);
  mu_assert("Error, mid should be 0x1FA", canMsg.mid == 0x1FA);
  mu_assert("Error, len should be 3", canMsg.len == 3);

  uint8_t expectedData[] = {0x1A, 0xC0, 0x01};
  testArrayEquality("Error: test_parse_small_asc_msg", canMsg.data,
                    expectedData, 3);
  return 0;
}

static char *test_parse_large_asc_msg() {
  char *line =
      "416.062056 1  132        Rx D 8  CC  96  F4  FF  F9  26  FF  0F";

  CanMessage canMsg;
  mu_assert("Error, failed to parse", asc2CanMessage(line, strlen(line), &canMsg) == 0);
  mu_assert("Error, time.sec should be 416", canMsg.time.tv_sec == 416);
  mu_assert("Error, time.usec should be 62056", canMsg.time.tv_usec == 62056);
  mu_assert("Error, mid should be 0x1FA", canMsg.mid == 0x132);
  mu_assert("Error, len should be 3", canMsg.len == 8);

  uint8_t expectedData[] = {0xCC, 0x96, 0xF4, 0xFF, 0xF9, 0x26, 0xFF, 0x0F};
  testArrayEquality("Error: test_parse_large_asc_msg", canMsg.data,
                    expectedData, 8);
  return 0;
}

static char *test_message_to_frame() {
  char *line =
      "416.062056 1  132        Rx D 8  CC  96  F4  FF  F9  26  FF  0F";

  CanMessage canMsg;
  mu_assert("Error, failed to parse", asc2CanMessage(line, strlen(line), &canMsg) == 0);
  
  CanFrame canFrame;
  canMessage2Frame(&canMsg, &canFrame);

  mu_assert("Error, mid should be 0x1FA", canFrame.id == 0x132);
  mu_assert("Error, len should be 3", canFrame.dlc == 8);
  uint8_t expectedData[] = {0xCC, 0x96, 0xF4, 0xFF, 0xF9, 0x26, 0xFF, 0x0F};
  testArrayEquality("Error: test_message_to_frame", canFrame.data,
                    expectedData, 8);

  return 0;
}  

static char *test_parse_failures() {
  // char *line = "date Wed Oct 2 12:10:47 pm 2019";

  char *lines[] = {
    "date Wed Oct 2 12:10:47 pm 2019", 
    "base hex timestamps absolute", 
    "Begin Triggerblock Wed Oct 2 12:10:47 pm 2019",
    "",
  };

  CanMessage canMsg;
  for (int i = 0; i < 4; i++) {
    mu_assert("Error, didn't detect parse failure", asc2CanMessage(lines[i], strlen(lines[i]), &canMsg) == 1);
    mu_assert("Error, mid should be 0x000", canMsg.mid == 0x000);
    mu_assert("Error, len should be 0", canMsg.len == 0);
  }

  return 0;
}

char *all_canlog_tests() {
  tests_run = 0;

  mu_run_test(test_parse_small_asc_msg);
  mu_run_test(test_parse_large_asc_msg);
  mu_run_test(test_message_to_frame);
  mu_run_test(test_parse_failures);

  printf("Tests run: %s -> %d\n", "all_canlog_tests", tests_run);

  return 0;
}