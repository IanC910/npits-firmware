
CCF = g++ -Wall -Werror

DEVICES = $(wildcard ../devices/*.cpp)

TEST_EXECS = $(wildcard *.test)

clean:
	rm -f $(TEST_EXECS)

MB1242_test:
	$(CCF) MB1242_test.cpp $(DEVICES) -o MB1242_test.test -lgpiod

OPS241_test:
	$(CCF) OPS241_test.cpp ../devices/OPS241.cpp -o OPS241_test.test