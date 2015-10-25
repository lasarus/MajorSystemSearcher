#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "phonetics.h"

struct phonetics * phonetics_init(void)
{
  int i;
  struct phonetics * phonetics = malloc(sizeof(struct phonetics));

  phonetics->n = 0;
  phonetics->words = NULL;
  for(i = 0; i < 10; i++)
    {
      phonetics->links[i] = NULL;
    }

  return phonetics;
}

void phonetics_free(struct phonetics * phonetics)
{
  int i;

  for(i = 0; i < 10; i++)
    {
      if(phonetics->links[i])
	{
	  phonetics_free(phonetics->links[i]);
	}
    }

  if(phonetics->words)
    {
      free(phonetics->words[0]);
    }
  free(phonetics->words);
  free(phonetics);
}
