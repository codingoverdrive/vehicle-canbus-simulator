/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
*/

#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/cmdargs.h"

// print the command line options
void printCommandLineOptions(char *progName) {
  fprintf(stderr, "\nUsage: %s <options> logfile\n\n", progName);
  fprintf(stderr, "Options:      -c <canbus>  "
    "(default is can0)\n");
  fprintf(stderr, "              -i           "
   	"(infinite or loop forever)\n");
	fprintf(stderr, "              -l <num>     "
		"(process input file <num> times, default=1)\n");
  fprintf(stderr, "              -m <ms>      "
    "(skip delay in timestamps > <ms> milliseconds)\n");
  fprintf(stderr, "              -t           "
    "(ignore timestamps and send frames with no delay)\n");
  fprintf(stderr, "              -v           "
   	"(verbose, display sent messages)\n");
  fprintf(stderr, "              -x <0/1>     "
   	"(enable loopback, (0 = off default, 1 = on)\n");
  fprintf(stderr, "\n");
}

// convert the command line args into options
int processCommandLineArgs(int argc, char *argv[], Options *options) {
  // initial values
  strcpy(options->ifname, "can0");
  options->verbose; // show frames as they are sent
  options->infiniteLoops = 0; // loop forever
  options->loops = 0;         // specify a set number of loops
  options->ignoreTimestamp = 0; // ignore time between frames, send without delay
  options->maxDelayMS = 0; // the maximum delay allowed between frames

  static int opt = 0;
  while ((opt = getopt(argc, argv, "ivtm:l:c:x:")) != -1) {
    switch (opt) {
    case 'c':
      strcpy(options->ifname, optarg);
      break;
    case 'i':
      options->infiniteLoops = 1;
      break;
    case 'l':
      if (!(options->loops = atoi(optarg))) {
					fprintf(stderr, "Invalid numeric value for option -l !\n");
					return 1;
			}
      break;
    case 'v':
      options->verbose = 1;
      break;
    case 'm':
      if (!(options->maxDelayMS = atoi(optarg))) {
					fprintf(stderr, "Invalid numeric value for option -s !\n");
					return 1;
			}
      break;
    case 't':
      options->ignoreTimestamp = 1;
      break;
    case 'x':
      options->enableLoopback = atoi(optarg);
      if (options->enableLoopback < 0 || options->enableLoopback > 1) {
					fprintf(stderr, "Invalid 0 or 1 value for option -x !\n");
					return 1;
			}
      break;
    default:
      printCommandLineOptions(basename(argv[0]));
      return 1;
      break;
    }    
  }

  return 0;
}
