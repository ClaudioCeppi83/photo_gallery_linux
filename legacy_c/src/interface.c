#include "interface.h"

void on_zoom_in_clicked(GtkWidget *widget, gpointer data)
{
	AppState *app_state = (AppState *)data;
	app_state->zoom_level *= 1.2;
	// In GTK4 with GtkPicture, we can implement zoom by changing the widget size
	// or using a GtkViewport. For now, we'll just update a scale if needed.
	// But GtkPicture scales to fit by default.
}

void on_zoom_out_clicked(GtkWidget *widget, gpointer data)
{
	AppState *app_state = (AppState *)data;
	app_state->zoom_level /= 1.2;
}

void on_draw(GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height, gpointer data)
{
	// This function is currently unused as we switched to GtkPicture
}
