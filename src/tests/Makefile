
CC = g++
CFLAGS = -std=c++17 -g -Wmaybe-uninitialized -Wall
CCF = $(CC) $(CFLAGS)

COMMON = $(wildcard ../common/*.cpp)
DEVICES = $(wildcard ../devices/*.cpp)
ALL_SOURCES = $(COMMON) $(DEVICES)
TEST_EXECS = $(wildcard *.test)

clean:
	rm -f $(TEST_EXECS)



# Simple Tests
gopro_simple_test:
	$(CCF) gopro_simple_test.cpp -o gopro_simple_test.test -lcurl -ljsoncpp

GPIO_simple_test:
	$(CCF) GPIO_simple_test.cpp -o GPIO_simple_test.test -lgpiod

gpio_simple_test:
	$(CCF) gpio_simple_test.cpp -o gpio_simple_test.test -lgpiod

MB1242_simple_test:
	$(CCF) MB1242_simple_test.cpp $(COMMON) -o MB1242_simple_test.test -lgpiod

OPS241_simple_test:
	$(CCF) OPS241_simple_test.cpp -o OPS241_simple_test.test

le_server_simple_test:
	g++ le_server_simple_test.c btlib.c -o le_server_simple_test.test -Wno-narrowing



# Tests
MB1242_direct_test:
	$(CCF) MB1242_direct_test.cpp $(ALL_SOURCES) -o MB1242_direct_test.test -lgpiod

MB1242_async_test:
	$(CCF) MB1242_async_test.cpp $(ALL_SOURCES) -o MB1242_async_test.test -lgpiod



OPS241_class_test:
	$(CCF) OPS241_class_test.cpp ../devices/OPS241.cpp -o OPS241_class_test.test