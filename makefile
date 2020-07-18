# CANBUS Simulator - replays CAN messages from log files
# Copyright (C) 2020  John Sidney-Woollett
# This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
# License: https://github.com/codingoverdrive/vehicle-canbus-simulator/blob/master/LICENSE

.PHONY: clean
.PHONY: test

# Build Test artifacts
src_test = $(filter-out src/cansim.c, $(wildcard src/*.c)) \
           $(wildcard test/*.c)
obj_test = $(src_test:.c=.o)

# Build app artifacts
src_app = $(wildcard src/*.c) 
obj_app = $(src_app:.c=.o)

CFLAGS =-I./src/include -I./test/include
LDFLAGS =-v #-lGL -lglut -lpng -lz -lm

TESTRUNNER = ./bin/testrunner
APP = ./bin/cansim

testrunner: $(obj_test)
	$(CC) $(CFLAGS) -o bin/$@ $^ $(LDFLAGS)

test: testrunner
	$(TESTRUNNER)

app: $(obj_app)
	$(CC) $(CFLAGS) -o $(APP) $^ $(LDFLAGS)

all: clean test app

clean:
	rm -f $(obj_test) $(TESTRUNNER)
	rm -f $(obj_app) $(APP)