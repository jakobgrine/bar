PREFIX = /usr/local

# CFLAGS += -std=c99 -pedantic -Wall -D_POSIX_C_SOURCE=2 #-D_XOPEN_SOURCE=500
CFLAGS += -pedantic -Wall # -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
LDLIBS = -lxcb

MOD =\
	modules/bspwm\
	modules/command\
	modules/text

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
