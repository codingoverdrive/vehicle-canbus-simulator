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
#include <stdlib.h>
#include <unistd.h>
#include "include/cansocket.h"

int openCanSocket(int *canSocket, int enableLoopback) {
  int sock;
  if ((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("openCanSocket: socket");
    return 1;
  }

  /* disable unneeded default receive filter on this RAW socket */
  setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

  if (!enableLoopback) {
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
  // fprintf(stderr, "\nifreq, %s -> %d\n\n", ifName, ifr.ifr_ifindex);
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
    // printf("Bytes sent: %d %d\n", numBytes, errno);
    if (numBytes < 0 && errno == ENOBUFS) {
      // fprintf(stderr, ".");
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
