
CC = g++
CFLAGS = -std=c++17 -g -Wmaybe-uninitialized -Wall
CCF = $(CC) $(CFLAGS)
# Sources
COMMON 				= $(wildcard ../common/*.cpp)
DEVICES 			= $(wildcard ../devices/*.cpp)
BLUETOOTH			= $(wildcard ../bluetooth/*.cpp)
DB 					= $(wildcard ../db/*.cpp)
NEAR_PASS_DETECTION = $(wildcard ../near_pass_detection/*.cpp)

ALL_SOURCES = $(COMMON) $(DEVICES) $(BLUETOOTH) $(DB) $(NEAR_PASS_TRACKING)

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
	g++ le_server_simple_test.c ../bluetooth/btlib.cpp -o le_server_simple_test.test -Wno-narrowing



# Tests
db_query_during_ride_test:
	$(CCF) db_query_during_ride_test.cpp $(COMMON) $(DB) -o db_query_during_ride_test.test -lsqlite3

MB1242_direct_test:
	$(CCF) MB1242_direct_test.cpp $(COMMON) $(DEVICES) -o MB1242_direct_test.test -lgpiod

MB1242_async_test:
	$(CCF) MB1242_async_test.cpp $(COMMON) $(DEVICES) -o MB1242_async_test.test -lgpiod

near_pass_db_test:
	$(CCF) near_pass_db_test.cpp $(COMMON) $(DB) -o near_pass_db_test.test -lsqlite3

NearPassDetector_class_test:
	$(CCF) NearPassDetector_class_test.cpp $(COMMON) $(DEVICES) $(NEAR_PASS_DETECTION) $(DB) -o NearPassDetector_class_test.test -lgpiod -lsqlite3

NearPassPredictor_class_test:
	$(CCF) NearPassPredictor_class_test.cpp $(COMMON) $(DEVICES) $(NEAR_PASS_DETECTION) $(DB) -o NearPassPredictor_class_test.test -lsqlite3 -lgpiod

OPS241_class_test:
	$(CCF) OPS241_class_test.cpp ../devices/OPS241.cpp -o OPS241_class_test.test

OPS243_class_test:
	$(CCF) OPS243_class_test.cpp ../devices/OPS243.cpp -o OPS243_class_test.test

sensor_coordination_test:
	$(CCF) sensor_coordination_test.cpp $(COMMON) $(DEVICES) $(NEAR_PASS_DETECTION) $(DB) -o sensor_coordination_test.test -lgpiod -lsqlite3

time_test:
	$(CCF) time_test.cpp $(COMMON) -o time_test.test
