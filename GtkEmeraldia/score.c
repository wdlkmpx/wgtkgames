/*                                                              */
/* score.c --  for gtkemeraldia                                   */
/*                                                              */
/* This file is copied from "xtetris" Version 2.5, and modified */

#include "games.h"
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

FILE *f_scores = NULL;
int errno_scores;

/* #define DEBUG */

#define HIGH_TABLE_SIZE 15      /* size of high score table */

static struct score_table
{
   char name[28];
   int score;
   int level;
   time_t date;
} high_scores[HIGH_TABLE_SIZE];


void  update_highscore_table ()
{
   int i, j;

   if (! app_data.usescorefile) return;
      read_high_scores ();

   /* Check for previous best score */
   for (i = 0; i < HIGH_TABLE_SIZE; i++) {
      if (strcmp (name, high_scores[i].name) == 0) {
         break;
      }
   }

   if (i < HIGH_TABLE_SIZE) {
      /* We have a previous best score. */
      if (high_scores[i].score >= sc) {
         return;                      /* Same/worse score - no update */
      }
      for (j = i; j > 0; j--) {        /* Remove previous best */
         high_scores[j] = high_scores[j - 1];
      }
   }

   /* Next line finds score greater than current one */
   for (i = 0; i < HIGH_TABLE_SIZE && sc >= high_scores[i].score; i++);
   i--;
   if (i >= 0)
   {
      for (j = 0; j < i; j++) {
         high_scores[j] = high_scores[j + 1];
      }
      strncpy (high_scores[i].name, name, sizeof(high_scores[i].name));
      high_scores[i].score = sc;
      high_scores[i].level = blocks / 20 + 1; 
      time(&(high_scores[i].date));
      write_high_scores ();
   }
}

void open_high_scores_file()
{
   f_scores = fopen (app_data.scorefile, "r+");
   if (!f_scores) {
      f_scores = fopen (app_data.scorefile, "w+");
   }
   if (!f_scores) {
      errno_scores = errno;
   }
}

void  read_high_scores()
{
   int i;

   if (! app_data.usescorefile) {
      return;
   }

   clearerr(f_scores);
   rewind(f_scores);

   for (i = 0; i < HIGH_TABLE_SIZE; i++)
   {
      struct score_table *score = &(high_scores[i]);
      if (feof(f_scores) || ferror(f_scores) || 4 != fscanf(f_scores, "%12[^,],%7d,%6d,%ld\n",
            score->name,
            &score->score,
            &score->level,
            &score->date)) 
      {
         strncpy (score->name, _("No name"), sizeof(score->name));
         score->date = -1;
         score->level = score->score = 0;
      }
   }
}


void  write_high_scores()
{
   int i;
  
   rewind(f_scores);
   if (ftruncate(fileno(f_scores), 0) != 0)
   {
      perror(_("write_high_scores:can't truncate scores file"));
      return;
   }
   for (i = 0; i < HIGH_TABLE_SIZE; i++)
   fprintf (f_scores, "%-12s,%7d,%6d,%ld\n",
            high_scores[i].name,
            high_scores[i].score,
            high_scores[i].level,
            high_scores[i].date);
            fflush(f_scores);
}


void  PrintHighScores ()
{
   GtkWidget *table, * vbox, * dialog;
   GtkListStore *model;
   GtkTreeIter iter;
   GtkCellRenderer *renderer;
   GtkTreeViewColumn *column;
   GtkTreeSelection *sel;
   int     i;
  
   if (! app_data.usescorefile) {
      return;
   }
   read_high_scores();
   dialog = gtk_dialog_new_with_buttons(_("gtkemeraldia's high scores"),
                                             GTK_WINDOW(topLevel),
                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                             "gtk-close", GTK_RESPONSE_ACCEPT,
                                             NULL);
   gtk_dialog_set_default_response (GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
   vbox = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

   model = gtk_list_store_new (4,
                               G_TYPE_STRING,
                               G_TYPE_INT,
                               G_TYPE_INT,
                               G_TYPE_STRING);

   for (i = HIGH_TABLE_SIZE-1; i >= 0; i--)
   {
      if (high_scores[i].score != 0)
      {
         char d[100], *dd;
         strftime (d, sizeof(d), "%c", localtime(&(high_scores[i].date)));
         dd = g_locale_to_utf8 (d, -1, NULL, NULL, NULL);
         gtk_list_store_append (model, &iter);
         gtk_list_store_set (model, &iter,
                             0, high_scores[i].name, 
                             1, high_scores[i].score, 
                             2, high_scores[i].level,
                             3, dd,
                             -1);
         g_free (dd);
      }
   }

   table = gtk_tree_view_new_with_model (GTK_TREE_MODEL(model));
   gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(table), TRUE);
   sel = gtk_tree_view_get_selection (GTK_TREE_VIEW(table));
   gtk_tree_selection_set_mode (sel, GTK_SELECTION_NONE);

   renderer = gtk_cell_renderer_text_new ();
   column = gtk_tree_view_column_new_with_attributes (_("Name"), renderer,
                                                      "text", 0, NULL);
   gtk_tree_view_append_column (GTK_TREE_VIEW (table), column);

   renderer = gtk_cell_renderer_text_new ();
   column = gtk_tree_view_column_new_with_attributes (_("Score"), renderer,
                                                      "text", 1, NULL);
   gtk_tree_view_append_column (GTK_TREE_VIEW (table), column);

   renderer = gtk_cell_renderer_text_new ();
   column = gtk_tree_view_column_new_with_attributes (_("Level"), renderer,
                                                      "text", 2, NULL);
   gtk_tree_view_append_column (GTK_TREE_VIEW (table), column);

   renderer = gtk_cell_renderer_text_new ();
   column = gtk_tree_view_column_new_with_attributes (_("Date"), renderer,
                                                      "text", 3, NULL);
   gtk_tree_view_append_column (GTK_TREE_VIEW (table), column);

   gtk_box_pack_start (GTK_BOX (vbox), table, TRUE, TRUE, 0);

   gtk_widget_show_all (dialog);
   gtk_dialog_run (GTK_DIALOG(dialog));
   gtk_widget_destroy (dialog);
}
