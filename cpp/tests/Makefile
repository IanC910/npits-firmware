
CCF = g++ -Wall -Werror

TEST_EXECS = $(wildcard *.test)

clean:
	rm -f $(TEST_EXECS)

MB1242_test:
	$(CCF) MB1242_test.cpp ../devices/MB1242.cpp ../devices/GPIO.cpp -o MB1242_test.test -lgpiod

OPS241_test:
	$(CCF) OPS241_test.cpp ../devices/OPS241.cpp -o OPS241_test.test