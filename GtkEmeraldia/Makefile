include ../config.mk

APP  = gtkemeraldia
# ls *.c | sed 's%\.c%.o%g'
OBJS = crack.o games.o graphics.o init-graphics.o main.o move.o score.o

CPPFLAGS += -I..

CFLAGS += $(GTK_CFLAGS)
LIBS   += $(GTK_LIBS)

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: $(APP)
	$(STRIP) $(APP)

distclean: clean

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
	install -c -m 644 data/gtkemeraldia.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	install -c -m 644 data/gtkemeraldia.png $(DESTDIR)$(datadir)/pixmaps

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/gtkemeraldia.desktop
	rm -f $(DESTDIR)$(datadir)/pixmaps/gtkemeraldia.png
