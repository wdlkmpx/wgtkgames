## GtkTetcolor

GtkTetcolor is reimplementation of tetcolor game made for DOS by 
S. Sotnikov on 1991. GtkTetcolor is a game that resembles well-known tetris
and columns. The rules of the game are simple - you can move or rotate
the block which continue to fall. After block landing the sequence of at
least three cells horizontally, vertically or diagonally having matching
colors is removed and the above blocks will collapse. If two or more
sequences will be removed simultaneously the player has got bonus
points. The game will be over when new block cannot be placed on screen. 

Original author: Andrey V. Panov <panov@canopus.iacp.dvo.ru>


## Requirements

- GTK+ 2/3


## Score accounting

Since I cannot decrypt formula of DOS tetcolor I use own:

   Score = Bonus + Line,

where Line is points for each deleted line and Bonus is added when
simultaneously more than 1 line disappears.

   Line = 40*(1 + (level - 1)*0.1)*(1 + 0.25*(5 - n)),

where n is number of cells in the line.   

            / (lines - 1)*500,           if 1 < lines <= 11,
	    |
   Bonus = <  5000 + 2500*(lines - 11),  if 11 < lines <= 17,
            |
	    \ 20000 + 5000*(lines - 17), if 17 < lines,

where lines is number of simultaneously destroyed lines.


## TODO

- add Tetcolor II rules (http://tetcolor.newmail.ru, link is broken)


## Old news

GtkTetcolor 0.6.4   (May 25 2004)

- fixed compilation by gcc-3.4
- updated intltool version which fixes configuring without OrigTree 
   perl module
- fixed loading of a current font into fontselection widget in the preferences 
   dialog

GtkTetcolor 0.6.3   (Mar 25 2004)

- ported to Gtk+-2.4

GtkTetcolor 0.6.2   (Mar 12 2003)

- added experimental rpm-spec-file
- German translation

GtkTetcolor 0.6.1   (Dec 19 2002)

- ported to win32
- minor bug and portability fixes

GtkTetcolor 0.6.0   (Nov 21 2002)

- fixed memory leak in changing of font
- code was cleaned up

GtkTetcolor 0.6.0pre1 (Nov 10 2002)

- ported to Gtk2/GNOME2

GtkTetcolor 0.5.2   (Mar 4 2002)

- source code was cleaned, possible memory leaks on old Gnome were fixed
 
GtkTetcolor 0.5.1   (Dec 6 2001)

- delay before cell destroying is now configurable
- score label is now displayed in various colors when current score lays in top ten
- bugfixes

GtkTetcolor 0.5.0   (Apr 9 2001)

- sound support for GNOME version
- scores saving done using GNOME routines, non-compatible with previous versions
- minor bugfixes

GtkTetcolor 0.4.3   (Aug 23 2000)

- fixed problem with visualization of left margin
- French translation

GtkTetcolor 0.4.2   (Aug 16 2000)

- timer 
- dialogs were gnomified (for GNOME version)
- bugfixes

GtkTetcolor 0.4.1   (Aug 11 2000)

- Font selection for labels 

GtkTetcolor 0.4.0   (Aug 4 2000)

- GNOME support (optional)
- changed formula for bonus points

GtkTetcolor 0.3.1   (Mar 28 2000)

- Bug with long players names (more than 255 characters) fixed 
 
GtkTetcolor 0.3     (Jan 27 2000)

- Removing of cells is slightly animated
- Made it possible to switch between numeric keypad and arrows keys
- Code cleanup

GtkTetcolor 0.2     (Nov 25 1999)

GtkTetcolor 0.1     (Jul 8 1999)

- First public release.
 