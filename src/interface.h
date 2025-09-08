#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>
#include "gallery.h" // Include gallery.h to get the AppState definition

// Function prototypes
void on_zoom_in_clicked(GtkWidget *widget, gpointer data);
void on_zoom_out_clicked(GtkWidget *widget, gpointer data);
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data);

#endif // INTERFACE_H
