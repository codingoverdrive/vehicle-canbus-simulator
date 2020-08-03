/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "include/util.h"
#include "include/cmdargs.h"
#include "include/cansocket.h"
#include "include/simulation.h"

#define VERSION "v1.0.1"
#define DEFAULT_LOOPS 1

int main(int argc, char *argv[]) {
  fprintf(stderr, "\nVehicle Canbus Simulator %s\n", VERSION);

  if (argc <= 1) {
    printCommandLineOptions(argv[0]);
    return 1;
  }

  // process command line options
  static Options options;
  if (processCommandLineArgs(argc, argv, &options) == 1) {
    return 1;
  }

  // ensure that we loop correctly
  if (options.loops > 0)
    options.infiniteLoops = 0;
  else if (options.loops < DEFAULT_LOOPS)
    options.loops = DEFAULT_LOOPS;

  // log file should be last argument on command line
  char filepath[128];
  // fprintf(stderr, "%d %s\n", optind, argv[optind]);
  if (argv[optind] == NULL) {
    fprintf(stderr, "No long file specified\n");
    printCommandLineOptions(basename(argv[0]));
    return 1;
  }

  // open the log file
  strcpy(filepath, argv[optind]);
  FILE *logFP = fopen(filepath, "r");
  if (logFP == NULL) {
    fprintf(stderr, "Failed to open specified log file %s\n\n", filepath);
    return 1;
  }
  if (options.verbose)
    printf("Log file:  %s\n", filepath);

  //open the CANBUS socket
  int canSocket;
  if (openCanSocket(&canSocket, options.enableLoopback) > 0)
    return 1;
  // printf("canSocket %d\n",canSocket);
  struct sockaddr_can canAddr;
  if (options.verbose)
    printf("CANBUS:    %s\n\n", options.ifname);
  if (bindSocket(canSocket, options.ifname, &canAddr) > 0) 
      return 1;

  runSimulation(logFP, canSocket, &options, NULL);

  closeSocket(canSocket);
  fclose(logFP);

  fprintf(stderr, "\nStopping\n\n");

  return 0;
}