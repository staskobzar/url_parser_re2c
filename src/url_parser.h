#ifndef URL_PARSER_H
#define URL_PARSER_H

#define HOST_MAX_LEN 255

enum host_type {
  UNKNOWN,
  REGNAME,
  IPV6ADDR,
  IPV4ADDR,
  IPVFUTUR
};

typedef struct _host_t {
  enum host_type type;
  char name[HOST_MAX_LEN];
} Host;

typedef struct _url_t {
  char *scheme;
  char *userinfo;
  Host *host;
  unsigned int port;
  char *path;
  char *query;
  char *fragment;
  short valid : 1;
} URL;

int url_parse (const char * urlstr, URL *url);
URL *url_create ();
void url_free (URL *url);
char * get_lex(const char *url, int *from, int len);

#endif
