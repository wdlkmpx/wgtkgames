include ../config.mk

APP  = gtktetris
# ls *.c | sed 's%\.c%.o%g'
# remove interface_menu.o (included by interface.c)
OBJS = draw.o help.o highscore.o interface.o main.o options.o tetris.o

#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += $(GTK_CFLAGS)
LIBS   += $(GTK_LIBS)

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: $(APP)
	$(STRIP) $(APP)

clean:
	rm -f *.o *~ $(APP) $(APP).exe

distclean: clean

install: all install-data
	mkdir -p $(DESTDIR)$(bindir)
	install -c $(APP) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall: uninstall-data
	rm -f $(DESTDIR)$(bindir)/$(APP)

# ==================================================================

install-data:
	mkdir -p $(DESTDIR)$(datadir)/applications
	install -c -m 644 data/gtktetris.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	install -c -m 644 data/gtktetris.png $(DESTDIR)$(datadir)/pixmaps

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/gtktetris.desktop
	rm -f $(DESTDIR)$(datadir)/pixmaps/gtktetris.png

