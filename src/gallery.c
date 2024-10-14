#include "gallery.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Global variables
char **image_files = NULL;
int current_image_index = 0;
int num_images = 0;
char *last_directory = NULL;
GdkPixbuf *current_pixbuf = NULL;

void load_images(const char *directory) {
    DIR *dir;
    struct dirent *ent;
    dir = opendir(directory);
    if (dir == NULL) {
        g_print("Could not open directory\n");
        return;
    }

    num_images = 0;
    while ((ent = readdir(dir)) != NULL) {
        if (strstr(ent->d_name, ".jpg") || strstr(ent->d_name, ".png")) {
            image_files = (char **)realloc(image_files, (num_images + 1) * sizeof(char *));
            image_files[num_images] = g_strdup_printf("%s/%s", directory, ent->d_name);
            num_images++;
        }
    }
    closedir(dir);
}

void on_next_clicked(GtkWidget *widget, gpointer data) {
    if (num_images == 0) {
        g_print("No images to display.\n");
        return;
    }
    current_image_index = (current_image_index + 1) % num_images;
    if (current_pixbuf) {
        g_object_unref(current_pixbuf);
    }
    current_pixbuf = gdk_pixbuf_new_from_file(image_files[current_image_index], NULL);
    gtk_widget_queue_draw(GTK_WIDGET(data));
}

void on_prev_clicked(GtkWidget *widget, gpointer data) {
    if (num_images == 0) {
        g_print("No images to display.\n");
        return;
    }
    current_image_index = (current_image_index - 1 + num_images) % num_images;
    if (current_pixbuf) {
        g_object_unref(current_pixbuf);
    }
    current_pixbuf = gdk_pixbuf_new_from_file(image_files[current_image_index], NULL);
    gtk_widget_queue_draw(GTK_WIDGET(data));
}

void on_choose_dir_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Choose Directory",
                                         GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                         GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                         "Cancel", GTK_RESPONSE_CANCEL,
                                         "Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (last_directory) {
            g_free(last_directory);
        }
        last_directory = g_strdup(filename);
        g_free(filename);

        // Save the selected directory to a configuration file
        FILE *fp = fopen("last_directory.conf", "w");
        if (fp) {
            fprintf(fp, "%s", last_directory);
            fclose(fp);
        }

        load_images(last_directory);
        if (num_images > 0) {
            if (current_pixbuf) {
                g_object_unref(current_pixbuf);
            }
            current_pixbuf = gdk_pixbuf_new_from_file(image_files[current_image_index], NULL);
            gtk_widget_queue_draw(GTK_WIDGET(data));
        } else {
            char message[256];
            snprintf(message, sizeof(message), "No images found in the directory: %s", last_directory);
            display_message(gtk_widget_get_toplevel(widget), message);
        }

        // Update the window title to show the current directory
        gtk_window_set_title(GTK_WINDOW(gtk_widget_get_toplevel(widget)), last_directory);
    }
    gtk_widget_destroy(dialog);
}

void display_message(GtkWidget *parent, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
