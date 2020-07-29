#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "canlog.h"

// ASC (Vector) Log format
//  0.000316 1  3C8        Rx D 8  01  00  00  00  00  00  00  00
#define ASC_TEMPLATE "%ld.%d %*d %hx %*s %*s %hd %99[0-9A-Fa-f ]"

// Parses a single line from an ASC (Vector) CANBUS log file
// resulting in a CanMessage object.
// Warning this function does not validate the line before
// processing, so avoid passing in lines (like comments)
// that will fail too match the correct pattern
int asc2CanMessage(char *str, int len, CanMessage *msg) {
  memset(msg, 0, sizeof(CanMessage));

  // parse line
  char dataBytes[100];
  int matches = sscanf(str, ASC_TEMPLATE, &msg->time.tv_sec, &msg->time.tv_usec, &msg->mid,
         &msg->len, dataBytes);
  // printf("%s\n", dataBytes);
  // printf("matches %d\n", matches);
  if (matches < 5)
    return 1;

  // parse each data byte (extracted from the string dataBytes)
  char hex[3] = {0, 0, 0};
  int d;
  for (int i = 0; i < msg->len; i++) {
    memcpy(hex, &dataBytes[4 * i], 2);
    sscanf(hex, "%x", &d);
    // printf("%d: %s %02X\n", i, hex, d);
    msg->data[i] = d;
  }

  return 0;
}

// Converts a CanMessage into a CAN Frame for transmission
// over the CANBUS
void canMessage2Frame(CanMessage *msg, CanFrame *frame) {
  memset(frame, 0, sizeof(CanFrame));
  frame->id = msg->mid;
  frame->dlc = msg->len;
  for (int i = 0; i < msg->len; i++)
    frame->data[i] = msg->data[i];
}

static void debugArrayHexBytes(char *prefixStr, uint8_t *bytes, int len) {
  if (prefixStr)
    printf("%s ", prefixStr);
  for (size_t i = 0; i < len; i++)
    printf("%02X ", bytes[i]);
  printf("\n");
}

void debugCanFrame(CanFrame *canFrame) {
  printf("%03X : %d : ", canFrame->id, canFrame->dlc);
  debugArrayHexBytes(NULL, canFrame->data, canFrame->dlc);
}