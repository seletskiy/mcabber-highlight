# mcabber-highlight Makefile

TARGETS = all clean install

PKGNAME = highlight
VERSION = 0.0.1

PLUGIN  = lib$(PKGNAME).so
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))

CFLAGS  += -std=c99 -fPIC -DMODULES_ENABLE
LDFLAGS += -shared

HAVE_MCABBER = $(shell pkg-config --exists mcabber; echo $$?)

ifneq ($(HAVE_MCABBER), 0)
	$(error "Requires mcabber development libraries to build.")
else
    CFLAGS  += $(shell pkg-config --cflags mcabber)
    LDFLAGS += $(shell pkg-config --libs mcabber)
endif

COMPILE ?= $(CC)
INSTALL ?= install -D
REMOVE  ?= rm -rf

DESTDIR ?=
PREFIX  ?= $(DESTDIR)/usr/local
PLUGDIR ?= $(PREFIX)/lib/mcabber

.PHONY: $(TARGETS)

all: $(PLUGIN)

$(PLUGIN): $(OBJECTS)
	$(COMPILE) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(COMPILE) $(CFLAGS) -o $@ -c $<

clean:
	$(REMOVE) $(OBJECTS) $(PLUGIN)

install: $(PLUGIN)
	$(INSTALL) -m 0744 $(PLUGIN) $(PLUGDIR)/$(PLUGIN)

