#
# Testing with cmocka
#
LDFLAGS += -lcmocka -lurlparser
CFLAGS = -I../src -L../src

OS := $(shell uname -s)
ifeq ($(OS), Darwin)
	CFLAGS += -I/usr/local/include -L/usr/local/lib
endif

test: url_parser_test
	./url_parser_test

url_parser_test: url_parser_test.c
	$(CC) -o url_parser_test url_parser_test.c $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f url_parser_test


