#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "../include/canlog.h"
#include "../include/util.h"

#define VERSION "v1.0.1"

void printCommandLineOptions(char *progName) {
  fprintf(stderr, "\nUsage: %s <options> logfile\n\n", progName);
}

int openCanSocket(int *canSocket, int loopback_disable) {
  int sock;
  if ((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("openCanSocket: socket");
    return 1;
  }

  /* disable unneeded default receive filter on this RAW socket */
  setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

  if (loopback_disable) {
    int loopback = 0;
    setsockopt(sock, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback,
               sizeof(loopback));
  }

  *canSocket = sock;

  return 0;
}

int bindSocket(int socket, char *ifName, struct sockaddr_can *addr) {
  struct ifreq ifr;

  strcpy(ifr.ifr_name, ifName);
	if (ioctl(socket, SIOCGIFINDEX, &ifr) < 0) {
		perror("bindSocket: SIOCGIFINDEX");
		return 1;
	}

  addr->can_family = AF_CAN;
  printf("\nifreq, %s -> %d\n\n", ifName, ifr.ifr_ifindex);
	addr->can_ifindex = ifr.ifr_ifindex;

  if (bind(socket, (struct sockaddr *)addr, sizeof(struct sockaddr_can)) < 0) {
    perror("bindSocket: bind");
    return 1;
  }

  return 0;
}

int sendCanFrameToSocket(int socket, CanFrame *canFrame) {
  // long numBytes = sendto(socket, &canFrame, sizeof(CanFrame), 0,
  //     (struct sockaddr*)&addr, sizeof(addr));
  int maxLoops = 100;
  while (maxLoops-- > 0) {
    long numBytes = write(socket, canFrame, sizeof(CanFrame));
    // printf("Bytes sent: %d\n", numBytes);
    if (numBytes < 0 && errno == ENOBUFS) {
      usleep(10000);
    }
    else if (numBytes != sizeof(CanFrame)) {
      perror("Cannot write to CAN socket");
      return 1;
    }
  }

  return 0;
}

void closeSocket(int canSocket) { close(canSocket); }

FILE *openLogFile(char *filepath) {
  FILE *fp;
  fp = fopen(filepath, "r");
  if (fp == NULL) {
    perror("openLogFile: Failed to open file");
    return NULL;
  }

  return fp;
}

int main(int argc, char *argv[]) {
  static CanMessage canMsg;
  static CanFrame canFrame;
  int keepRunning = 1;

  fprintf(stderr, "\nVehicle Canbus Simulator %s\n", VERSION);

  if (argc <= 1) {
    printCommandLineOptions(argv[0]);
    return 1;
  }

  char *filepath = "./logs/sample-log.asc"; // TODO get this from arguments
  // char *filepath = "./logs/jsw.asc"; // TODO get this from arguments
  FILE *logFP = fopen(filepath, "r");
  if (logFP == NULL) {
    perror("Failed to open log file");
    return 1;
  }

  int canSocket;
  int disableLoopback = 1;
  if (openCanSocket(&canSocket, disableLoopback) > 0)
    return 1;
  printf("canSocket %d\n",canSocket);
  struct sockaddr_can canAddr;
  printf("canAddr %d %d\n", sizeof(canAddr), sizeof(struct sockaddr_can));
  //TODO get can interface from arguments
  if (bindSocket(canSocket, "can0", &canAddr) > 0) 
      return 1;

  struct timeval lastTime, nowTime, lastFrameTime;
  gettimeofday(&lastTime, NULL);
  lastFrameTime.tv_sec = 0;
  lastFrameTime.tv_usec = 0;

  int logFrames = 1; //TODO get this from arguments
  int infiniteLoops = 0; // TODO get this from arguments
  int loops = 1;         // TODO get this from arguments
  while (keepRunning && (infiniteLoops || loops--)) {
    rewind(logFP);

    char *line = malloc(128);
    size_t len = 0;
    ssize_t read;

    while (keepRunning && (read = getline(&line, &len, logFP)) != -1) {
      //skip headers or bad data
      if (asc2CanMessage(line, read - 1, &canMsg) == 1)
        continue;

      // fprintf(stderr, "%s", line);

      gettimeofday(&nowTime, NULL);
      if (lastFrameTime.tv_sec != 0 && lastFrameTime.tv_usec != 0) {
        // printf("LAST> %ld.%ld\n", lastTime.tv_sec, lastTime.tv_usec);
        // printf("NOW> %ld.%ld\n", nowTime.tv_sec, nowTime.tv_usec);
        unsigned long elapsedTime = deltaTimeMs(nowTime, lastTime);
        unsigned long frameDeltaTime = deltaTimeMs(canMsg.time, lastFrameTime);
        long sleepTimeMS = frameDeltaTime - elapsedTime;
        // fprintf(stderr, "%d %d SLEEP %d\n", elapsedTime, frameDeltaTime, sleepTimeMS);
        if (sleepTimeMS > 0) {
          usleep(1000 * sleepTimeMS);
          gettimeofday(&nowTime, NULL);
        }
      }

      canMessage2Frame(&canMsg, &canFrame);
      if (logFrames)
        debugCanFrame(&canFrame);
      if (sendCanFrameToSocket(canSocket, &canFrame) == 1) {
        fprintf(stderr, "\nAborting\n");
        keepRunning = 0;
      };

      lastFrameTime = canMsg.time;
      lastTime = nowTime;
    }
  }

  closeSocket(canSocket);
  fclose(logFP);

  fprintf(stderr, "\nStopping\n");

  return 0;
}