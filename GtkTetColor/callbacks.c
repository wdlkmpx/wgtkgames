/* GtkTetcolor
 * Copyright (C) 1999 Andrey V. Panov
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "main.h"
#include "callbacks.h"
#include "interface.h"
#include "score.h"
#include "game.h"
#include "preferences.h"
#include "pixmaps.h"

gchar *label_name[MAX_LABEL];	/* Label identifiers */
gboolean preferences_changed;


gboolean
on_main_window_delete_event (GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
    return FALSE;
}

void main_window_destroy_cb (GtkWidget * w, gpointer   user_data)
{
    app_exit ();
}

void on_new_activate (void)
{
    init_game (NULL);
}

void on_scores_activate (void)
{
    create_scores_dialog ();
}

void on_preferences_activate (void)
{
    create_preferences_dialog ();
}

void on_quit_activate (void)
{
    gtk_widget_destroy (main_window);
}

void help_on_keys_activate (void)
{
    create_help_dialog ();
}


gboolean
on_main_window_key_press_event (GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
    gint key = event->keyval;

    if (!timeout || !continue_game) {
        return FALSE;
    }

    switch (key)
    {
        case GDK_KEY(Right):
        case GDK_KEY(KP_Right):
        case GDK_KEY(KP_6):
            if (shift_block (1)) {
                redraw_cells ();
            }
            return TRUE;

        case GDK_KEY(Left):
        case GDK_KEY(KP_Left):
        case GDK_KEY(KP_4):
            if (shift_block (-1)) {
                redraw_cells ();
            }
            return TRUE;

        case GDK_KEY(Up):
        case GDK_KEY(KP_Up):
        case GDK_KEY(KP_8):
            if (block.type) {
                if (rotate_block (rotate_cell_left))
                    redraw_cells ();
            }
            return TRUE;

        case GDK_KEY(Down):
        case GDK_KEY(KP_Begin):
        case GDK_KEY(KP_5):
            if (block.type) {
                if (rotate_block (rotate_cell_right))
                    redraw_cells ();
            }
            return TRUE;

        case GDK_KEY(space):
            if (timeout) {
                g_source_remove (timeout);
                interval = INI_INTERVAL / 100;
                timeout = g_timeout_add (interval, (GSourceFunc) timeout_callback, main_window);
                return TRUE;
            }
            break;
    }

    return FALSE;
}


void on_name_response (GtkDialog * dlg, int response, gpointer user_data)
{
    GtkWidget *widget;
    gchar *str;

    widget = (GtkWidget *) g_object_get_data (G_OBJECT (dlg), "name_entry");
    str = g_strdup (gtk_entry_get_text (GTK_ENTRY (widget)));
    if (strlen (str) == 0) {
        str = g_strdup (g_get_real_name ());
        if (strlen (str) == 0) {
            str = g_strdup (g_get_user_name ());
        }
    }
    if (strlen(str) > 255){
        strncpy (new_name, str, 255);
        new_name[255] = '\0';
    } else {
        strcpy (new_name, str);
    }
    if (str) {
        g_free (str);
    }

    strcpy (name[name_i], new_name);
    saved_score[name_i] = score;
    write_score ();
    gtk_widget_destroy (GTK_WIDGET (dlg));
}


void on_pause_response (GtkDialog * dlg, int response, gpointer user_data)
{
    continue_game = 1;
    interval = INI_INTERVAL - INI_INTERVAL * (level - 1) / 10;
    timeout = g_timeout_add (interval, (GSourceFunc) timeout_callback, main_window);
    level_timeout = g_timeout_add (LEVEL_INT, (GSourceFunc) change_level, main_window);
    gtk_widget_destroy (GTK_WIDGET (dlg));
}

void on_pause_activate (void)
{
    if (continue_game) {
        create_pause_dialog ();
        if (timeout) {
            g_source_remove (timeout);
        }
        if (level_timeout) {
            g_source_remove (level_timeout);
        }
        continue_game = 0;
    }
}


void preferences_dialog_response (GtkDialog * dialog, gint response_id, gpointer data)
{
    switch (response_id)
    {
        case GTK_RESPONSE_OK:	/* OK */
            change_preferences (GTK_WIDGET (dialog));
            if (preferences_changed)
                save_preferences ();
            if (dialog)
                gtk_widget_destroy (GTK_WIDGET (dialog));
            break;
        case GTK_RESPONSE_APPLY:	/* APPLY */
            change_preferences (GTK_WIDGET (dialog));
            if (preferences_changed)
                save_preferences ();
            break;
        case GTK_RESPONSE_CLOSE:	/* CLOSE */
        case GTK_RESPONSE_DELETE_EVENT:
            if (dialog)
                gtk_widget_destroy (GTK_WIDGET (dialog));
            break;
    }
}


gboolean on_drawingarea_expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer user_data)
{
    redraw_all_cells ();
    return FALSE;
}

///#ifdef USE_GNOME
void on_sound_checkbutton_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    sound_on = !sound_on;
    preferences_changed = TRUE;
}
///#endif
