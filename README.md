# Vehicle Canbus Simulator
This project allow you to playback CANBUS messages over a local network to aid development without requiring access to a vehicle.

It currently supports (Vetor's) `*.asc` CANBUS log files as these are compact and include timing information.

The code is designed to run on unix machines, including the Raspberry Pi. It relies on SocketCAN documented at https://github.com/linux-can/can-utils which makes your CANBUS device available as a standard socket network device. Unfortunately, this code will not work on Mac OSX because can-utils does not work on OSX.

## CANBUS Networks
A CANBUS network is a two wire network which requires a minimum of two nodes. There is no concept of master or slave; each node on the network is a peer of the others.

The network works using differential voltage signalling on two wires; normally designated CAN-H and CAN-L. The network also requires a termination resistor (typically 120 ohm) at each end to avoid signal reflection.

Nodes transmit and receive messages at upto 1Mbps. Tesla's CANBUS networks typically run at 500kbps with around 1-2k messages per second transmitted on the bus.

See https://en.wikipedia.org/wiki/CAN_bus for more information.

## Hardware Requirements
The Vehicle Canbus Simulator application requires a USB connected CANBUS transceiver to work. Without it the application is unable to transmit messages from the log file to the CANBUS.

Sample set up using two raspberry Pi's and two USB-CANBUS boards (from Inno-maker).

![](./doc/CanBus-Setup.jpg "Canbus setup")

It is possible to use a single raspberry Pi by connecting the two USBs to the pi. You will then need to configure both `can0` and `can1` on the device. Note that a single pi may struggle to send/receive messages without dropping frames at higher CANBUS bit rates.

## Installing Dependencies
For Debian based distros (including the Raspberry Pi), you will need to install `can-utils` on the computer hosting the Vehicle Canbus Simulator application. This is simple to do with the standard raspbian images (based on Debian).

```
apt-get install can-utils
```

Then connect your CANBUS device to the USB port of the computer.

## Configuring the CANBUS

You will need to configure the CANBUS before starting the Can Simulator App. 

```
# configure CAN0 with bitrate set to 500kbps
ip link set can0 type can bitrate 500000

# bring the network up
ifconfig can0 up
```

## Launching the application

The app supports various switches to control its behavior.

If you pass no parameters then the application will look for a log file called `sample.asc` in the same folder as the application.

It will replay the log file in a loop until you kill the app using Ctrl-C.

```
./cansimulator
```

### App Switches

The following command line options are supported

```
-l on / off - looping on or off
-f logfile  - the name of the logfile to read
-t on / off - if on then honour the timings from the log file
-d delay - if -t is off then sets delay in ms between messages
```

# Test and Build
You will need to have the standard C build tools installed in order to test and build the app for yourself.

## Unit Tests

Invoke the unit tests using the following command:

```
make clean test
```
The output will indicate whether the tests ran sucessfully or not.

## Building the App

To build the app, use the following command:

```
make clean build
```

This will create an executable called `XXXXX` in the bin directory