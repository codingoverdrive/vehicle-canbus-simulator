/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "include/cansocket.h"
#include "include/cmdargs.h"
#include "include/util.h"

// runs the simulation
int runSimulation(FILE * fileP, int socket, Options *options, void (*callback)()) {
  static CanMessage canMsg;
  static CanFrame canFrame;
  int keepRunning = 1;

  struct timeval lastTime, nowTime, lastFrameTime;
  gettimeofday(&lastTime, NULL);

  int loopCounter = 0;
  while (keepRunning && (options->infiniteLoops || options->loops--)) {
    if (options->verbose)
      fprintf(stderr, "--- loop: %d ---\n", ++loopCounter);
    rewind(fileP);

    // reset the frame time for each loop to ensure correct timings
    // especially when they are capped to x ms and shorter than the 
    // timings from the log file
    lastFrameTime.tv_sec = 0;
    lastFrameTime.tv_usec = 0;

    char *line = malloc(128);
    size_t len = 0;
    ssize_t read;

    while (keepRunning && (read = getline(&line, &len, fileP)) != -1) {
      // fprintf(stderr, "%s", line);

      //skip headers or bad data
      if (asc2CanMessage(line, read - 1, &canMsg) == 1)
        continue;

      // delay sending the frame (if necessary or required)
      gettimeofday(&nowTime, NULL);
      if (!options->ignoreTimestamp && lastFrameTime.tv_sec != 0 && lastFrameTime.tv_usec != 0) {
        // printf("LAST> %ld.%ld\n", lastTime.tv_sec, lastTime.tv_usec);
        // printf("NOW> %ld.%ld\n", nowTime.tv_sec, nowTime.tv_usec);
        unsigned long elapsedTime = deltaTimeMs(nowTime, lastTime);
        unsigned long frameDeltaTime = deltaTimeMs(canMsg.time, lastFrameTime);
        long sleepTimeMS = frameDeltaTime - elapsedTime;
        // fprintf(stderr, "%d %d SLEEP %d\n", elapsedTime, frameDeltaTime, sleepTimeMS);
        if (options->maxDelayMS && sleepTimeMS > options->maxDelayMS) {
          sleepTimeMS = options->maxDelayMS;
        }
        if (sleepTimeMS > 0) {
          usleep(1000 * sleepTimeMS);
          gettimeofday(&nowTime, NULL);
        }
      }

      canMessage2Frame(&canMsg, &canFrame);
      if (options->verbose)
        debugCanFrame(&canFrame);
      if (sendCanFrameToSocket(socket, &canFrame) == 1) {
        fprintf(stderr, "\nAborting\n");
        keepRunning = 0;
      };

      if (callback != NULL)
        callback();

      lastFrameTime = canMsg.time;
      lastTime = nowTime;
    }
  }

  return 0;
}

