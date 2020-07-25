#include <stdio.h>
// #include <linux/can.h>

#include "../include/canlog.h"

#define VERSION "v1.0.1"

void printCommandLineOptions(char *progName) {
  fprintf(stderr, "\nVehicle Canbus Simulator %s\n", VERSION);
  fprintf(stderr, "\nUsage: %s <options> logfile\n\n", progName);
}

int openCanSocket(int *socket) {
  //   if ((*socket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
  //     perror("socket");
  //     return 1;
  //   }

  //   addr.can_family = AF_CAN;
  //   addr.can_ifindex = 0;

  //   /* disable unneeded default receive filter on this RAW socket */
  //   setsockopt(*socket, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

  //   if (loopback_disable) {
  //     int loopback = 0;
  //     setsockopt(*socket, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback,
  //     sizeof(loopback));
  //   }

  return 0;
}

int bindSocket(int socket, int ifIndex, struct sockaddr_can *addr) {
  //   addr.can_family = AF_CAN;
  //   addr.can_ifindex = 0;

  //   if (bind(s, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
  //     perror("bind");
  //     return 1;
  //   }
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printCommandLineOptions(argv[0]);
    return 0;
  }
}