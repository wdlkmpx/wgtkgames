## GtkTetris

A simple Tetris clone for the GTK library.

## Requirements

- GTK 2 >= 2.14
- GTK 3 >= 3.14

## Authors (from the AUTHORS file)

- Mattias Wadman (Original author)
- Iavor Veltchev <i_veltchev@yahoo.com> \
    Improved user interface with: \
    GTK+ 2.x (gtktetris-0.6.2) \
    GTK+ 1.2 (gtktetris-0.6.1) \
    Common highscore file with more information in it

- David Necas <yeti@monoceros.physics.muni.cz> \
    Level speeds tuneing script \
    Suggested sgid games insted of suid games when installing binary

- Roel <roel_v@gmx.net> \
    Hide/Show next block \
    New score system (like on Gameboy)

- Jonathan Coles <jpcoles@mindless.com> \
    Drop block button \
    Various code improvments

- Kevin Glazier <striker99@mediaone.net>
    Suggested start at level and start with blocks at bottom option.

## Old NEWS

Version 0.6.2b (March 2013) (Iavor Veltchev)
- code cleanup in compliance with new compilers (still GTK+ 2.x)
- icon added

Version 0.6.2 (June 2006) (Iavor Veltchev)
- GTK+ 2.x interface

Version 0.5.x to 0.6.1 (Aug 23 2002) (Iavor Veltchev)
- Improved user interface!
- every user has his/her own settings file: $HOME/.gtktetris
- the highscore file records user name, score, level, and lines
- three block sizes, set in the Makefile 
	(SIZE = -DBIGBLOCKS, -DBIGGERBLOCKS, or NULL)
- corrections in the help window (left 'a' and right 'd' were switched)

Version 0.4 to 0.5 (Sep 10 2000)
- Install binary as sgid games insted of suid games
- Level speeds tuneing
- More drop bonus on higher levels
- Code clean-up, fixed typos
- Improved highscore table
- Makefile does not longer overwrite existing highscore file

Version 0.3 to 0.4 (Aug 8 2000)
- Fixed bug that made check lines function miss full rows sometimes
- Major code clean-up
- Fixed all warnings when compiling with -Wall
- Changed Makefile to install binary as suid games insted of root
- Changed default install path to /usr/local/bin and /usr/local/share/gtktetris
  for highscore file
- Added uninstall to makefile
- Changed the README a bit

Version 0.2 to 0.3 (Feb 23 2000)
- Drop block button
- Hide/Show next block
- New score system (like on game boy)
- Smoother row remover
- New help text
- New highscore table
- Added THANKS file

Version 0.1 to 0.2 (Feb 3 2000)
- Added big blocks option in the Makefile for people running at high resolutions
- Code clean-up in interface.c and highscore.c
- Added gimme-some-random-blocks-at-start option
- Added start level option
- Fixed the help text
- Fixed that the blocks always starts at top
