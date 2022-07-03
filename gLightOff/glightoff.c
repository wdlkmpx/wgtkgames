/*
 * Copyright (C) 2004 Paolo Borelli
 *
 * This game is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 * USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#undef PACKAGE_NAME
#define PACKAGE_NAME "gLighOff"

#include "gtkcompat.h"

#include <stdlib.h>
#include <string.h>

#ifdef ENABLE_NLS
#include <locale.h>
#include <libintl.h>
#define _(x) gettext(x)
#define N_(x) (x)
#else
#define _(x)  (x)
#define N_(x) (x)
#endif

#include "levels.h"

#define BOARD_SIZE 5

#define GLIGHTOFF_LOCALEDIR PACKAGE_LOCALEDIR
#define GLIGHTOFF_GFXDIR    PACKAGE_DATADIR "/pixmaps/glightoff"
#define GLIGHTOFF_ICONDIR   PACKAGE_DATADIR "/pixmaps"

/* hardcoded for now... */
#define DEFAULT_BACKGROUND_COLOR "#7590BE"
#define DEFAULT_CURSOR_COLOR     "#CCCCCC"

const char *greeting_msg = N_("Turn off all the lights to win!");


typedef struct _GLightOffApp GLightOffApp;

struct _GLightOffApp
{
    GtkWidget *main_window;
    GtkWidget *vbox;
    GtkWidget *draw_area;
    GtkWidget *statusbar;

    int board[BOARD_SIZE][BOARD_SIZE]; /* the 5x5 game board,
                                        * 0 if the cell is OFF
                                        * 1 if it is ON */
    guint cur_level;
    guint move_count;
    /* keyboard nav */
    gboolean show_cursor;
    int cursor_x, cursor_y;
};

static GLightOffApp *app = NULL;

/* raw pixbufs */
static GdkPixbuf *light_on_pb_raw = NULL;
static GdkPixbuf *light_off_pb_raw = NULL;

/* scaled pixbufs */
static GdkPixbuf *light_on_pb = NULL;
static GdkPixbuf *light_off_pb = NULL;

/* the buffer we draw in */
static GdkPixmap *buffer_pixmap = NULL;


/* if level = 0, set a random level */
static void reset_game (int level)
{
    if (!LEVEL_IS_VALID (level))
        level = g_random_int_range (1, N_LEVELS);

    memcpy (app->board, levels[level], BOARD_SIZE * BOARD_SIZE * sizeof(int));

    app->cur_level = level;
    app->move_count = 0;

    /* init keynav: cursor centered and hidden */
    app->show_cursor = FALSE;
    app->cursor_x = 2;
    app->cursor_y = 2;
}


static void draw_cursor (int cellsize)
{
    static GdkGC *cursor_gc;

    if (! app->show_cursor)
        return;

    if (!cursor_gc)
    {
        GdkColormap *cmap;
        GdkColor color;

        cursor_gc = gdk_gc_new (app->draw_area->window);
        gdk_color_parse (DEFAULT_CURSOR_COLOR, &color);
        cmap = gtk_widget_get_colormap (app->draw_area);
        gdk_colormap_alloc_color (cmap, &color, FALSE, TRUE);
        gdk_gc_set_foreground (cursor_gc, &color);
    }

    gdk_draw_arc (buffer_pixmap,
                  cursor_gc,
                  FALSE,
                  app->cursor_x * cellsize + cellsize / 10,
                  app->cursor_y * cellsize + cellsize / 10,
                  cellsize * 0.8,
                  cellsize * 0.8,
                  0, 64 * 360);
}


static void draw_lights ()
{
    int i, j;
    int cellsize;
    static GdkGC *bgnd_gc;

    cellsize = gtk_widget_get_allocated_width(app->draw_area) / BOARD_SIZE;

    if (!bgnd_gc)
    {
        GdkColormap *cmap;
        GdkColor color;

        bgnd_gc = gdk_gc_new (app->draw_area->window);
        gdk_color_parse (DEFAULT_BACKGROUND_COLOR, &color);
        cmap = gtk_widget_get_colormap (app->draw_area);
        gdk_colormap_alloc_color (cmap, &color, FALSE, TRUE);
        gdk_gc_set_foreground (bgnd_gc, &color);
    }

    /* redraw the background.
     * The lights have alpha so we must redraw the background.
     * It also clears the cursor.
     */
    gdk_draw_rectangle (buffer_pixmap,
                        bgnd_gc,
                        TRUE,
                        0, 0,
                        gtk_widget_get_allocated_width (app->draw_area),
                        gtk_widget_get_allocated_height (app->draw_area));

    draw_cursor (cellsize);

    /* draw the lights */
    for (i = 0; i < BOARD_SIZE; i++)
    {
        for (j = 0; j < BOARD_SIZE; j++)
        {
            gdk_draw_pixbuf (buffer_pixmap,
                             NULL,
                             app->board[i][j] ? light_on_pb : light_off_pb,
                             0, 0,
                             i * cellsize, j * cellsize,
                             -1, -1,
                             GDK_RGB_DITHER_NORMAL, 0, 0); 
        }
    }
}


static void redraw_all (void)
{
    draw_lights ();
    gtk_widget_queue_draw (app->draw_area);
}


static gboolean check_win (void)
{
    int i, j;

    for (i = 0; i < BOARD_SIZE; i++)
        for (j = 0; j < BOARD_SIZE; j++)
            if (app->board[i][j])
                return FALSE;

    return TRUE;
}


static void winner_dialog ()
{
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new (GTK_WINDOW (app->main_window),
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_INFO,
                                     GTK_BUTTONS_NONE,
                                     _("You won!"));
    gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
                                              _("You completed level %d in %d moves."),
                                              app->cur_level, app->move_count);

    gtk_dialog_add_buttons (GTK_DIALOG (dialog),
                            _("New Game"), GTK_RESPONSE_ACCEPT,
                            GTK_STOCK_QUIT, GTK_RESPONSE_REJECT,
                            NULL);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        reset_game (0);
        redraw_all ();

        gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), 0, _(greeting_msg));
    }
    else
    {
        gtk_main_quit ();
    }

    gtk_widget_destroy (dialog);
}


static void toggle_cell (int x, int y)
{
    char *str;
    g_return_if_fail (x >= 0 && x < BOARD_SIZE);
    g_return_if_fail (y >= 0 && y < BOARD_SIZE);

    app->board[x][y] = app->board[x][y] ? 0 : 1;
    if (x > 0) app->board[x - 1][y] = app->board[x - 1][y] ? 0 : 1;
    if (x < 4) app->board[x + 1][y] = app->board[x + 1][y] ? 0 : 1;
    if (y > 0) app->board[x][y -1] = app->board[x][y - 1] ? 0 : 1;
    if (y < 4) app->board[x][y + 1] = app->board[x][y + 1] ? 0 : 1;

    app->move_count++;

    gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), 0);
    str = g_strdup_printf (_("Moves: %d"), app->move_count);
    gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), 0, str);
    g_free (str);

    redraw_all ();
    if (check_win ())
        winner_dialog ();
}


static gboolean button_press_event_cb (GtkWidget *w, GdkEventButton *e, gpointer data)
{
    int x, y;
    int i, j;
    int cellsize;

    /* Ignore the 2BUTTON and 3BUTTON events. */
    if (e->type != GDK_BUTTON_PRESS)
        return TRUE;

    gtk_widget_get_pointer (w, &x, &y);

    cellsize = gtk_widget_get_allocated_width(w) / BOARD_SIZE;
    i = x / cellsize;
    j = y / cellsize;

    /* hide keynav cursor */
    app->show_cursor = FALSE;
    app->cursor_x = i;
    app->cursor_y = j;

    toggle_cell (i, j);

    return TRUE;
}


static void move_cursor (int dx, int dy)
{
    app->cursor_x += dx;
    app->cursor_y += dy;

    if (app->cursor_x < 0) app->cursor_x = 0;
    if (app->cursor_y < 0) app->cursor_y = 0;
    if (app->cursor_x > 4) app->cursor_x = 4;
    if (app->cursor_y > 4) app->cursor_y = 4;

    redraw_all ();
}


static gboolean key_press_event_cb (GtkWidget *w, GdkEventKey *e, gpointer data)
{
    app->show_cursor = TRUE;

    switch (e->keyval)
    {
        case GDK_Left:
        case GDK_KP_Left:
            move_cursor (-1, 0);
            break;
        case GDK_Right:
        case GDK_KP_Right:
            move_cursor (1, 0);
            break;
        case GDK_Up:
        case GDK_KP_Up:
            move_cursor (0, -1);
            break;
        case GDK_Down:
        case GDK_KP_Down:
            move_cursor (0, 1);
            break;
        case GDK_space:
        case GDK_Return:
        case GDK_KP_Enter:
            if (app->show_cursor)
                toggle_cell (app->cursor_x, app->cursor_y);
            break;
        default:
            redraw_all ();
    }

    return TRUE;
}


static gint configure_event_cb (GtkWidget *w, GdkEventConfigure *e, gpointer data)
{
    int cellsize;

    cellsize = gtk_widget_get_allocated_width(w) / BOARD_SIZE;

    /* create the buffer pixmap of the proper size */
    if (buffer_pixmap) {
        g_object_unref (buffer_pixmap);
    }
    buffer_pixmap = gdk_pixmap_new (gtk_widget_get_window (w),
                                    gtk_widget_get_allocated_width (w),
                                    gtk_widget_get_allocated_height (w),
                                    -1);
    /* scale the pixbufs */
    if (light_on_pb)  g_object_unref (light_on_pb);
    if (light_off_pb) g_object_unref (light_off_pb);

    light_on_pb = gdk_pixbuf_scale_simple (light_on_pb_raw,
                                           cellsize, cellsize, 
                                           GDK_INTERP_BILINEAR);
    light_off_pb = gdk_pixbuf_scale_simple (light_off_pb_raw,
                                            cellsize, cellsize, 
                                            GDK_INTERP_BILINEAR);
    draw_lights ();
    return TRUE;
}


static gboolean expose_event_cb (GtkWidget *w, GdkEventExpose *e, gpointer data)
{
    gdk_draw_drawable (gtk_widget_get_window (w),
                       w->style->fg_gc[gtk_widget_get_state (w)],
                       buffer_pixmap,
                       e->area.x, e->area.y,
                       e->area.x, e->area.y,
                       e->area.width, e->area.height);
    return FALSE;
}


static void new_menu_cb (GtkAction *action, gpointer data)
{
    reset_game (0);
    redraw_all ();
    gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), 0,
                        _(greeting_msg));
}


static void restart_menu_cb (GtkAction *action, gpointer data)
{
    reset_game (app->cur_level);
    redraw_all ();
    gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), 0,
                        _(greeting_msg));
}


static void quit_menu_cb (GtkAction *action, gpointer data)
{
    gtk_main_quit ();
}


static void about_menu_cb (GtkAction *action, gpointer data)
{
    GdkPixbuf *logo;

    const gchar *authors[] = {
        "Paolo Borelli <pborelli@katamail.com>",
        NULL
    };

    const gchar *copyright = "Copyright \xc2\xa9 2004 Paolo Borelli";

    /* Translators: add your name, so credits appear in the "about" dialog */
    const gchar *translator_credits = _("translator-credits");

    logo = gdk_pixbuf_new_from_file (GLIGHTOFF_ICONDIR"/glightoff.jpg", NULL);

    gtk_show_about_dialog (GTK_WINDOW (app->main_window),
                           "program-name", PACKAGE_NAME,
                           "version", VERSION,
                           "copyright", copyright,
                           "authors", authors,
                           "license", "GPL2",
                           "translator_credits", translator_credits,
                           "logo", logo,
                           "website", PACKAGE_URL,
                           "comments", "GLightOff is a simple (but not so easy to solve!) puzzle game. The goal is turning off all the lights on the board.",
                           NULL);
    g_object_unref (logo);
}


static GtkActionEntry entries[] = {
    { "FileMenu", NULL, N_("_File") },
/*  { "PreferencesMenu", NULL, N_("_Preferences") }, */
    { "HelpMenu", NULL, N_("_Help") },
    { "New", GTK_STOCK_NEW,
        N_("_New Game"), "<control>N",
        N_("Play New Game"),
        G_CALLBACK (new_menu_cb) },
    { "Restart", GTK_STOCK_REFRESH,
        N_("_Restart"), NULL,
        N_("Restart Current Game"),
        G_CALLBACK (restart_menu_cb) },    
    { "Quit", GTK_STOCK_QUIT,
        N_("_Quit"), "<control>Q",
        N_("Quit"),
        G_CALLBACK (quit_menu_cb) },
/*  { "Preferences", GTK_STOCK_PREFERENCES,
        N_("_Preferences"), NULL,
        N_("Preferences"),
        G_CALLBACK (preferences_menu_cb) }, */
    { "About", GTK_STOCK_ABOUT,
        N_("_About"), NULL,
        N_("About"),
        G_CALLBACK (about_menu_cb) },
};

static guint n_entries = G_N_ELEMENTS (entries);

static const gchar *ui_info = 
"<ui>"
"  <menubar name='MenuBar'>"
"    <menu action='FileMenu'>"
"      <menuitem action='New'/>"
"      <menuitem action='Restart'/>"
"      <separator/>"
"      <menuitem action='Quit'/>"
"    </menu>"
#if 0
"    <menu action='PreferencesMenu'>"
"      <menuitem action='Preferences'/>"
"    </menu>"
#endif
"    <menu action='HelpMenu'>"
"      <menuitem action='About'/>"
"    </menu>"
"  </menubar>"
"</ui>";


static gboolean quit_cb (GtkWidget *w, GdkEventAny *e, gpointer data)
{
    gtk_main_quit ();
    return TRUE;
}


static void create_gui (void)
{
    GtkUIManager *ui;
    GtkActionGroup *actions;
    GtkWidget *aspect_frame;

    /* main window */
    app->main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (app->main_window), 450, 500);

    /* set a min size to avoid pathological gtk+ behavior */
    gtk_widget_set_size_request (GTK_WIDGET (app->main_window), 190, 240);
    gtk_window_set_title (GTK_WINDOW (app->main_window), "GLightOff");
    gtk_window_set_default_icon_from_file (GLIGHTOFF_ICONDIR"/glightoff.jpg", NULL);

    g_signal_connect (G_OBJECT (app->main_window), "delete_event",
                      G_CALLBACK (quit_cb), NULL);

    app->vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (app->main_window), app->vbox);

    /* menu bar */
    actions = gtk_action_group_new ("Actions");
    gtk_action_group_set_translation_domain (actions, NULL);
    gtk_action_group_add_actions (actions, entries, n_entries, NULL);

    ui = gtk_ui_manager_new ();
    gtk_ui_manager_insert_action_group (ui, actions, 0);
    gtk_window_add_accel_group (GTK_WINDOW (app->main_window), 
                                gtk_ui_manager_get_accel_group (ui));

    if (!gtk_ui_manager_add_ui_from_string (ui, ui_info, -1, NULL))
    {
        g_message ("building menus failed");
    }

    gtk_box_pack_start (GTK_BOX (app->vbox),
                        gtk_ui_manager_get_widget (ui, "/MenuBar"),
                        FALSE, FALSE, 0);
    /* the game board */
    aspect_frame = gtk_aspect_frame_new (NULL, 0.5, 0.5, 1, FALSE);
    gtk_frame_set_shadow_type (GTK_FRAME (aspect_frame), GTK_SHADOW_NONE);
    gtk_box_pack_start (GTK_BOX (app->vbox),
                        aspect_frame, TRUE, TRUE, 0);
    app->draw_area = gtk_drawing_area_new ();
    gtk_container_add (GTK_CONTAINER (aspect_frame), app->draw_area);

    g_signal_connect (G_OBJECT (app->draw_area), "configure_event",
                      G_CALLBACK (configure_event_cb), NULL);
    g_signal_connect (G_OBJECT (app->draw_area), "button_press_event",
                      G_CALLBACK (button_press_event_cb), NULL);
    g_signal_connect (G_OBJECT (app->draw_area), "key_press_event",
                      G_CALLBACK (key_press_event_cb), NULL);
    g_signal_connect (G_OBJECT (app->draw_area), "expose_event",
                      G_CALLBACK (expose_event_cb), NULL);

    gtk_widget_set_events (app->draw_area,
                           gtk_widget_get_events (app->draw_area) |
                           GDK_BUTTON_PRESS_MASK |
                           GDK_KEY_PRESS_MASK);
    GTK_WIDGET_SET_FLAGS (app->draw_area, GTK_CAN_FOCUS);
    gtk_widget_grab_focus (app->draw_area);

    /* statusbar */
    app->statusbar = gtk_statusbar_new ();
    gtk_box_pack_end (GTK_BOX (app->vbox),
                      app->statusbar, FALSE, TRUE, 0);

    gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), 0,
                        _(greeting_msg));

    gtk_widget_show_all (app->main_window);
}


static gboolean load_image (const char *filename, GdkPixbuf **pixbuf)
{
    GdkPixbuf *tmp;

    /* FIXME: use GError and display the error details... */
    tmp = gdk_pixbuf_new_from_file (filename, NULL);
    if (!tmp)
    {
        char *message;
        GtkWidget *dialog;

        message = g_strdup_printf (_("Could not find image file:\n%s\n\n"
                                    "Please check GLightOff is installed correctly."),
                                    filename);
        dialog = gtk_message_dialog_new (GTK_WINDOW (app->main_window),
                                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_OK,
                                        message,
                                        NULL);
        gtk_dialog_run (GTK_DIALOG (dialog));
        g_free (message);

        return FALSE;
    }

    if (*pixbuf) {
        g_object_unref (*pixbuf);
    }
    *pixbuf = tmp;
	return TRUE;
}


static gboolean init_pixbufs (void)
{
    if (!load_image (GLIGHTOFF_GFXDIR"/bulb_on.svg", &light_on_pb_raw)) {
        return FALSE;
    }
    if (!load_image (GLIGHTOFF_GFXDIR"/bulb_off.svg", &light_off_pb_raw)) {
        return FALSE;
    }
    return TRUE;
}


int main (int argc, char *argv[])
{
#ifdef ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, GLIGHTOFF_LOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);
#endif
    g_set_application_name ("GLightOff");

    gtk_init (&argc, &argv);

    app = g_new0 (GLightOffApp, 1);

    if (!init_pixbufs ()) {
        return -1;
    }
    reset_game (0);

    create_gui ();

    gtk_main ();

    return 0;
}

