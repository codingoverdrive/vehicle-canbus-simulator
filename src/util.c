#include <stdlib.h>

// calculates the time delta between two times normalised to milliseconds
// the delta can be positive or negative
unsigned long deltaTimeMs(struct timeval recentTime, struct timeval olderTime) {
  return 1000 * (recentTime.tv_sec - olderTime.tv_sec) +
         (recentTime.tv_usec - olderTime.tv_usec) / 1000;
}