PREFIX = /usr/local

CFLAGS += -pedantic -Wall -Wextra
LDLIBS = -lxcb -lX11

# Uncomment to compile without X11 support
# CFLAGS += -DNO_X
# LDLIBS =

MOD =\
	modules/bspwm\
	modules/command\
	modules/text\
	modules/backlight

all: statusbar

debug: CFLAGS += -g
debug: statusbar

.c.o:
	$(CC) -o $@ -c $(CFLAGS) $<

statusbar.o: config.h

config.h:
	cp config.def.h $@

statusbar: statusbar.o $(MOD:=.o)
	$(CC) -o $@ $(MOD:=.o) statusbar.o $(LDLIBS) $(CFLAGS)

clean:
	rm -f statusbar statusbar.o $(MOD:=.o)

install: statusbar
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f statusbar $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/statusbar

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/statusbar

.PHONY: all debug clean install uninstall
