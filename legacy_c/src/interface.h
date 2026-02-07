#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>
#include "gallery.h"

// Function prototypes
void on_zoom_in_clicked(GtkWidget *widget, gpointer data);
void on_zoom_out_clicked(GtkWidget *widget, gpointer data);
void on_draw(GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height, gpointer data);

#endif // INTERFACE_H
