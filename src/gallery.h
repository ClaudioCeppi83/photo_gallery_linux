#ifndef GALLERY_H
#define GALLERY_H

#include <gtk/gtk.h>

// Application state structure
typedef struct
{
	GtkWidget *window;
	GtkWidget *drawing_area;
	GtkWidget *box;
	GPtrArray *image_files;
	int current_image_index;
	char *last_directory;
	GdkPixbuf *current_pixbuf;
	double zoom_level;
} AppState;

// Function prototypes
void load_images(AppState *app_state);
void on_next_clicked(GtkWidget *widget, gpointer data);
void on_prev_clicked(GtkWidget *widget, gpointer data);
void on_choose_dir_clicked(GtkWidget *widget, gpointer data);
void display_message(GtkWidget *parent, const char *message);

#endif // GALLERY_H
