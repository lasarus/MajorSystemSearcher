#ifndef PHONETICS_H
#define PHONETICS_H

struct phonetics
{
  int n;
  char ** words;

  struct phonetics * links[10];
};

struct phonetics * phonetics_init(void);
void phonetics_free(struct phonetics * phonetics);

#endif
