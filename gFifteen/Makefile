include ../config.mk

APP  = gfifteen
# ls *.c | sed "s%\.c%.o%g" | tr "\n" " " >> Makefile
OBJS =  gfifteen.o

LDFLAGS +=  $(GLIGHTOFF_LIBS) $(INTLLIBS)
#CPPFLAGS += -I..
CFLAGS += -std=c99 -I.. $(GTK_CFLAGS)
LIBS   += $(GTK_LIBS)

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: all
	$(STRIP) $(APP)

distclean: clean

clean:
	rm -f *.o *~ $(APP) $(APP).exe

install: all install-data
	mkdir -p $(DESTDIR)$(bindir)
	install -c $(APP) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall: uninstall-data
	rm -f $(DESTDIR)$(bindir)/$(APP)

# ==================================================================

install-data:
	mkdir -p $(DESTDIR)$(mandir)/man6
	$(INSTALL_DATA) data/gfifteen.6 $(DESTDIR)$(mandir)/man6
	mkdir -p $(DESTDIR)$(datadir)/applications
	$(INSTALL_DATA) data/gfifteen.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	$(INSTALL_DATA) data/gfifteen.svg $(DESTDIR)$(datadir)/pixmaps
	mkdir -p $(DESTDIR)$(datadir)/pixmaps/gfifteen
	$(INSTALL_DATA) data/*.png $(DESTDIR)$(datadir)/pixmaps/gfifteen

uninstall-data:
	rm -f $(DESTDIR)$(mandir)/man6/gfifteen.6
	rm -f $(DESTDIR)$(datadir)/applications/gfifteen.desktop
	rm -f $(DESTDIR)$(datadir)/pixmaps/gfifteen.svg
	rm -rf $(DESTDIR)$(datadir)/pixmaps/gfifteen

