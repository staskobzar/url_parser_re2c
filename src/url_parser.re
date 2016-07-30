#include <stdlib.h>
#include <string.h>
#include "url_parser.h"

int url_parse (const char *url_str, URL *url)
{
  const char *marker;
  const char *ctxmarker;
  const char *src = url_str;
  int pos = 0;
/*!re2c
  re2c:define:YYCTYPE = "unsigned char";
  re2c:define:YYCURSOR = url_str;
  re2c:define:YYMARKER = marker;
  re2c:define:YYCTXMARKER = ctxmarker;
  re2c:yyfill:enable = 0;

  EOF = "\x00";
  ALPHA = [a-zA-Z];
  DIGIT = [0-9];
  HEXDIG = [0-9a-fA-F];

  SUB_DELIMS = [!$&'()*+,;=] ;
  GEN_DELIMS = [:/?#\[\]@]   ;

  RESERVED = GEN_DELIMS | SUB_DELIMS ;
  UNRESERVED = ALPHA | DIGIT | [-._~] ;

  PCT_ENCODED = "%" HEXDIG HEXDIG ;

  PCHAR = UNRESERVED | PCT_ENCODED | SUB_DELIMS | ":" | "@" ;

  SEGMENT_NZ_NC = ( UNRESERVED | PCT_ENCODED | SUB_DELIMS | "@")+;
  SEGMENT_NZ = PCHAR+;
  SEGMENT = PCHAR*;

  USERINFO = UNRESERVED | PCT_ENCODED | SUB_DELIMS | ":";

  DEC_OCT = DIGIT | [1-9] DIGIT | "1" DIGIT{2} | "2" [0-4] DIGIT | "25" [0-5] ;

  IPV4ADDR = DEC_OCT "." DEC_OCT "." DEC_OCT "." DEC_OCT;

  H16 = HEXDIG{1,4};

  REGNAME = UNRESERVED | PCT_ENCODED | SUB_DELIMS ;

  // scheme
  * { return 0; }

  ALPHA (ALPHA | DIGIT | [+.-])* ":" {
    int len = url_str - src - 1;
    url->scheme = get_lex(src, &pos, len);
    pos++; // add ":"
    goto hier_part;
  }
*/

hier_part:
/*!re2c
  * { goto hier_part_path; }
  EOF { return 0;}
  "//"  { pos +=2; goto hier_part_authority; }
*/

hier_part_authority:
/*!re2c
  [^] { url_str--; goto hier_part_host; }
  EOF { return 0;}
  USERINFO* "@" {
    int len = url_str - src - pos - 1; // unshift 1 char for "@"
    url->userinfo = get_lex(src, &pos, len);
    pos++; // shift for "@" char
    goto hier_part_host;
  }
*/

hier_part_host:
/*!re2c
  EOF { return 0;}
  * { return 0; }
  "" / "/" {
    url->host->type = UNKNOWN;
    goto hier_part_path;
  }
  IPV4ADDR {
    int len = url_str - src - pos;
    url->host->type = IPV4ADDR;
    char *name = get_lex(src, &pos, len);
    strncpy (url->host->name, name, HOST_MAX_LEN < len + 1 ? HOST_MAX_LEN : len + 1);
    free (name);
    goto hier_part_port;
  }
  "[v" [^\]]+ "]" {
    pos++; // shift "["
    int len = url_str - src - pos - 1;
    url->host->type = IPVFUTUR;
    char *name = get_lex(src, &pos, len);
    pos++;
    strncpy (url->host->name, name, HOST_MAX_LEN < len + 1 ? HOST_MAX_LEN : len + 1);
    free (name);
    goto hier_part_port;
  }
  "[" [^\]]+ "]" {
    pos++; // shift "["
    int len = url_str - src - pos - 1; // skip "]"
    url->host->type = IPV6ADDR;
    char *name = get_lex(src, &pos, len);
    pos++;
    strncpy (url->host->name, name, HOST_MAX_LEN < len + 1 ? HOST_MAX_LEN : len + 1);
    free (name);
    goto hier_part_port;
  }
  REGNAME+ {
    int len = url_str - src - pos;
    url->host->type = REGNAME;
    char *name = get_lex(src, &pos, len);
    strncpy (url->host->name, name, HOST_MAX_LEN < len + 1 ? HOST_MAX_LEN : len + 1);
    free (name);
    goto hier_part_port;
  }
*/

hier_part_port:
/*!re2c
  EOF {
    url->valid = 1;
    return 1;
  }
  "" { goto hier_part_path; }
  ":" DIGIT* {
    pos++; // shift ":"
    int len = url_str - src - pos;
    char *port = get_lex(src, &pos, len);
    url->port = atoi (port);
    free (port);
    goto hier_part_path;
  }
*/

hier_part_path:
/*!re2c
  EOF {
    url->valid = 1;
    return 1;
  }
  * {goto query_frag;}
  ("/" SEGMENT)+ {
    int len = url_str - src - pos;
    url->path = get_lex(src, &pos, len);
    goto query_frag;
  }
*/

query_frag:
/*!re2c
  * { return 0; }
  EOF {
    url->valid = 1;
    return 1;
  }
  "?" (PCHAR | [/?])* {
    pos++; // shift "?"
    int len = url_str - src - pos;
    url->query = get_lex(src, &pos, len);
    goto query_frag;
  }
  "#" (PCHAR | [/?])* {
    pos++; // shift "#"
    int len = url_str - src - pos;
    url->fragment = get_lex(src, &pos, len);
    return 1;
  }
*/
  return 0;
}

URL *url_create ()
{
  URL *url = (URL*) malloc (sizeof(URL));
  url->scheme   = NULL;
  url->userinfo = NULL;
  url->host = (Host*) malloc (sizeof(Host));
  url->port = 0;
  url->path = NULL;
  url->query = NULL;
  url->fragment = NULL;
  url->valid = 0;
  return url;
}

void url_free (URL *url)
{
  if (url->scheme) free (url->scheme);
  if (url->userinfo) free (url->userinfo);
  if (url->host) free (url->host);
  if (url->path) free (url->path);
  if (url->query) free (url->query);
  if (url->fragment) free (url->fragment);
  free (url);
}

char * get_lex(const char *url, int *from, int len)
{
  char *res = (char *) malloc (len + 1);
  int i;
  for (i = 0; i < len; i++)
  {
    res[i] = url[*from + i];
  }
  res[i] = '\0';
  *from += len;
  return res;
}
