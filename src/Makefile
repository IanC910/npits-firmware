
CROSS_TOOL = aarch64-linux-gnu-

# Sources
COMMON 				= $(wildcard common/*.cpp)
DEVICES 			= $(wildcard devices/*.cpp)
BLUETOOTH			= $(wildcard bluetooth/*.cpp)
DB 					= $(wildcard db/*.cpp)
NEAR_PASS_DETECTOR 	= $(wildcard near_pass_detector/*.cpp)

ALL_SOURCES = $(COMMON) $(DEVICES) $(BLUETOOTH) $(DB) $(NEAR_PASS_DETECTOR)

# Flags
CFLAGS = -std=c++17 -g -Wmaybe-uninitialized -Wall
GPIO_FLAGS = -lgpiod
SQLITE_FLAGS = -lsqlite3
GOPRO_FLAGS = -lcurl -ljsoncpp

LIB_FLAGS = $(GPIO_FLAGS) $(SQLITE_FLAGS) # $(GOPRO_FLAGS)

TARGET = npits.exe

npits:
	g++ $(CFLAGS) main.cpp $(ALL_SOURCES) -o $(TARGET) $(LIB_FLAGS)

clean:
	rm -f $(TARGET)
