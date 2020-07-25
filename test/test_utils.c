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

char *all_util_tests() {
  tests_run = 0;

  mu_run_test(test_time_delta);

  printf("Tests run: %s -> %d\n", "util", tests_run);

  return 0;
}