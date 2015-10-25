#include <curses.h>
#include <string.h>
#include <stdlib.h>

#include "phonetics.h"
#include "gui.h"

struct number_lists
{
  int n;
  int selected;

  struct internal_list
  {
    int selected;

    int n;
    char ** words;
  } * numbers;
};

static void free_number_list(struct number_lists * number_list)
{
  free(number_list->numbers);
}

static void init_number_list(struct number_lists * number_list,
			     struct phonetics * phonetics, char * number)
{
  number_list->numbers = malloc(sizeof(struct internal_list) * 256);
  int i, n = 0;
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

      number_list->numbers[n].n = phonetics->n;
      number_list->numbers[n].words = phonetics->words;
      number_list->numbers[n].selected = 0;
      n++;
    }
  number_list->n = n;
  number_list->selected = n - 1;
}

static void draw_number_list(struct number_lists * list)
{
  for(int i = 0; i < list->n; i++)
    {
      mvprintw(i * 2 + 2, 1, "%i, %i/%i%*c", i + 1, list->numbers[i].selected + 1, list->numbers[i].n, 16, ' ');
      mvprintw(i * 2 + 2, 0, "%c", (list->selected == i) ? '>' : ' ');
      if(list->numbers[i].words)
	mvprintw(i * 2 + 2, 16, "%s%*c", list->numbers[i].words[list->numbers[i].selected], 40, ' ');
    }
}

void find_interesting(struct phonetics * start, char * number)
{
  int i, j;
  struct phonetics * longest = NULL;
  int longest_n = -1;
  int longest_i = 0;
  for(i = 0; number[i] != '\0'; i++)
    {
      struct phonetics * phonetics = start;
      for(j = i; number[j] != '\0'; j++)
	{
	  if(number[j] < '0' || number[j] > '9')
	    {
	      printf("invalid token!\n");
	      break;
	    }

	  if(phonetics->links[number[j] - '0'])
	    {
	      phonetics = phonetics->links[number[j] - '0'];
	    }
	  else
	    {
	      break;
	    }

	  if(phonetics->words && j - i > longest_n)
	    {
	      longest = phonetics;
	      longest_n = j - i;
	      longest_i = i;
	    }
	}
    }
  if(longest)
    {
      mvprintw(30, 0, "%i - %.*s\n", longest_n + 1, longest_n + 1, number + longest_i);
      for(int i = 0; i < longest->n; i++)
	{
	  printw("%s ", longest->words[i]);
	}
      printf("\n");
    }
}

int gui_main(struct phonetics * phonetics, char * num, char * output)
{
  initscr();
  cbreak();
  keypad(stdscr, 1);

  char number[256];
  if(num == NULL)
    {
      printw("ENTER NUMBER:");
      refresh();
      getstr(number);
    }
  else
    {
      strcpy(number, num);
    }
  struct number_lists list;
  init_number_list(&list, phonetics, number);

  noecho();
  int done = 0;
  int used[256];
  used[0] = -1;

  char * output_buffer = malloc(256);
  output_buffer[0] = 0;
  size_t output_size = 256;

  int end_loop = 0;
  clear();
  printw("%s", number + done);
  draw_number_list(&list);
  printw("\n%s%*c", output_buffer, 100, ' ');
  refresh();
  while(!end_loop)
    {
      int buffer_change = 0;
      int c = getch();
      //struct internal_list * selected = NULL;

      struct internal_list * selected = &list.numbers[list.selected];
      switch(c)
	{
	case 'q':
	  end_loop = 1;
	  break;

	case KEY_UP:
	case 'k':
	  list.selected -= 1;
	  break;

	case KEY_DOWN:
	case 'j':
	  list.selected += 1;
	  break;

	case KEY_RIGHT:
	case 'l':
	  if(selected)
	    {
	      selected->selected += 1;
	    }
	  break;

	case KEY_LEFT:
	case 'h':
	  if(selected)
	    {
	      selected->selected -= 1;
	    }
	  break;

	case 'i':
	  find_interesting(phonetics, number + done);
	  break;

	case '\n':
	case KEY_ENTER:
	case ' ':
	  {
	    if(number[done] == '\0')
	      {
		end_loop = 1;
		break;
	      }

	    if(!selected->words)
	      {
		break;
	      }
	    size_t buffer_length = strlen(output_buffer);
	    char * sel = selected->words[selected->selected];
	    if(buffer_length + strlen(sel) + 2 > output_size)
	      {
		output_size *= 2;
		output_buffer = realloc(output_buffer, output_size);
	      }

	    for(int i = 0; i < 256; i++)
	      {
		if(used[i] == -1)
		  {
		    used[i] = list.selected + 1;
		    used[i + 1] = -1;
		    break;
		  }
	      }

	    done += list.selected + 1;
	    output_buffer[buffer_length] = ' ';
	    strcpy(output_buffer + buffer_length + 1, sel);
	    buffer_change = 1;
	  }
	  break;

	case 127:
	case KEY_BACKSPACE:
	  {
	    char * delete_from = output_buffer;
	    for(int i = 0; output_buffer[i] != '\0'; i++)
	      {
		if(output_buffer[i] == ' ')
		  {
		    delete_from = &output_buffer[i];
		  }
	      }

	    for(int i = 0; i < 256; i++)
	      {
		if(used[i] == -1)
		  {
		    if(i == 0)
		      break;

		    done -= used[i - 1];
		    used[i - 1] = -1;
		    break;
		  }
	      }

	    *delete_from = 0;
	    buffer_change = 1;
	  }
	}

      if(list.selected < 0)
	{
	  list.selected += list.n;
	}
      else if(list.selected >= list.n)
	{
	  list.selected -= list.n;
	}

      if(selected)
	{
	  if(selected->selected < 0)
	    {
	      selected->selected += selected->n;
	    }
	  else if(selected->selected >= selected->n)
	    {
	      selected->selected -= selected->n;
	    }
	}

      if(buffer_change)
	{
	  free_number_list(&list);
	  init_number_list(&list, phonetics, number + done);
	}

      clear();
      printw("%s", number + done);
      draw_number_list(&list);
      printw("\n%s%*c", output_buffer, 100, ' ');
      find_interesting(phonetics, number + done);
      refresh();
    }

  endwin();

  FILE * file;
  if(output)
    {
      file = fopen(output, "w");
    }
  else
    {
      file = stdout;
    }

  fprintf(file, "%s%s\n", output_buffer, (*(number + done)) ? " # INCOMPLETE" : "");

  if(output)
    {
      fclose(file);
    }


  free_number_list(&list);
  free(output_buffer);
  return 0;
}
