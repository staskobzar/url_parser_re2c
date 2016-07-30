#
# Makefile for re2c-url-parser project
#
PROJECT     := url_parser
VERSION     := 1.0
SRCDIR      := src
TESTS       := tests

all: parser

.PHONY: parser
parser:
	$(MAKE) -C $(SRCDIR)

.PHONY: test
test:
	$(MAKE) -C $(TESTS)

.PHONY: clean
clean:
	$(MAKE) -C $(SRCDIR) $@
	$(MAKE) -C $(TESTS) $@
	rm -f example/example

.PHONY: memtest
memtest: example
	valgrind ./example/example "https://user:pass@localhost.org:8899/index.php?q=letto#wello"

.PHONY: check
check: clean all test

.PHONY: example
example:
	$(MAKE) -C example
