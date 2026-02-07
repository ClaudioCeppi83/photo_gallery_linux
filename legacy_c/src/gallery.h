#ifndef GALLERY_H
#define GALLERY_H

#include <gtk/gtk.h>
#include <adwaita.h>

// Forward declaration to avoid circular dependencies
typedef struct _AppState AppState;

#include "interface.h"

// Main application state structure
struct _AppState
{
	GtkWidget *window;
	GtkWidget *picture;
	GtkWidget *overlay;
	GtkWidget *scrolled_window; // Contenedor desplazable para las miniaturas
	GtkWidget *thumbnails_box;	// Caja horizontal para las miniaturas
	GPtrArray *image_files;
	int current_image_index;
	char *last_directory;
	GdkPixbuf *current_pixbuf;
	double zoom_level;
	GtkWidget *loading_spinner;
	GtkWidget *next_button;
	GtkWidget *prev_button;
	int total_image_count;
	GPtrArray *loaded_image_paths;
	int loaded_start_index;
	int loaded_end_index;
};

// Function prototypes from gallery.c
void on_next_clicked(GtkWidget *widget, gpointer data);
void on_prev_clicked(GtkWidget *widget, gpointer data);
void on_choose_dir_clicked(GtkWidget *widget, gpointer data);
void display_message(GtkWidget *parent, const char *message);

#endif // GALLERY_H