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
#define DEFAULT_LOOPS 1

void printCommandLineOptions(char *progName) {
  fprintf(stderr, "\nUsage: %s <options> logfile\n\n", progName);
  fprintf(stderr, "Options:      -c <canbus>  "
    "(default is can0)\n");
	fprintf(stderr, "              -l <num>     "
		"(process input file <num> times, default=1)\n");
  fprintf(stderr, "              -i           "
   	"(infinite or loop forever)\n");
  fprintf(stderr, "              -t           "
    "(ignore timestamps and send frames with no delay)\n");
  fprintf(stderr, "              -m <ms>     "
    "(skip gaps in timestamps > 'ms' milliseconds)\n");
  fprintf(stderr, "\n");
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
		fprintf(stderr, "bindSocket: SIOCGIFINDEX for %s\n", ifName);
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
  int dataSize = sizeof(CanFrame);
  long numBytes = 0;
  int maxRetries = 100;
  while (maxRetries-- > 0) {
    numBytes = write(socket, canFrame, dataSize);
    printf("Bytes sent: %d %d\n", numBytes, errno);
    if (numBytes < 0 && errno == ENOBUFS) {
      fprintf(stderr, ".");
      usleep(50000);
    }
    else {
      break;
    }
  }

  if (numBytes != dataSize) {
      perror("Cannot write to socket");
      return 1;
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

  // command line options
  static char ifname[32] = "can0";
  static int opt = 0;
  static int verbose; // show frames as they are sent
  static int infiniteLoops = 0; // loop forever
  static int loops = 0;         // specify a set number of loops
  static int ignoreTimestamp = 0; // ignore time between frames, send without delay
  static int maxDelayMS = 0; // the maximum delay allowed between frames

  // process command line options
  while ((opt = getopt(argc, argv, "ivtm:l:c:")) != -1) {
    switch (opt) {
    case 'c':
      strcpy(ifname, optarg);
      break;
    case 'i':
      infiniteLoops = 1;
      break;
    case 'l':
      if (!(loops = atoi(optarg))) {
					fprintf(stderr, "Invalid numeric argument for option -l !\n");
					return 1;
			}
      break;
    case 'v':
      verbose = 1;
      break;
    case 'm':
      if (!(maxDelayMS = atoi(optarg))) {
					fprintf(stderr, "Invalid numeric argument for option -s !\n");
					return 1;
			}
      break;
    case 't':
      ignoreTimestamp = 1;
      break;
    default:
      printCommandLineOptions(basename(argv[0]));
      return 1;
      break;
    }    
  }

  // ensure that we loop correctly
  if (loops > 0)
    infiniteLoops = 0;
  else if (loops < DEFAULT_LOOPS)
    loops = DEFAULT_LOOPS;

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
  // printf("canSocket %d\n",canSocket);
  struct sockaddr_can canAddr;
  if (verbose)
    printf("Interface: %s\n", ifname);
  if (bindSocket(canSocket, ifname, &canAddr) > 0) 
      return 1;

  struct timeval lastTime, nowTime, lastFrameTime;
  gettimeofday(&lastTime, NULL);
  lastFrameTime.tv_sec = 0;
  lastFrameTime.tv_usec = 0;

  int loopCounter = 0;
  while (keepRunning && (infiniteLoops || loops--)) {
    if (verbose)
      fprintf(stderr, "--- loop: %d ---\n", ++loopCounter);
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
      if (!ignoreTimestamp && lastFrameTime.tv_sec != 0 && lastFrameTime.tv_usec != 0) {
        // printf("LAST> %ld.%ld\n", lastTime.tv_sec, lastTime.tv_usec);
        // printf("NOW> %ld.%ld\n", nowTime.tv_sec, nowTime.tv_usec);
        unsigned long elapsedTime = deltaTimeMs(nowTime, lastTime);
        unsigned long frameDeltaTime = deltaTimeMs(canMsg.time, lastFrameTime);
        long sleepTimeMS = frameDeltaTime - elapsedTime;
        // fprintf(stderr, "%d %d SLEEP %d\n", elapsedTime, frameDeltaTime, sleepTimeMS);
        if (maxDelayMS && sleepTimeMS > maxDelayMS) {
          sleepTimeMS = maxDelayMS;
        }
        if (sleepTimeMS > 0) {
          usleep(1000 * sleepTimeMS);
          gettimeofday(&nowTime, NULL);
        }
      }

      canMessage2Frame(&canMsg, &canFrame);
      if (verbose)
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

  fprintf(stderr, "\nStopping\n\n");

  return 0;
}