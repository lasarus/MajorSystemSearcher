#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "phonetics.h"
#include "parser.h"

enum
  {
    STATE_NUM,
    STATE_WORDS,
    STATE_NEXT,
    STATE_ERROR
  };

static char * error_msg = NULL;

static void parse_num(int c, int * state, struct phonetics ** current)
{
  if(c >= '0' && c <= '9')
    {
      struct phonetics * link = (*current)->links[c - '0'];
      if(!link)
	{
	  link = phonetics_init();
	  (*current)->links[c - '0'] = link;
	}
      *current = link;
    }
  else if(c == ' ')
    {
      *state = STATE_WORDS;
    }
  else
    {
      *state = STATE_ERROR;
      error_msg = "expected space";
    }
}

static int char_index = 0;
static void parse_words(int c, int * state,
			char ** buffer, int * buffer_length)
{
  if(c == '\n')
    {
      (*buffer)[char_index] = '\0';
      *state = STATE_NEXT;
    }
  else
    {
      if(char_index >= *buffer_length - 1) // reserve space for \0
	{
	  *buffer_length *= 2;
	  *buffer = realloc(*buffer, *buffer_length);
	}

      (*buffer)[char_index] = c;
      char_index++;
    }
}

struct phonetics * parse_phonetics(FILE * file)
{
  struct phonetics * base = phonetics_init();
  struct phonetics * current = base;
  int c = 0;
  int state = STATE_NUM;

  int line = 0;

  int buffer_length = 128;
  char * buffer = malloc(buffer_length);
  while((c = getc(file)) != EOF)
    {
      if(c == '\n')
	line++;

      switch(state)
	{
	case STATE_NUM:
	  parse_num(c, &state, &current);
	  break;

	case STATE_WORDS:
	  parse_words(c, &state, &buffer, &buffer_length);
	  break;

	case STATE_ERROR:
	default:
	  printf("error at line %i:\n", line);
	  if(error_msg)
	    printf(">  %s\n", error_msg);
	  exit(1);
	}

      if(state == STATE_NEXT)
	{
	  int n = 1;
	  int i, current_word;
	  for(i = 0; i < char_index; i++)
	    {
	      if(buffer[i] == ' ')
		{
		  n++;
		}
	    }

	  current->words = malloc(sizeof(char *) * n);
	  char * tmp_buffer = malloc(char_index + 1);
	  strcpy(tmp_buffer, buffer);

	  current->words[0] = tmp_buffer;
	  current_word = 1;
	  for(i = 0; i < char_index; i++)
	    {
	      if(tmp_buffer[i] == ' ')
		{
		  tmp_buffer[i] = '\0';
		  current->words[current_word] = &tmp_buffer[i + 1];
		  current_word++;
		}
	    }

	  current->n = n;

	  char_index = 0;

	  current = base;
	  state = STATE_NUM;
	}
    }

  free(buffer);

  return base;
}
