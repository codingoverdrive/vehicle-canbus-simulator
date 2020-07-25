#ifndef _canmessage

#include <stdint.h>
#include <sys/time.h>

// Internal representation of a Canbus message
// supports (upto) 8 byte messages
typedef struct _canmessage {
  struct timeval time;
  uint16_t mid;
  uint16_t len;
  uint8_t data[8];
} CanMessage;

#endif

void asc2CanMessage(char *str, int len, CanMessage *msg);
