
CC = g++
CFLAGS = -g -Wmaybe-uninitialized -Wall
CCF = $(CC) $(CFLAGS)

DEVICES = $(wildcard ../devices/*.cpp)
COMMON = $(wildcard ../common/*.cpp)
TEST_EXECS = $(wildcard *.test)

clean:
	rm -f $(TEST_EXECS)



# Simple Tests
GPIO_simple_test:
	$(CCF) GPIO_simple_test.cpp -o GPIO_simple_test.test -lgpiod

MB1242_simple_test:
	$(CCF) MB1242_simple_test.cpp $(COMMON) -o MB1242_simple_test.test -lgpiod

OPS241_simple_test:
	$(CCF) OPS241_simple_test.cpp -o OPS241_simple_test.test



# Tests
MB1242_test:
	$(CCF) MB1242_test.cpp $(COMMON) $(DEVICES) -o MB1242_test.test -lgpiod

OPS241_test:
	$(CCF) OPS241_test.cpp ../devices/OPS241.cpp -o OPS241_test.test