
# Sources
COMMON 				= $(wildcard common/*.cpp)
DEVICES 			= $(wildcard devices/*.cpp)
BLUETOOTH			= $(wildcard bluetooth/*.cpp)
DB 					= $(wildcard db/*.cpp)
NEAR_PASS_DETECTION = $(wildcard near_pass_detection/*.cpp)

ALL_SOURCES = $(COMMON) $(DEVICES) $(BLUETOOTH) $(DB) $(NEAR_PASS_DETECTION)

# Flags
CFLAGS = -std=c++17 -Wmaybe-uninitialized -Wall # -g -fsanitize=address
GPIO_FLAGS = -lgpiod
SQLITE_FLAGS = -lsqlite3
GOPRO_FLAGS = -lcurl -ljsoncpp

LIB_FLAGS = $(GPIO_FLAGS) $(SQLITE_FLAGS) # $(GOPRO_FLAGS)

TARGET = npits.exe

npits:
	g++ $(CFLAGS) main.cpp $(ALL_SOURCES) -o $(TARGET) $(LIB_FLAGS)
	chmod 644 $(TARGET)
	chmod +x $(TARGET)

clean:
	rm -f $(TARGET)
