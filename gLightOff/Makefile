include ../config.mk

APP  = glightoff
# ls *.c | sed "s%\.c%.o%g" | tr "\n" " " >> Makefile
OBJS =  glightoff.o

LDFLAGS +=  $(GLIGHTOFF_LIBS) $(INTLLIBS)
#CPPFLAGS += -I..
CFLAGS += -I.. $(GTK_CFLAGS)
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
	mkdir -p $(DESTDIR)$(datadir)/applications
	$(INSTALL_DATA) data/glightoff.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	$(INSTALL_DATA) data/glightoff.jpg $(DESTDIR)$(datadir)/pixmaps
	mkdir -p $(DESTDIR)$(datadir)/pixmaps/glightoff
	$(INSTALL_DATA) data/bulb_on.svg $(DESTDIR)$(datadir)/pixmaps/glightoff
	$(INSTALL_DATA) data/bulb_off.svg $(DESTDIR)$(datadir)/pixmaps/glightoff

uninstall-data:
	rm -f $(DESTDIR)$(datadir)/applications/glightoff.desktop
	rm -f $(DESTDIR)$(datadir)/pixmaps/glightoff.jpg
	rm -rf $(DESTDIR)$(datadir)/pixmaps/glightoff

