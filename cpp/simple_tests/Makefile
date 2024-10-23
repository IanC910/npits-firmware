
CCF = g++ -Wall -Werror

TEST_EXECS = $(wildcard *.test)

clean:
	rm -f $(TEST_EXECS)

GPIO_simple_test:
	$(CCF) GPIO_simple_test.cpp -o GPIO_simple_test.test -lgpiod

MB1242_simple_test:
	$(CCF) MB1242_simple_test.cpp -o MB1242_simple_test.test -lgpiod

OPS241_simple_test:
	$(CCF) OPS241_simple_test.cpp -o OPS241_simple_test.test