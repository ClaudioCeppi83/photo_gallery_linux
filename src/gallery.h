#ifndef GALLERY_H
#define GALLERY_H

#include <gtk/gtk.h>

// Application state structure
typedef struct
{
	GtkWidget *window;
	GtkWidget *drawing_area;
	GtkWidget *box;
	GPtrArray *image_files; // Currently loaded subset of image paths for display
	int current_image_index;
	char *last_directory;
	GdkPixbuf *current_pixbuf;
	double zoom_level;
	GtkWidget *loading_spinner; // New: Loading spinner widget
	GtkWidget *next_button;		// New: Next button widget
	GtkWidget *prev_button;		// New: Previous button widget

	int total_image_count;		   // Total number of image files in the directory
	GPtrArray *loaded_image_paths; // All image paths found in the directory
	int loaded_start_index;		   // Start index of the currently loaded subset
	int loaded_end_index;		   // End index of the currently loaded subset
} AppState;

// Function prototypes
// load_images is now _load_images_sync and is static, so no prototype needed here
void on_next_clicked(GtkWidget *widget, gpointer data);
void on_prev_clicked(GtkWidget *widget, gpointer data);
void on_choose_dir_clicked(GtkWidget *widget, gpointer data);
void display_message(GtkWidget *parent, const char *message);

#endif // GALLERY_H