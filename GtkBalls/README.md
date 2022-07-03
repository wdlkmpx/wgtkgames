## GtkBalls

Clone of Lines - logic game about balls

License is GPL2, see COPYING

## Requirements

 - GTK 2 >= 2.14
 - GTK 3 >= 3.14

## Bugs

If you find a bug, tell me what you did (or fix it and send a patch).

## Development

You can participate in development of GtkBalls by sending patches or pull requests.

## Authors (from the AUTHORS file)

drF_ckoff <dfo@antex.ru>
Eugene Morozov <roshiajin@yahoo.com> - author of gtkballs

Theme authors:
Default    - Roxana Chernogolova <roxana@ericos.e-burg.ru>
klines     - Roman Razilov <Roman.Razilov@gmx.de>
5balls-Big - Paul Sherman <psherma1@rochester.rr.com>

Translations:
Polish - Przemys³aw Su³ek <pbs@linux.net.pl>
German - Hermann J. Beckers <beckers@st-oneline.de>
French - ObiTuarY <obituary@freshmeat.net>

Manual page:
Christoph Thompson <obituary@freshmeat.net>

## Old news...

New in GtkBalls 3.1.5
  - Make pointer highlighting parameters configurable
  - New theme from 5balls - 5balls-Big

New in GtkBalls 3.1.4
  - Polished and fixed gfx behavior (f.e. cell movement now works while ball
    is moving/destroying)
  - Migration to GtkUIManager
  - Scores now saved in utf-8 encoding (old scores will NOT be lost =)
  - Workaround stupid gtk+ bug in window resizing code

New in GtkBalls 3.1.3
  - Fixed critical bugs in pathfinding functions
  - New theme - "mascots"
  - Updates to auto*/gettext

New in GtkBalls 3.1.2
  - Fixed big bug that lead to sigsegv
  - Added saving/loading of keyboard accelerators

New in GtkBalls 3.1.1
  - Added ability to delete saved games from save/load game dialog
  - UI polishing
  - translations updates
  - small bug fixes
  - compilation fixes
  - gtkballs.spec fixes

New in GtkBalls 3.1.0
  - Now images in themes can be in any format that gdk-pixbuf understand
  - Game can be played with keyboard only (use arrows and space/enter)
  - Hilight cell "under the pointer" (for keyboard play)
  - Updated gtkballs(6x) man page
  - Death of score writer process now handled correctly (mostly)
  - compilation with gcc 3.x fixes
  - code reorganization (should be much readable now)
  - migration to autoconf 2.54/automake 1.6.3/gettext 0.11.5

New in GtkBalls 3.0.1
  - Optional "Time Limit" per move. (If you think, that this function works
    unexpected - tell me, why it should work otherwise...)
  - Removed world writable score file.
  - auto* fixes (PLD), cosmetic fixes, documentation fixes (Debian).

New in GtkBalls 3.0.0
  - Port to gtk2, bugfixes, code cleanups, ui polishing, etc...
  - Rules now stored in preferences file
  - Updated Polish translation

New in GtkBalls 2.2.0
  - Game rules now customizable. (i.e. you can set board size different
    than 9x9)
  - "Input name" dialog now works as expected
  - Save game file format changed to plain text.
  - Score file format changed. Though old format still supported.
  - Updated Polish translation

New in GtkBalls 2.1.1
  - Save/Load game
  - Option to turn off "disappearing" animation
  - Dont try to load 'Default' theme at startup twice if not found

New in GtkBalls 2.1
  - "Advanced" themes.

New in GtkBalls 2.0.1
 - Fixed the "Undo" bug: after a move resulting in a score growth, the "Undo"
   did not decrease the score.
 - New translations: Polish and German
 - Updated French translation. (But i dont know French, so review it
   if you know =)
 - Dialogs can be closed by hitting "ESC" key

New in GtkBalls 2.0
 - Undo (backport from 2.1 branch)
   (i got no bug reports for 2.0pre1 release, so i add "undo" option
   and release it as 2.0)

New in GtkBalls 2.0pre1
 - New maintainer (drF_ckoff)
 - i18n fixes
 - Removed almost all *set_usize()'s. Now all sizes
   is up to gtk+. Must look better on all combinations
   of themes/font sizes...
 - Beautyfied alot all dialogs
 - Some sort of keyboard control in dialogs (not yet
   finished)
 - "Theme" support. (see THEME)
 - New default theme + 2 extra themes
 - Removed dependence on libxml, now saves settings
   in plain text ~/.gtkballsrc
 - Not "user visible" changes: all sources now has
   "drF_ckoff'ish" indentation =)

New in GtkBalls 1.05pre1
 - Support for GNU autocong and GNU gettext
 - Minor changes to game rules
 - Improvements of user interface
