#include "gallery.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h> // New include for GdkPixbuf functions

void load_images(AppState *app_state)
{
	DIR *dir;
	struct dirent *ent;

	// If image_files already exists, free it before creating a new one.
	if (app_state->image_files)
	{
		g_ptr_array_free(app_state->image_files, TRUE);
	}
	app_state->image_files = g_ptr_array_new_with_free_func(g_free);

	dir = opendir(app_state->last_directory);
	if (dir == NULL)
	{
		g_print("Could not open directory\n");
		return;
	}

	// Get supported image extensions
	GSList *formats = gdk_pixbuf_get_formats();
	GHashTable *supported_extensions = g_hash_table_new(g_str_hash, g_str_equal);

	for (GSList *l = formats; l != NULL; l = l->next)
	{
		GdkPixbufFormat *format = (GdkPixbufFormat *)l->data;
		gchar **extensions = gdk_pixbuf_format_get_extensions(format);
		for (gchar **ext = extensions; *ext != NULL; ext++)
		{
			g_hash_table_insert(supported_extensions, *ext, GINT_TO_POINTER(1)); // Value doesn't matter, just using it as a set
		}
		g_strfreev(extensions); // Free the array of extensions
	}
	g_slist_free(formats); // Free the list of formats

	while ((ent = readdir(dir)) != NULL)
	{
		const char *dot = strrchr(ent->d_name, '.');
		if (dot && dot != ent->d_name) // Check if dot exists and is not the first character
		{
			const char *extension = dot + 1;
			// Convert extension to lowercase for case-insensitive comparison
			gchar *lower_extension = g_ascii_strdown(extension, -1);

			if (g_hash_table_contains(supported_extensions, lower_extension))
			{
				char *full_path = g_strdup_printf("%s/%s", app_state->last_directory, ent->d_name);
				g_ptr_array_add(app_state->image_files, full_path);
			}
			g_free(lower_extension);
		}
	}
	closedir(dir);

	g_hash_table_destroy(supported_extensions); // Free the hash table
}


void on_next_clicked(GtkWidget *widget, gpointer data)
{
	AppState *app_state = (AppState *)data;
	if (app_state->image_files->len == 0)
	{
		g_print("No images to display.\n");
		return;
	}
	app_state->current_image_index = (app_state->current_image_index + 1) % app_state->image_files->len;
	if (app_state->current_pixbuf)
	{
		g_object_unref(app_state->current_pixbuf);
	}
	GError *error = NULL;
	app_state->current_pixbuf = gdk_pixbuf_new_from_file((char *)g_ptr_array_index(app_state->image_files, app_state->current_image_index), &error);
	if (error != NULL)
	{
		g_warning("Error loading image: %s", error->message);
		g_error_free(error);
		app_state->current_pixbuf = NULL; // Ensure it's NULL if loading failed
	}
	gtk_widget_queue_draw(app_state->drawing_area);
}

void on_prev_clicked(GtkWidget *widget, gpointer data)
{
	AppState *app_state = (AppState *)data;
	if (app_state->image_files->len == 0)
	{
		g_print("No images to display.\n");
		return;
	}
	app_state->current_image_index = (app_state->current_image_index - 1 + app_state->image_files->len) % app_state->image_files->len;
	if (app_state->current_pixbuf)
	{
		g_object_unref(app_state->current_pixbuf);
	}
	GError *error = NULL;
	app_state->current_pixbuf = gdk_pixbuf_new_from_file((char *)g_ptr_array_index(app_state->image_files, app_state->current_image_index), &error);
	if (error != NULL)
	{
		g_warning("Error loading image: %s", error->message);
		g_error_free(error);
		app_state->current_pixbuf = NULL; // Ensure it's NULL if loading failed
	}
	gtk_widget_queue_draw(app_state->drawing_area);
}

void on_choose_dir_clicked(GtkWidget *widget, gpointer data)
{
	AppState *app_state = (AppState *)data;
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new("Choose Directory",
										 GTK_WINDOW(app_state->window),
										 GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
										 "Cancel", GTK_RESPONSE_CANCEL,
										 "Open", GTK_RESPONSE_ACCEPT,
										 NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		if (app_state->last_directory)
		{
			g_free(app_state->last_directory);
		}
		app_state->last_directory = g_strdup(filename);
		g_free(filename);

		// Save the selected directory to a configuration file
		FILE *fp = fopen("last_directory.conf", "w");
		if (fp)
		{
			fprintf(fp, "%s", app_state->last_directory);
			fclose(fp);
		}

		load_images(app_state);
		if (app_state->image_files->len > 0)
		{
			if (app_state->current_pixbuf)
			{
				g_object_unref(app_state->current_pixbuf);
			}
			GError *error = NULL;
			app_state->current_pixbuf = gdk_pixbuf_new_from_file((char *)g_ptr_array_index(app_state->image_files, app_state->current_image_index), &error);
			if (error != NULL)
			{
				g_warning("Error loading image: %s", error->message);
				g_error_free(error);
				app_state->current_pixbuf = NULL; // Ensure it's NULL if loading failed
			}
			gtk_widget_queue_draw(app_state->drawing_area);
		}
		else
		{
			char message[256];
			snprintf(message, sizeof(message), "No images found in the directory: %s", app_state->last_directory);
			display_message(app_state->window, message);
		}

		// Update the window title to show the current directory
		gtk_window_set_title(GTK_WINDOW(app_state->window), app_state->last_directory);
	}
	gtk_widget_destroy(dialog);
}

void display_message(GtkWidget *parent, const char *message)
{
	GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
											   GTK_DIALOG_MODAL,
											   GTK_MESSAGE_INFO,
											   GTK_BUTTONS_OK,
											   "%s", message);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
