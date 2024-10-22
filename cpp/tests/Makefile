
CCF = g++ -Wall -Werror

ROOT_DIR = ..

DEVICES = $(wildcard $(ROOT_DIR)/devices/*.cpp)

TEST_EXECS = $(wildcard *.test)

clean:
	rm -f $(TEST_EXECS)

OPS241_test:
	$(CCF) OPS241_test.cpp ../devices/OPS241.cpp -o OPS241_test.test