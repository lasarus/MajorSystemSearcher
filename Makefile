.PHONY : all clean

all: majorsearch dictionary.phon

CMUDICT := http://svn.code.sf.net/p/cmusphinx/code/trunk/cmudict/cmudict-0.7b

SOURCES := main.c parser.c phonetics.c gui.c
OBJECTS := $(addprefix obj/,$(SOURCES:=.o))

WARNINGS := -Wall -Wextra -Wno-experimental -Wpedantic
CFLAGS := $(WARNINGS) -g $(shell pkg-config --cflags ncurses)

CC := gcc
PYTHON := python2

obj/%.o:	src/%
	$(CC) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj

majorsearch: obj $(OBJECTS) Makefile
	mkdir -p obj
	$(CC) -o $@ $(CFLAGS) $(OBJECTS) $(shell pkg-config --libs ncurses)

cmudict:
	wget $(CMUDICT) -O cmudict

dictionary.phon: cmudict
	$(PYTHON) converter.py $< > $@

clean:
	rm -f majorsearch
	rm -f $(OBJECTS)
	rm -rf obj
	rm -f dictionary.phon
