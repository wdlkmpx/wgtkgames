
# WGtkGames (forked apps)

- gFifteen (https://gentoo.osuosl.org/distfiles/1a/gfifteen-1.0.3.tar.gz)
- gLightOff (https://sourceforge.net/projects/glightoff/files/glightoff/1.0.0/glightoff-1.0.0.tar.gz)
- GtkBalls (https://github.com/emorozov/gtkballs)
- GtkEmeraldia (https://www.ibiblio.org/pub/Linux/games/tetris/xemeraldia-0.4.3.tar.gz)
- GtkTetColor (https://www.ibiblio.org/pub/Linux/games/gtktetcolor-0.6.4.tar.gz)
- GtkTetris (https://github.com/wader/gtktetris)

# LICENSE:

- gFifteen:     GPL3
- gLightOff:    GPL2
- GtkBalls:     GPL2
- GtkEmeraldia: custom (xemeraldia)
- GtkTetColor:  GPL2
- GtkTetris:    MIT

## Installation

Run

- ./configure --prefix=/usr
- make
- make install

By default the GTK2 port is compiled, use `--enable-gtk3` to build the GTK3 port
(this may disable some apps that only work with gtk2)

- ./configure --prefix=/usr --enable-gtk3
