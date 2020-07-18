/* CANBUS Simulator - replays CAN messages from log files
 * Copyright (C) 2020  John Sidney-Woollett 
 * All Rights Reserved
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 * License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE
 */

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include "canlog.h"

int openCanSocket(int *canSocket, int loopback_disable);
int bindSocket(int socket, char *ifName, struct sockaddr_can *addr);
int sendCanFrameToSocket(int socket, CanFrame *canFrame);
void closeSocket(int canSocket);