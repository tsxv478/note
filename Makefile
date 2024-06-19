VERSION = 0.1
PREFIX = /usr/local
CC = cc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = --std=c99 -pedantic -Wall -Os
LDFLAGS  = -s -static

all: note

note: note.c

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f note $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/note

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/note

clean:
	rm -f note note-$(VERSION).tar.gz

dist: clean
	mkdir -p note-$(VERSION)
	cp -R TODO Makefile config.h note.c note-$(VERSION)
	tar -cf note-$(VERSION).tar note-$(VERSION)
	gzip note-$(VERSION).tar
	rm -rf note-$(VERSION)

.c:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $<

.PHONY: all install uninstall clean dist
