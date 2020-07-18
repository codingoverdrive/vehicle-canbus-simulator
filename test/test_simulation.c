/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "minunit.h"
#include "canlog.h"
#include "cmdargs.h"
#include "simulation.h"
#include "util.h"
#include "helper_utils.h"

int tests_run;
static int receivedFrameCount = 0;
static CanFrame receivedFrames[100];
static int pipeFD[2];
static Options options;

static CanFrame expectedFrames[] = {
  {0x3A2, 8, {0xAB, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {0x3E8, 8, {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {0x3DA, 8, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {0x264, 6, {0x70, 0x00, 0x00, 0x00, 0xE0, 0x01}},
};

char *compareReceivedToExpectedFrames() {
    for (int i = 0; i < receivedFrameCount; i++) {
      mu_assert("Error, mids don't match expected", receivedFrames[i % 4].id == expectedFrames[i].id);
      mu_assert("Error, dlc don't match expected", receivedFrames[i % 4].dlc == expectedFrames[i].dlc);
      testArrayEquality("Error, data doesn't match", receivedFrames[i % 4].data,
                    expectedFrames[i].data, expectedFrames[i].dlc);
    }
}

// open the log file
FILE *openLogFile(char *filepath) {
  FILE *fp;
  fp = fopen(filepath, "r");
  if (fp == NULL) {
    perror("openLogFile: Failed to open file");
    return NULL;
  }
  return fp;
}

void initialiseOptions(Options *options) {
  options->verbose = 0; // show frames as they are sent
  options->infiniteLoops = 0; // loop forever
  options->loops = 1;         // specify a set number of loops
  options->ignoreTimestamp = 0; // ignore time between frames, send without delay
  options->maxDelayMS = 0; // the maximum delay allowed between frames
}

void initialiseFrameBuffer() {
  for (int i = 0; i < receivedFrameCount; i++) {
    memset(&receivedFrames[i], 0, sizeof(CanFrame));
  }
  receivedFrameCount = 0;
}

// callback used to fetch the last sent CanFrame
void simCallback() {
  CanFrame canframe;

  int nbytes = read(pipeFD[0], &canframe, sizeof(CanFrame));
  receivedFrames[receivedFrameCount++] = canframe;
  // debugCanFrame(&canframe);
}

int runTestSimulation(Options *options) {
    char *logfilename = "./test/test-log.asc";
    FILE *fp = openLogFile(logfilename);
    if (fp == NULL) {
      printf("Failed to open log file %s\n", logfilename);
      return 1;
    }

    // create a pipe to receive the CanFrame
    pipe(pipeFD);

    // runSimulation(fp, pipeFD[0], &options, NULL);
    runSimulation(fp, pipeFD[1], options, simCallback);

    close(pipeFD[0]);
    close(pipeFD[1]);
    fclose(fp);

    return 0;
}

char *test_with_default_options() {
    initialiseFrameBuffer();
    initialiseOptions(&options);
    options.verbose = 0;

    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    if (runTestSimulation(&options) == 1) {
      return "ERROR, runSimulation failed";
    };
    gettimeofday(&endTime, NULL);
    unsigned long elaspedMs = deltaTimeMs(endTime, startTime);
    // printf("Elapsed %d\n", elaspedMs);

    mu_assert("Error, should receive 4 frames", receivedFrameCount == 4);
    mu_assert("Error, elapsed should be around 4s", elaspedMs >= 3900 && elaspedMs <= 4100);

    compareReceivedToExpectedFrames();

    return 0;
}

char *test_with_two_loops() {
    initialiseFrameBuffer();
    initialiseOptions(&options);
    options.verbose = 0;
    options.loops = 2;
    options.maxDelayMS = 50;

    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    if (runTestSimulation(&options) == 1) {
      return "ERROR, runSimulation failed";
    };
    gettimeofday(&endTime, NULL);
    unsigned long elaspedMs = deltaTimeMs(endTime, startTime);
    // printf("Elapsed %d\n", elaspedMs);

    mu_assert("Error, should receive 2 x 4 frames", receivedFrameCount == 8);
    mu_assert("Error, elapsed should be < 1s", elaspedMs >= 300 && elaspedMs <= 500);

    compareReceivedToExpectedFrames();

    return 0;
}

char *all_simulation_tests() {
  tests_run = 0;

  mu_run_test(test_with_default_options);
  mu_run_test(test_with_two_loops);

  printf("Tests run: %s -> %d\n", "all_simulation_tests", tests_run);

  return 0;
}