#ifndef GALLERY_H
#define GALLERY_H

#include <gtk/gtk.h>

// Function prototypes
void load_images(const char *directory);
void on_next_clicked(GtkWidget *widget, gpointer data);
void on_prev_clicked(GtkWidget *widget, gpointer data);
void on_choose_dir_clicked(GtkWidget *widget, gpointer data);
void display_message(GtkWidget *parent, const char *message);

// Global variables
extern char **image_files;
extern int current_image_index;
extern int num_images;
extern char *last_directory;
extern GdkPixbuf *current_pixbuf;

#endif // GALLERY_H
