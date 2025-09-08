#include "interface.h"

void on_zoom_in_clicked(GtkWidget *widget, gpointer data)
{
	AppState *app_state = (AppState *)data;
	app_state->zoom_level *= 1.2;
	gtk_widget_queue_draw(app_state->drawing_area);
}

void on_zoom_out_clicked(GtkWidget *widget, gpointer data)
{
	AppState *app_state = (AppState *)data;
	app_state->zoom_level /= 1.2;
	gtk_widget_queue_draw(app_state->drawing_area);
}

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	AppState *app_state = (AppState *)data;
	if (app_state->current_pixbuf == NULL)
	{
		// Draw "No image available" text
		cairo_set_source_rgb(cr, 0.5, 0.5, 0.5); // Grey color
		cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(cr, 24.0);

		int window_width = gtk_widget_get_allocated_width(widget);
		int window_height = gtk_widget_get_allocated_height(widget);

		cairo_text_extents_t extents;
		const char *text = "No image available";
		cairo_text_extents(cr, text, &extents);

		double x = window_width / 2.0 - (extents.width / 2.0 + extents.x_bearing);
		double y = window_height / 2.0 - (extents.height / 2.0 + extents.y_bearing);

		cairo_move_to(cr, x, y);
		cairo_show_text(cr, text);

		return FALSE;
	}

	int window_width = gtk_widget_get_allocated_width(widget);
	int window_height = gtk_widget_get_allocated_height(widget);
	int pixbuf_width = gdk_pixbuf_get_width(app_state->current_pixbuf);
	int pixbuf_height = gdk_pixbuf_get_height(app_state->current_pixbuf);

	double scale_x = (double)window_width / pixbuf_width;
	double scale_y = (double)window_height / pixbuf_height;
	double scale = MIN(scale_x, scale_y) * app_state->zoom_level;

	int draw_width = pixbuf_width * scale;
	int draw_height = pixbuf_height * scale;

	int offset_x = (window_width - draw_width) / 2;
	int offset_y = (window_height - draw_height) / 2;

	cairo_scale(cr, scale, scale);
	gdk_cairo_set_source_pixbuf(cr, app_state->current_pixbuf, offset_x / scale, offset_y / scale);
	cairo_paint(cr);

	return FALSE;
}
