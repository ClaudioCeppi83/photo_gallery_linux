#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>

// Function prototypes
void on_zoom_in_clicked(GtkWidget *widget, gpointer data);
void on_zoom_out_clicked(GtkWidget *widget, gpointer data);
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data);

// Global variables
extern GtkWidget *window;
extern GtkWidget *drawing_area;
extern GtkWidget *button_next;
extern GtkWidget *button_prev;
extern GtkWidget *button_choose_dir;
extern GtkWidget *button_zoom_in;
extern GtkWidget *button_zoom_out;
extern GtkWidget *box;
extern double zoom_level;

#endif // INTERFACE_H
