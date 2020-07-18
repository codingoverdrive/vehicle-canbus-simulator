/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
 */

// command line options
typedef struct _options {
  char ifname[32]; // can interface name
  int verbose; // show frames as they are sent
  int infiniteLoops; // loop forever
  int loops;         // specify a set number of loops
  int ignoreTimestamp; // ignore time between frames, send without delay
  int maxDelayMS; // the maximum delay allowed between frames
  int enableLoopback; // enable the loopback
} Options;

void printCommandLineOptions(char *progName);
int processCommandLineArgs(int argc, char *argv[], Options *options);