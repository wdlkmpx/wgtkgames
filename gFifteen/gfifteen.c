/*
  gFifteen - graphical implementation of the puzzle game fifteen
  Copyright (C) 2012  Christopher Howard
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// https://zetcode.com/gfx/cairo/

#include "config.h"
#include "gtkcompat.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#undef PACKAGE_NAME
#define PACKAGE_NAME "gFifteen"

static const char * about_authors[] = { "Christopher Howard <christopher.howard@frigidcode.com>", NULL };
static const char * about_comments = "gFifteen is a graphical implementation of the old puzzle game \"fifteen\"";
static const char * about_license = "GPL3";
static const char * about_copyright = "Copyright (C) 2012  Christopher Howard";

GtkWidget * gf_window;
GtkWidget * gf_bsquares_widget;
GtkWidget * gf_main_grid_widget;
GtkWidget * gf_statusbar_widget;

GtkWidget * gf_menu_bar_widget;

#define N_TILE_SETS 3

/* Board square graphic surfaces.
   Elements 1 through 15 are the graphics data for each of these numbers.
   I'm going to leave element 0 NULL for now and not use it, but perhaps I will
   in the future somehow. */
cairo_surface_t * gf_bsquare_surfaces[N_TILE_SETS][16] = { { NULL } };

int current_tile_set = 0;

/* Values of individual squares. 4x4 grid. Possible element values are 0-15
   where 0 represents empty square. Initialized to winning position. */
char gf_bsquare_values[16] = {
    1,   2,  3,  4,
    5,   6,  7,  8,
    9,  10, 11, 12,
    13, 14, 15,  0 };

const char gf_bsquare_win_state[16] = {
    1,   2,  3,  4,
    5,   6,  7,  8,
    9,  10, 11, 12,
    13, 14, 15,  0 };

#define GF_STATUSBAR_TB_SIZE 64
char gf_statusbar_textbuffer[GF_STATUSBAR_TB_SIZE];
int gf_moves_used = 0;
char gf_game_state = 1;
time_t gf_time_start;

static gboolean gf_bsquares_draw_callback (GtkWidget *widget, gpointer compat, gpointer data);
static void gf_bsquares_clicked_callback (GtkWidget * widget, GdkEventButton * event, gpointer data);

int gf_movsquare(int i);
void gf_randboard (int rand_bits);
int gf_wincheck (void);
void load_tiles();

static void gf_start_new_game ();

static void activate_action (GtkAction *action)
{
    // returned name is not supposed to be freed, according to gtk+ API
    const gchar * action_name = gtk_action_get_name (action);
    if (action_name[0] == 'N')
        gf_start_new_game();
    else if (action_name[0] == 'Q')
        gtk_main_quit();
    else if (action_name[0] == 'A')
    {
        gtk_show_about_dialog (NULL,
                               "authors", about_authors,
                               "license", about_license,
                               "comments", about_comments,
                               "copyright", about_copyright,
                               "website", PACKAGE_URL,
                               "program-name", PACKAGE_NAME,
                               "version", PACKAGE_VERSION,
                               NULL);
    }
}


enum {
    TILE_DEFAULT,
    TILE_WOOD,
    TILE_CHIP,
};

static void activate_radio_action (GtkAction *action, GtkRadioAction *current)
{
    switch (gtk_radio_action_get_current_value (current))
    {
        case TILE_DEFAULT: current_tile_set = 0; break;
        case TILE_WOOD:    current_tile_set = 1; break;
        case TILE_CHIP:    current_tile_set = 2; break;
    }
    gtk_widget_queue_draw (gf_bsquares_widget);
}


static GtkActionEntry entries[] =
{
    { "GameMenu", NULL, "_Game",  NULL, NULL, NULL },
    { "TileMenu", NULL, "_Tiles", NULL, NULL, NULL },
    { "HelpMenu", NULL, "_Help",  NULL, NULL, NULL },
    { "New",  GTK_STOCK_NEW, "_New", "<control>N", "Randomize the puzzle", G_CALLBACK (activate_action) },
    { "Quit", GTK_STOCK_QUIT,"_Quit", "<control>Q", "Quit",                G_CALLBACK (activate_action) },
    { "About", GTK_STOCK_ABOUT, "_About", NULL, "Show information about the software", G_CALLBACK (activate_action) }
};


static GtkRadioActionEntry tile_entries[] =
{
    { "TileDefault", NULL, "_Default", NULL, "Default tile set", TILE_DEFAULT },
    { "TileWood",    NULL, "_Wood",    NULL, "Wood tile set",    TILE_WOOD },
    { "TileChip",    NULL, "_Chip",    NULL, "Chip tile set",    TILE_CHIP },
};

static guint n_entries = G_N_ELEMENTS (entries);

static const gchar ui_definition[] = 
  "<ui>"
  "  <menubar name='MenuBar'>"
  "    <menu action='GameMenu'>"
  "      <menuitem action='New' />"
  "      <menuitem action='Quit' />"
  "    </menu>"
  "    <menu action='TileMenu'>"
  "      <menuitem action='TileDefault' />"
  "      <menuitem action='TileWood' />"
  "      <menuitem action='TileChip' />"
  "    </menu>"
  "    <menu action='HelpMenu'>"
  "      <menuitem action='About' />"
  "    </menu>"
  "  </menubar>"
  "</ui>";


int main (int argc, char ** argv)
{
    // GTK+ initialization
    gtk_init(&argc, &argv);

    // Prepare GUI manager and key accelerators
    GtkUIManager * ui_manager = gtk_ui_manager_new ();
    GtkActionGroup * actions = gtk_action_group_new ("Actions");
    gtk_action_group_add_actions (actions, entries, n_entries, NULL);
    gtk_action_group_add_radio_actions (actions,
                                        tile_entries, N_TILE_SETS,
                                        TILE_DEFAULT,
                                        G_CALLBACK (activate_radio_action),
                                        NULL);
    gtk_ui_manager_insert_action_group (ui_manager, actions, 0);

    GError * err = NULL;
    gtk_ui_manager_add_ui_from_string (ui_manager, ui_definition, -1, &err);

    if (err != NULL) {
        fprintf (stderr, "error: problem creating user interface: %s\n", err->message);
        g_error_free (err);
    }

    gf_menu_bar_widget = gtk_ui_manager_get_widget (ui_manager, "/MenuBar");

    // create the window and our widgets
    gf_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_resizable (GTK_WINDOW (gf_window), FALSE);
    gtk_window_set_title(GTK_WINDOW(gf_window), "gFifteen");

    gf_bsquares_widget = gtk_drawing_area_new ();
    gf_statusbar_widget = gtk_statusbar_new();
    gf_main_grid_widget = gtkcompat_grid_new(4,4);

    // add the key accelerators to the window
    gtk_window_add_accel_group (GTK_WINDOW (gf_window),
                                gtk_ui_manager_get_accel_group (ui_manager));

    // perform some board square drawable configuration
    gtk_widget_set_size_request (gf_bsquares_widget, 256, 256);

    gtk_widget_add_events (gf_bsquares_widget, GDK_BUTTON_PRESS_MASK);

    // initialize the status bar message
    gtk_statusbar_push (GTK_STATUSBAR (gf_statusbar_widget), 0, "The board has been randomized.");

    // make needed signal connections
    g_signal_connect (gf_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect (G_OBJECT (gf_bsquares_widget), GTKCOMPAT_DRAW_SIGNAL,
                      G_CALLBACK (gf_bsquares_draw_callback), NULL);
    g_signal_connect (G_OBJECT (gf_bsquares_widget), "button_press_event",
                      G_CALLBACK (gf_bsquares_clicked_callback), NULL);

    // arrange widgets in a grid
    gtk_grid_attach (GTK_GRID (gf_main_grid_widget), gf_menu_bar_widget, 0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (gf_main_grid_widget), gf_bsquares_widget, 0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (gf_main_grid_widget), gf_statusbar_widget, 0, 2, 1, 1);
    gtk_container_add (GTK_CONTAINER (gf_window), gf_main_grid_widget);

    // display all widgets
    gtk_widget_show_all (gf_window);

    // load image resources
    load_tiles ();

    // randomize the board
    time_t t = time(NULL);
    srand(t);
    for(int i=0; i<32; i++) gf_randboard(rand());

    // start the clock
    gf_time_start  = time(NULL);

    // enter the event loop
    gtk_main ();

    return 0;
}


static void gf_bsquares_clicked_callback (GtkWidget * widget,
                                          GdkEventButton * event,
                                          gpointer data)
{
    if (!gf_game_state) return;
    int ix = (int) event->x / 64; // portable?
    int iy = (int) event->y / 64; // portable?
    int i = 4*iy + ix;
    if (gf_movsquare (i))
    {
        gf_moves_used++;
        if (gf_wincheck())
        {
            gf_game_state = 0;
            gtk_statusbar_remove_all(GTK_STATUSBAR (gf_statusbar_widget), 0);
            time_t time_passed = time(NULL) - gf_time_start;
            ldiv_t completed_time = ldiv(time_passed, 60);
            snprintf (gf_statusbar_textbuffer, GF_STATUSBAR_TB_SIZE,
                      "Solved: %d moves in %ldmin %lds",
                      gf_moves_used,
                      completed_time.quot,
                      completed_time.rem);
            gtk_statusbar_push(GTK_STATUSBAR (gf_statusbar_widget), 0, gf_statusbar_textbuffer);
            gtk_widget_queue_draw (gf_bsquares_widget);
            return;
        }
        else
        {
            gtk_statusbar_remove_all(GTK_STATUSBAR (gf_statusbar_widget), 0);
            snprintf(gf_statusbar_textbuffer, GF_STATUSBAR_TB_SIZE, "Moves used: %d", gf_moves_used);
            gtk_statusbar_push(GTK_STATUSBAR (gf_statusbar_widget), 0, gf_statusbar_textbuffer);
        }
    }
    gtk_widget_queue_draw (gf_bsquares_widget);
}


gboolean gf_bsquares_draw_callback (GtkWidget *widget, gpointer compat, gpointer data)
{
#if GTK_CHECK_VERSION (3, 0, 0)
    cairo_t * cr = (cairo_t *) compat;
    //GdkRectangle rect;
    //GdkRectangle * area = &rect;
    //gdk_cairo_get_clip_rectangle (cr, area);
#else // gtk2
    //GdkEventExpose * event = (GdkEventExpose *) compat;
    cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (widget));
    //GdkRectangle * area = &(event->area);
#endif
    for(int r=0; r<4; r++)
    {
        for(int c=0; c<4; c++) {
            int i = 4*r + c;
            if(gf_bsquare_values[i]) {
                cairo_set_source_surface (cr, gf_bsquare_surfaces[current_tile_set][(int) gf_bsquare_values[i]], 64*c, 64*r);
                cairo_paint (cr);
            }
        }
    }
#if GTK_MAJOR_VERSION == 2
    cairo_destroy (cr);
#endif
  return FALSE;
}


int gf_movsquare (int i)
{
    int empty_i = 16;
    /* figure out index of empty square */
    for (int wrk_i = 0; empty_i == 16; wrk_i++)
        if (!gf_bsquare_values[wrk_i]) empty_i = wrk_i;
    /* can't move the empty square */
    if (i == empty_i) return 0;
    if (i+4 == empty_i) goto swap; // is square above empty?
    if (i-4 == empty_i) goto swap; // is square below empty?
    if (! (i-1 == empty_i)) goto right_check; // is square to left empty?
    if (i/4 == empty_i/4) goto swap; // make sure it is on same row
  right_check:
    if (! (i+1 == empty_i)) return 0; // is square to right empty
    if (! (i/4 == empty_i/4)) return 0; // make sure it is on same row
  swap:
    gf_bsquare_values[empty_i] = gf_bsquare_values[i];
    gf_bsquare_values[i] = 0;
    return 1;
}


/* sliding randomization algorithm. I THINK this is portable */
void gf_randboard (int rand_bits)
{
    int empty_i = 16;
    /* figure out index of empty square */
    for (int wrk_i = 0; empty_i == 16; wrk_i++)
        if (!gf_bsquare_values[wrk_i]) empty_i = wrk_i;
    /* I'm going to assume here at least a 16 bit int. RAND_MAX must be at least
        32767, according to C99. I want to ignore the high two bits in case those
        are constant due to negative representation system. */
    for (int i = 14; i >= 2; i-=2)
    {
        /* pull two random bits */
        rand_bits = rand_bits >> 2;
        switch(rand_bits & 0x3)
        {
            case 0x0: // try swap empty with square above
                if (empty_i >= 4) // empty must not be in top row
                {
                    gf_bsquare_values[empty_i] = gf_bsquare_values[empty_i-4];
                    gf_bsquare_values[empty_i-4] = 0;
                    empty_i = empty_i - 4;
                }
                break;
            case 0x1: // try swap empty with square below
                if (empty_i <= 11) // empty must not be in bottom row
                {
                    gf_bsquare_values[empty_i] = gf_bsquare_values[empty_i+4];
                    gf_bsquare_values[empty_i+4] = 0;
                    empty_i = empty_i + 4;
                }
                break;
            case 0x2: // try swap empty with square to left
                if (! (empty_i == 0 || // can't be in left column
                    empty_i == 4 ||
                    empty_i == 8 ||
                    empty_i == 12))
                {
                    gf_bsquare_values[empty_i] = gf_bsquare_values[empty_i-1];
                    gf_bsquare_values[empty_i-1] = 0;
                    empty_i--;
                }
                break;
            case 0x3: // try swap empty with square to right
                if (! (empty_i == 3  || // can't be in right column
                    empty_i == 7  ||
                    empty_i == 11 ||
                    empty_i == 15 ))
                {
                    gf_bsquare_values[empty_i] = gf_bsquare_values[empty_i+1];
                    gf_bsquare_values[empty_i+1] = 0;
                    empty_i++;
                }
                break;
        }
    }
}


int gf_wincheck (void)
{
    for (int i=0; i<16; i++) {
        if (gf_bsquare_win_state[i] != gf_bsquare_values[i]) return 0;
    }
    return 1;
}


static void gf_start_new_game ()
{
    /* randomize board */
    time_t t = time(NULL);
    srand(t);
    for(int i=0; i<32; i++) gf_randboard(rand());
    /* clean info bar */
    gtk_statusbar_remove_all(GTK_STATUSBAR (gf_statusbar_widget), 0);
    gtk_statusbar_push (GTK_STATUSBAR (gf_statusbar_widget), 0, "The board has been randomized.");
    /* restore state */
    gf_moves_used = 0;
    gf_game_state = 1;
    gf_time_start = time(NULL);
    /* redraw */
    gtk_widget_queue_draw (gf_bsquares_widget);
}


static void generic_cut (int index, cairo_surface_t * loaded_surface);

static cairo_surface_t *load_surface_from_png (const char *name)
{
    cairo_surface_t *loaded_surface;
    char * png_file = g_strconcat (PACKAGE_DATADIR, "/pixmaps/gfifteen/", name, NULL);
    if (access(png_file, F_OK) != 0) {
        free (png_file);
        // local dir (data/xx.png)
        png_file = g_strconcat ("data/", name, NULL);
    }
    ///puts (png_file); // debug
    loaded_surface = cairo_image_surface_create_from_png (png_file);
    if (cairo_surface_status(loaded_surface) == CAIRO_STATUS_FILE_NOT_FOUND) {
        g_error ("error: could not load file: %s\n", name);
    }
    free (png_file);
    return loaded_surface;
}


void load_tiles()
{
    cairo_t *cr;
    cairo_surface_t * loaded_surface;
    cairo_text_extents_t extents;
    int i, j;

    // create surface for all tileset
    for (i = 0; i < N_TILE_SETS; i++) {
        for (j = 1; j < 16; j++ ) { // 64x64
            gf_bsquare_surfaces[i][j] = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 64, 64);
        }
    }

    // tileset 0 is special, we'll create blocks
    /// gf_bsquare_surfaces[0][1] = load_surface_from_png ("bsquare01.png");
    char text[8];
    for (i = 1; i < 16; i++)
    {
        cr = cairo_create (gf_bsquare_surfaces[0][i]);
        // background
        cairo_set_source_rgb (cr, 0.1, 0.1, 0.1);
        cairo_paint (cr);
        // draw dividing line
        cairo_set_source_rgb (cr, 0.8, 0.8, 0.8);
        cairo_set_line_width (cr, 0.6);
        cairo_rectangle (cr, 0, 0, 64, 64);
        cairo_stroke (cr);
        // text
        cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
        cairo_set_font_size (cr, 24);
        snprintf (text, sizeof(text), "%d", i);
        cairo_text_extents (cr, text, &extents);          // center text (64x64)
        cairo_move_to (cr, 64/2 - extents.width/2, 64/2); // center text (64x64)
        cairo_show_text (cr, text);
        // cleanup
        cairo_destroy (cr);
    }

    // tileset 1
    loaded_surface = load_surface_from_png ("tile_wood.png");
    generic_cut (1, loaded_surface);

    // tileset 2
    loaded_surface = load_surface_from_png ("tile_chip.png");
    generic_cut (2, loaded_surface);
}


static void generic_cut (int index, cairo_surface_t * loaded_surface)
{
    cairo_t * cr;

    cr = cairo_create (gf_bsquare_surfaces[index][1]);
    cairo_set_source_surface (cr, loaded_surface, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][2]);
    cairo_set_source_surface (cr, loaded_surface, -64, 0);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][3]);
    cairo_set_source_surface (cr, loaded_surface, -128, 0);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][4]);
    cairo_set_source_surface (cr, loaded_surface, -192, 0);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][5]);
    cairo_set_source_surface (cr, loaded_surface, 0, -64);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][6]);
    cairo_set_source_surface (cr, loaded_surface, -64, -64);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][7]);
    cairo_set_source_surface (cr, loaded_surface, -128, -64);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][8]);
    cairo_set_source_surface (cr, loaded_surface, -192, -64);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][9]);
    cairo_set_source_surface (cr, loaded_surface, 0, -128);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][10]);
    cairo_set_source_surface (cr, loaded_surface, -64, -128);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][11]);
    cairo_set_source_surface (cr, loaded_surface, -128, -128);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][12]);
    cairo_set_source_surface (cr, loaded_surface, -192, -128);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][13]);
    cairo_set_source_surface (cr, loaded_surface, 0, -192);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][14]);
    cairo_set_source_surface (cr, loaded_surface, -64, -192);
    cairo_paint (cr);
    cairo_destroy (cr);

    cr = cairo_create (gf_bsquare_surfaces[index][15]);
    cairo_set_source_surface (cr, loaded_surface, -128, -192);
    cairo_paint (cr);
    cairo_destroy (cr);
}
