#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <url_parser.h>

static void test_url_parser_fail (void **state)
{
  URL *url = url_create();
  assert_int_equal (0, url_parse ("asdas", url));
  assert_int_equal (0, url_parse ("", url));
  assert_int_equal (0, url_parse ("-http://qwerty.com/asd", url));
  url_free (url);
}

static void test_url_parser_scheme (void **state)
{
  URL *url = url_create();
  const char *url_str = "http://www.ics.uci.edu/pub/ietf/uri/#Related";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal(url->scheme, "http");
  assert_null (url->port);
  assert_null (url->userinfo);

  const char *url_str1 = "ldap://1.1.1.1/pub/ietf/uri/#Related";
  assert_int_equal (1, url_parse (url_str1, url));
  assert_string_equal(url->scheme, "ldap");
  assert_null (url->port);
  assert_null (url->userinfo);
  url_free (url);
}

static void test_url_parser_hier_regname (void **state)
{
  URL *url = url_create();
  const char *url_str = "http://www.ics.uci.edu/pub/ietf/uri/#Related";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal (url->host->name, "www.ics.uci.edu");
  assert_int_equal (REGNAME, url->host->type);
  assert_null (url->port);
  assert_null (url->userinfo);

  // null hier part
  const char *url_str_1 = "file:///etc/hosts";
  assert_int_equal (1, url_parse (url_str_1, url));
  assert_int_equal (UNKNOWN, url->host->type);
  url_free (url);
}

static void test_url_parser_hier_regname_port (void **state)
{
  URL *url = url_create();
  const char *url_str = "http://www.ic.info:8080/pub/index.php";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal (url->host->name, "www.ic.info");
  assert_int_equal (8080, url->port);
  assert_int_equal (REGNAME, url->host->type);
  url_free (url);
}

static void test_url_parser_hier_ipv4 (void **state)
{
  URL *url = url_create();
  const char *url_str = "http://143.5.88.91/index.html";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal (url->host->name, "143.5.88.91");
  assert_int_equal (IPV4ADDR, url->host->type);
  assert_int_equal (0, url->port);
  url_free (url);
}

static void test_url_parser_hier_ipv4_port (void **state)
{
  URL *url = url_create();
  const char *url_str = "http://143.5.88.91:9898/index.html";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal (url->host->name, "143.5.88.91");
  assert_int_equal (IPV4ADDR, url->host->type);
  assert_int_equal (9898, url->port);
  url_free (url);
}

static void test_url_parser_hier_ipv6 (void **state)
{
  URL *url = url_create();
  const char *url_str = "ftp://[2000::FF:1BAC]/index.html";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal (url->host->name, "2000::FF:1BAC");
  assert_int_equal (IPV6ADDR, url->host->type);
  assert_int_equal (0, url->port);
  url_free (url);
}

static void test_url_parser_hier_ipv6_port (void **state)
{
  URL *url = url_create();
  const char *url_str = "ftp://[::ffff:204.152.189.116]:8899/index.html";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal (url->host->name, "::ffff:204.152.189.116");
  assert_int_equal (IPV6ADDR, url->host->type);
  assert_int_equal (8899, url->port);
  url_free (url);
}

static void test_url_parser_hier_ipvfuture (void **state)
{
  URL *url = url_create();
  const char *url_str = "web://[vFF01A.host]";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal (url->host->name, "vFF01A.host");
  assert_int_equal (IPVFUTUR, url->host->type);
  assert_int_equal (0, url->port);
  url_free (url);
}

static void test_url_parser_hier_ipvfuture_port (void **state)
{
  URL *url = url_create();
  const char *url_str = "web://[vFF01A.host]:6547";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal (url->host->name, "vFF01A.host");
  assert_int_equal (IPVFUTUR, url->host->type);
  assert_int_equal (6547, url->port);
  assert_null (url->userinfo);
  assert_null (url->path);
  assert_null (url->query);
  assert_null (url->fragment);
  url_free (url);
}

static void test_url_parser_path (void **state)
{
  URL *url = url_create();
  const char *url_str = "http://www.giac.org/practical/gsec/Richard_Siedzik_GSEC.pdf";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal (url->path, "/practical/gsec/Richard_Siedzik_GSEC.pdf");
  url_free (url);
}

static void test_url_parser_query (void **state)
{
  URL *url = url_create();
  const char *url_str = "ldap://[2001:db8::7]/c=GB?objectClass?one#section5";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal (url->query, "objectClass?one");
  url_free (url);
}

static void test_url_parser_fragment (void **state)
{
  URL *url = url_create();
  const char *url_str = "ldap://[2001:db8::7]/c=GB?objectClass?one#section5";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal (url->fragment, "section5");
  url_free (url);
}

static void test_url_parser (void **state)
{
  URL *url = url_create();
  const char *url_str = "foo://alice:secret@example.com:8042/over/there?name=ferret#nose";
  assert_int_equal (1, url_parse (url_str, url));
  assert_string_equal (url->scheme,     "foo");
  assert_string_equal (url->userinfo,   "alice:secret");
  assert_string_equal (url->host->name, "example.com");
  assert_int_equal    (url->host->type, REGNAME);
  assert_int_equal    (url->port,       8042);
  assert_string_equal (url->path,       "/over/there");
  assert_string_equal (url->query,      "name=ferret");
  assert_string_equal (url->fragment,   "nose");
  url_free (url);
}

int main(int argc, const char *argv[])
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_url_parser_fail),
    cmocka_unit_test(test_url_parser_scheme),
    cmocka_unit_test(test_url_parser_hier_regname),
    cmocka_unit_test(test_url_parser_hier_regname_port),
    cmocka_unit_test(test_url_parser_hier_ipv4),
    cmocka_unit_test(test_url_parser_hier_ipv4_port),
    cmocka_unit_test(test_url_parser_hier_ipv6),
    cmocka_unit_test(test_url_parser_hier_ipv6_port),
    cmocka_unit_test(test_url_parser_hier_ipvfuture),
    cmocka_unit_test(test_url_parser_hier_ipvfuture_port),
    cmocka_unit_test(test_url_parser_path),
    cmocka_unit_test(test_url_parser_query),
    cmocka_unit_test(test_url_parser_fragment),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
  return 0;
}
/* vim: let g:syntastic_c_include_dirs = ['src'] */
