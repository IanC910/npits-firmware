
# Sources
COMMON 				= $(wildcard ../common/*.cpp)
DEVICES 			= $(wildcard ../devices/*.cpp)
BLUETOOTH			= $(wildcard ../bluetooth/*.cpp)
DB 					= $(wildcard ../db/*.cpp)
NEAR_PASS_TRACKING 	= $(wildcard ../near_pass_tracking/*.cpp)

ALL_SOURCES = $(COMMON) $(DEVICES) $(BLUETOOTH) $(DB) $(NEAR_PASS_TRACKING)

# Flags
CFLAGS = -std=c++17 -g -Wmaybe-uninitialized -Wall
GPIO_FLAGS = -lgpiod
GOPRO_FLAGS = -lcurl -ljsoncpp
SQLITE_FLAGS = -lsqlite3

ALL_FLAGS = $(CFLAGS) $(GPIO_FLAGS) $(GOPRO_FLAGS) $(SQLITE_FLAGS)

TARGET = npits.exe

npits:
	g++ $(ALL_FLAGS) main.cpp $(ALL_SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
