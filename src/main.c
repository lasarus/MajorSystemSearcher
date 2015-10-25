#include <stdio.h>
#include <string.h>

#include "phonetics.h"
#include "parser.h"
#include "gui.h"

static void print_number(struct phonetics * phonetics, char * number)
{
  int i;
  for(i = 0; number[i] != '\0'; i++)
    {
      if(number[i] < '0' || number[i] > '9')
	{
	  printf("invalid token!\n");
	  break;
	}

      if(phonetics->links[number[i] - '0'])
	{
	  phonetics = phonetics->links[number[i] - '0'];
	}
      else
	{
	  break;
	}

      if(phonetics->words)
	{
	  fwrite(number, i + 1, 1, stdout);
	  printf("\n-");
	  for(int j = 0; j < phonetics->n; j++)
	    {
	      printf(" %s", phonetics->words[j]);
	    }
	  printf("\n");
	}
    }
}

static void listen(struct phonetics * phonetics)
{
  char buffer[256] = "";
  while(1)
    {
      printf("> ");
      fgets(buffer, 256, stdin);
      buffer[strlen(buffer) - 1] = '\0'; // remove newline

      if(strcmp(buffer, "quit") == 0)
	{
	  break;
	}

      print_number(phonetics, buffer);
    }
}

enum
{
  MAIN_GUI,
  MAIN_INTERACTIVE
};

int main(int argc, char ** argv)
{
  char * path = "dictionary.phon";
  char * number = NULL;
  char * output = NULL;

  int mode = MAIN_INTERACTIVE;
  for(int i = 1; i < argc; i++)
    {
      if(strcmp(argv[i], "-d") == 0)
	{
	  path = argv[i + 1];
	  i++;
	}
      else if(strcmp(argv[i], "-n") == 0)
	{
	  number = argv[i + 1];
	  i++;
	}
      else if(strcmp(argv[i], "-g") == 0)
	{
	  mode = MAIN_GUI;
	}
      else if(strcmp(argv[i], "-o") == 0)
	{
	  output = argv[i + 1];
	  i++;
	}
    }

  FILE * file = fopen(path, "r");
  struct phonetics * phonetics = parse_phonetics(file);
  fclose(file);

  switch(mode)
    {
    case MAIN_INTERACTIVE:
      if(number)
	{
	  print_number(phonetics, number);
	}
      else
	{
	  listen(phonetics);
	}
      break;

    case MAIN_GUI:
      gui_main(phonetics, number, output);
      break;
    }

  phonetics_free(phonetics);
}
