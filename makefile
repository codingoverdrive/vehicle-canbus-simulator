.PHONY: clean
.PHONY: test

# Build Test artifacts
src_test = $(wildcard src/*.c) \
           $(wildcard test/*.c)
obj_test = $(src_test:.c=.o)

# Build app artifacts
src_app = $(wildcard src/*.c) \
          $(wildcard src/app/*.c)
obj_app = $(src_app:.c=.o)

CFLAGS =-I./src/include -I./test/include
LDFLAGS =-v #-lGL -lglut -lpng -lz -lm

TESTRUNNER = ./bin/testrunner
APP = ./bin/app

testrunner: $(obj_test)
	$(CC) $(CFLAGS) -o bin/$@ $^ $(LDFLAGS)

test: testrunner
	$(TESTRUNNER)

app: $(obj_app)
	$(CC) $(CFLAGS) -o bin/$@ $^ $(LDFLAGS)

all: clean test app

clean:
	rm -f $(obj_test) $(TESTRUNNER)
	rm -f $(obj_app) $(APP)