include ../config.mk

APP  = gtktetcolor

# ls *.c | sed 's%\.c%.o%g'
# always remove: interface_menu.o  (it's included by interface.c) 
OBJS = callbacks.o game.o interface.o main.o pixmaps.o preferences.o score.o

CPPFLAGS += -I..

CFLAGS += $(GTK_CFLAGS)
LIBS   += $(GTK_LIBS)

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: $(APP)
	$(STRIP) $(APP)

distclean: mostlyclean

mostlyclean:
	rm -f *.o *~ $(APP) $(APP).exe

clean: mostlyclean

install: $(APP) install-data
	mkdir -p $(DESTDIR)$(bindir)
	install $(APP) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall: uninstall-data
	rm -f $(DESTDIR)$(bindir)/$(APP)


# ==================================================================

install-data:
	mkdir -p $(DESTDIR)$(datadir)/applications
	install -c -m 644 data/gtktetcolor.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	install -c -m 644 data/gtktetcolor.png $(DESTDIR)$(datadir)/pixmaps

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/gtktetcolor.desktop
	rm -f $(DESTDIR)$(datadir)/pixmaps/gtktetcolor.png

