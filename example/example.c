#include <stdio.h>
#include <url_parser.h>

void usage(const char *prog)
{
  printf ("===================================\n");
  printf ("Usage: %s url\n", prog);
  printf ("===================================\n");
}
int main(int argc, const char *argv[])
{
  if (argc < 2)
  {
    usage (argv[0]);
    return 0;
  }

  URL *url = url_create();
  printf ("===================================\n");
  if (url_parse (argv[1], url))
  {
    printf ("Parsed url: %s\n", argv[1]);
    printf ("  scheme:    %s\n", url->scheme);
    printf ("  userinfo:  %s\n", url->userinfo);
    printf ("  host type: ");
    switch (url->host->type)
    {
      case REGNAME: printf ("DOMAIN"); break;
      case IPV4ADDR: printf ("IPv4 address"); break;
      case IPV6ADDR: printf ("IPv6 address"); break;
      case IPVFUTUR: printf ("IP future"); break;
      default: printf ("UNKNOWN"); break;
    }
    printf ("\n");
    printf ("  host:      %s\n", url->host->name);
    printf ("  port:      %d\n", url->port);
    printf ("  path:      %s\n", url->path);
    printf ("  query:     %s\n", url->query);
    printf ("  fragment:  %s\n", url->fragment);
  } else {
    printf ("ERROR: Invalid url: %s\n", argv[1]);
  }
  printf ("===================================\n");
  url_free (url);
  return 0;
}

/* vim: let g:syntastic_c_include_dirs = ['src'] */
