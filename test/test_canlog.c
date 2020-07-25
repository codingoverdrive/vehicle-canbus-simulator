#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "minunit.h"
#include "canlog.h"

int tests_run;

static void debugBytes(char *msg, uint8_t *bytes, int len) {
  if (msg)
    printf("%s ", msg);
  for (size_t i = 0; i < len; i++)
    printf("%02X ", bytes[i]);
  printf("\n");
}

static int byteCompare_uint8(uint8_t *this, uint8_t *other, int len) {
  for (int i = 0; i < len; i++)
    if ((uint8_t)this[i] != (uint8_t)other[i])
      return 0;

  return 1;
}

#define testArrayEquality(failureMsg, bytes, expected, len)                    \
  if (!byteCompare_uint8(bytes, expected, len)) {                              \
    debugBytes("Expected ", expected, len);                                    \
    debugBytes("Got      ", bytes, len);                                       \
    mu_assert(failureMsg, 0);                                                  \
  }

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
  asc2CanMessage(line, strlen(line), &canMsg);
  mu_assert("Error, time.sec should be 416", canMsg.time.tv_sec == 416);
  mu_assert("Error, time.usec should be 62056", canMsg.time.tv_usec == 62056);
  mu_assert("Error, mid should be 0x1FA", canMsg.mid == 0x132);
  mu_assert("Error, len should be 3", canMsg.len == 8);

  uint8_t expectedData[] = {0xCC, 0x96, 0xF4, 0xFF, 0xF9, 0x26, 0xFF, 0x0F};
  testArrayEquality("Error: test_parse_large_asc_msg", canMsg.data,
                    expectedData, 8);
  return 0;
}

char *all_canlog_tests() {
  tests_run = 0;

  mu_run_test(test_parse_small_asc_msg);
  mu_run_test(test_parse_large_asc_msg);

  printf("Tests run: %s -> %d\n", "all_canlog_tests", tests_run);

  return 0;
}