### URL Parsing with re2c

Small library for parsing URL. Parsing rules are based on ABNF RFC 3986
[Apendix A](https://tools.ietf.org/html/rfc3986#appendix-A).

Library is using [re2c lexer](http://re2c.org/) to generate C parsing code.

See how to use in "example" directory.

Used in ruby library as C extension (https://github.com/staskobzar/url_parser_ruby_ext_re2c).
More details in the (blog article)[http://staskobzar.blogspot.ca/2016/08/url-parser-ruby-c-extension-binding.html]
