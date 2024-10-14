#include "interface.h"
#include "gallery.h"

// Global variables
GtkWidget *window;
GtkWidget *drawing_area;
GtkWidget *button_next;
GtkWidget *button_prev;
GtkWidget *button_choose_dir;
GtkWidget *button_zoom_in;
GtkWidget *button_zoom_out;
GtkWidget *box;
double zoom_level = 1.0;

void on_zoom_in_clicked(GtkWidget *widget, gpointer data) {
    zoom_level *= 1.2;
    gtk_widget_queue_draw(drawing_area);
}

void on_zoom_out_clicked(GtkWidget *widget, gpointer data) {
    zoom_level /= 1.2;
    gtk_widget_queue_draw(drawing_area);
}

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    if (current_pixbuf == NULL) {
        return FALSE;
    }

    int window_width = gtk_widget_get_allocated_width(widget);
    int window_height = gtk_widget_get_allocated_height(widget);
    int pixbuf_width = gdk_pixbuf_get_width(current_pixbuf);
    int pixbuf_height = gdk_pixbuf_get_height(current_pixbuf);

    double scale_x = (double)window_width / pixbuf_width;
    double scale_y = (double)window_height / pixbuf_height;
    double scale = MIN(scale_x, scale_y) * zoom_level;

    int draw_width = pixbuf_width * scale;
    int draw_height = pixbuf_height * scale;

    int offset_x = (window_width - draw_width) / 2;
    int offset_y = (window_height - draw_height) / 2;

    cairo_scale(cr, scale, scale);
    gdk_cairo_set_source_pixbuf(cr, current_pixbuf, offset_x / scale, offset_y / scale);
    cairo_paint(cr);

    return FALSE;
}
