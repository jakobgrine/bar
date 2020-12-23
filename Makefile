PREFIX = /usr/local

CFLAGS += -pedantic -Wall -Wextra
LDLIBS = -lxcb

MOD =\
	modules/bspwm\
	modules/command\
	modules/text\
	modules/backlight

all: bar

debug: CFLAGS += -g
debug: bar

.c.o:
	$(CC) -o $@ -c $(CFLAGS) $<

bar.o: config.h

config.h:
	cp config.def.h $@

bar: bar.o $(MOD:=.o)
	$(CC) -o $@ $(MOD:=.o) bar.o $(LDLIBS) $(CFLAGS)

clean:
	rm -f bar bar.o $(MOD:=.o)

install: bar
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f bar $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/bar

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/bar

.PHONY: all debug clean install uninstall
