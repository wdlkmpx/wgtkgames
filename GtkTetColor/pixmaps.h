
#ifndef __PIXMAPS_H__
#define __PIXMAPS_H__

gboolean redraw_cells (void);
gboolean redraw_all_cells (void);
gboolean create_pixmaps (GtkWidget * widget, gint first_time);
void free_pixmap_chars (void);

#endif
