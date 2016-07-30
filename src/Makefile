
liburlparser.a: url_parser.o
	ar rcs $@ $^

url_parser.o: url_parser.c url_parser.h

url_parser.c: url_parser.re
	re2c -o $@ $<

.PHONY: clean

clean:
	rm -f url_parser.c url_parser.o liburlparser.a
