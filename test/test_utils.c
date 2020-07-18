/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
 */

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include "minunit.h"
#include "util.h"

int tests_run;

char *test_time_delta() {
  struct timeval firstTime, secondTime;
  gettimeofday(&firstTime, NULL);
  // printf("F> %ld.%ld\n", firstTime.tv_sec, firstTime.tv_usec);
  usleep(2000000); // 2 secs or 2M microseconds
  gettimeofday(&secondTime, NULL);
  // printf("S> %ld.%ld\n", secondTime.tv_sec, secondTime.tv_usec);
  unsigned long delta = deltaTimeMs(secondTime, firstTime);
  // printf("Delta %d\n", delta);
  mu_assert("Error, time delta should be 2s", delta >= 1999 && delta <= 2001);

  return 0;
}

char *test_fixed_deltas() {
  struct timeval firstTime, secondTime;

  firstTime.tv_sec = 1234567890;
  firstTime.tv_usec = 999999;
  secondTime.tv_sec = 1234567890;
  secondTime.tv_usec = 888888;
  unsigned long delta = deltaTimeMs(secondTime, firstTime);
  // printf("Delta %d\n", delta);
  mu_assert("Error, time delta should be -111ms", delta >= -112 && delta <= -110);

  firstTime.tv_sec = 1234567890;
  firstTime.tv_usec = 999999;
  secondTime.tv_sec = 1234567891;
  secondTime.tv_usec = 1;
  delta = deltaTimeMs(secondTime, firstTime);
  // printf("Delta %d\n", delta);
  mu_assert("Error, time delta should be 2ms", delta >= 1 && delta <= 3);

  return 0;
}

char *all_util_tests() {
  tests_run = 0;

  mu_run_test(test_time_delta);
  mu_run_test(test_fixed_deltas);

  printf("Tests run: %s -> %d\n", "all_util_tests", tests_run);

  return 0;
}