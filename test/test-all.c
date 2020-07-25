#include <stdio.h>
#include "test_canlog.h"
#include "test_utils.h"

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
  
  printf("ALL TESTS PASSED\n");

  return result != 0;
}