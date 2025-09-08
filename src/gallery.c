#include "gallery.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h> // New include for GdkPixbuf functions

void update_thumbnails(AppState *app_state);

// Define the size of the image batch to load at a time
#define BATCH_SIZE 100

// Forward declarations for thread functions
static gpointer scan_directory_thread_func(gpointer data);
static gboolean on_scan_completed(gpointer data);
typedef struct
{
	int index;
	AppState *app_state;
} ThumbnailData;

void on_thumbnail_clicked(GtkWidget *button, gpointer user_data);

static void _load_images_sync(AppState *app_state)
{
	DIR *dir;
	struct dirent *ent;

	// Free previous loaded_image_paths if it exists
	if (app_state->loaded_image_paths)
	{
		g_print("_load_images_sync: Freeing loaded_image_paths %p\n", (void *)app_state->loaded_image_paths);
		g_ptr_array_free(app_state->loaded_image_paths, TRUE);
	}
	app_state->loaded_image_paths = g_ptr_array_new_with_free_func(g_free);
	app_state->total_image_count = 0;

	dir = opendir(app_state->last_directory);
	if (dir == NULL)
	{
		g_print("Could not open directory: %s\n", app_state->last_directory);
		return;
	}

	// Get supported image extensions
	GSList *formats = gdk_pixbuf_get_formats();
	GHashTable *supported_extensions = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

	for (GSList *l = formats; l != NULL; l = l->next)
	{
		GdkPixbufFormat *format = (GdkPixbufFormat *)l->data;
		gchar **extensions = gdk_pixbuf_format_get_extensions(format);
		for (gchar **ext = extensions; *ext != NULL; ext++)
		{
			g_hash_table_insert(supported_extensions, g_strdup(*ext), GINT_TO_POINTER(1)); // Value doesn't matter, just using it as a set
		}
		g_strfreev(extensions); // Free the array of extensions
	}
	g_slist_free(formats); // Free the list of formats

	// First pass: Collect all image paths
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
				g_ptr_array_add(app_state->loaded_image_paths, full_path);
				app_state->total_image_count++;
			}
			g_free(lower_extension);
		}
	}
	closedir(dir);

	g_hash_table_destroy(supported_extensions); // Free the hash table

	// Now, populate app_state->image_files with a subset for display
	if (app_state->image_files)
	{
		g_print("_load_images_sync: Freeing image_files %p\n", (void *)app_state->image_files);
		g_ptr_array_free(app_state->image_files, FALSE); // Changed to FALSE: image_files does not own the strings
	}
	app_state->image_files = g_ptr_array_new(); // Changed: no free function

	app_state->loaded_start_index = 0;
	app_state->loaded_end_index = MIN(BATCH_SIZE, app_state->total_image_count);

	for (int i = app_state->loaded_start_index; i < app_state->loaded_end_index; i++)
	{
		g_ptr_array_add(app_state->image_files, g_ptr_array_index(app_state->loaded_image_paths, i)); // Removed g_strdup
	}
}

// Helper function to load a specific batch of images
static void load_image_batch(AppState *app_state, int start_index)
{
	if (app_state->image_files)
	{
		g_ptr_array_free(app_state->image_files, FALSE); // Changed to FALSE: image_files does not own the strings
	}
	app_state->image_files = g_ptr_array_new(); // Changed: no free function

	app_state->loaded_start_index = start_index;
	app_state->loaded_end_index = MIN(start_index + BATCH_SIZE, app_state->total_image_count);

	for (int i = app_state->loaded_start_index; i < app_state->loaded_end_index; i++)
	{
		g_ptr_array_add(app_state->image_files, g_ptr_array_index(app_state->loaded_image_paths, i)); // Removed g_strdup
	}
}

void on_next_clicked(GtkWidget *widget, gpointer data)
{
	AppState *app_state = (AppState *)data;
	if (app_state->total_image_count == 0)
	{
		g_print("No images to display.\n");
		return;
	}

	app_state->current_image_index++;
	if (app_state->current_image_index >= app_state->total_image_count)
	{
		app_state->current_image_index = 0; // Loop back to the first image
	}

	// Check if we need to load a new batch
	if (app_state->current_image_index >= app_state->loaded_end_index)
	{
		load_image_batch(app_state, app_state->current_image_index); // Load new batch starting from current index
	}

	GError *error = NULL;
	char *image_path = (char *)g_ptr_array_index(app_state->image_files, app_state->current_image_index - app_state->loaded_start_index);
	char *temp_path = g_strdup(image_path); // Duplicate the string
	GdkPixbuf *new_pixbuf = gdk_pixbuf_new_from_file(temp_path, &error);
	g_free(temp_path); // Free the duplicated string immediately

	if (new_pixbuf != NULL)
	{
		// if (app_state->current_pixbuf)
		// {
		// g_print("on_next_clicked: Unrefencing pixbuf %p, ref_count %d\n", (void*)app_state->current_pixbuf, G_OBJECT(app_state->current_pixbuf)->ref_count);
		// g_object_unref(app_state->current_pixbuf);
		// }
		app_state->current_pixbuf = new_pixbuf;
	}
	else
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
	if (app_state->total_image_count == 0)
	{
		g_print("No images to display.\n");
		return;
	}

	app_state->current_image_index--;
	if (app_state->current_image_index < 0)
	{
		app_state->current_image_index = app_state->total_image_count - 1; // Loop back to the last image
	}

	// Check if we need to load a new batch
	if (app_state->current_image_index < app_state->loaded_start_index)
	{
		load_image_batch(app_state, app_state->current_image_index - BATCH_SIZE + 1); // Load previous batch
	}

	GError *error = NULL;
	char *image_path = (char *)g_ptr_array_index(app_state->image_files, app_state->current_image_index - app_state->loaded_start_index);
	char *temp_path = g_strdup(image_path); // Duplicate the string
	GdkPixbuf *new_pixbuf = gdk_pixbuf_new_from_file(temp_path, &error);
	g_free(temp_path); // Free the duplicated string immediately

	if (new_pixbuf != NULL)
	{
		// if (app_state->current_pixbuf)
		// {
		// g_print("on_prev_clicked: Unrefencing pixbuf %p, ref_count %d\n", (void*)app_state->current_pixbuf, G_OBJECT(app_state->current_pixbuf)->ref_count);
		// g_object_unref(app_state->current_pixbuf);
		// }
		app_state->current_pixbuf = new_pixbuf;
	}
	else
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

		// Show loading indicator and disable UI elements
		gtk_widget_set_visible(app_state->loading_spinner, TRUE);
		gtk_spinner_start((GtkSpinner *)app_state->loading_spinner);
		gtk_widget_set_sensitive(app_state->next_button, FALSE);
		gtk_widget_set_sensitive(app_state->prev_button, FALSE);

		// Start scanning in a new thread
		g_thread_new("scan_directory_thread", scan_directory_thread_func, app_state);

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

// Thread function for asynchronous directory scanning
static gpointer scan_directory_thread_func(gpointer data)
{
	AppState *app_state = (AppState *)data;

	_load_images_sync(app_state); // Perform the actual scanning

	// Signal the main thread that scanning is complete
	g_idle_add((GSourceFunc)on_scan_completed, app_state);

	return NULL;
}

// Callback for when scanning is completed (runs in main thread)
static gboolean on_scan_completed(gpointer data)
{
	AppState *app_state = (AppState *)data;

	// Hide loading indicator and enable UI elements
	gtk_spinner_stop((GtkSpinner *)app_state->loading_spinner);
	gtk_widget_set_visible(app_state->loading_spinner, FALSE);
	gtk_widget_set_sensitive(app_state->next_button, TRUE);
	gtk_widget_set_sensitive(app_state->prev_button, TRUE);

	if (app_state->image_files->len > 0)
	{
		gtk_widget_show(app_state->next_button);
		gtk_widget_show(app_state->prev_button);
		app_state->current_image_index = 0; // Reset to first image
		GError *error = NULL;
		char *image_path = (char *)g_ptr_array_index(app_state->image_files, app_state->current_image_index - app_state->loaded_start_index);
		GdkPixbuf *new_pixbuf = gdk_pixbuf_new_from_file(image_path, &error);

		if (new_pixbuf != NULL)
		{
			if (app_state->current_pixbuf)
			{
				g_object_unref(app_state->current_pixbuf);
			}
			app_state->current_pixbuf = new_pixbuf;
		}
		else
		{
			g_warning("Error loading image: %s", error->message);
			g_error_free(error);
			app_state->current_pixbuf = NULL; // Ensure it's NULL if loading failed
		}
		gtk_widget_queue_draw(app_state->drawing_area);
	}
	else
	{
		gtk_widget_hide(app_state->next_button);
		gtk_widget_hide(app_state->prev_button);
		char message[256];
		sprintf(message, "No images found in the directory: %s", app_state->last_directory);
		display_message(app_state->window, message);
	}
	// Poblar la tira de miniaturas después de cargar las imágenes
	update_thumbnails(app_state);

	return G_SOURCE_REMOVE; // Remove this source from the main loop
}

// Función para poblar la tira de miniaturas
void update_thumbnails(AppState *app_state)
{
	if (!app_state || !app_state->thumbnails_box || !app_state->scrolled_window || !app_state->loaded_image_paths)
	{
		return;
	}
	gtk_container_foreach(GTK_CONTAINER(app_state->thumbnails_box), (GtkCallback)gtk_widget_destroy, NULL);
	if (app_state->total_image_count == 0)
	{
		gtk_widget_hide(app_state->scrolled_window);
		return;
	}
	for (int i = 0; i < app_state->total_image_count; i++)
	{
		char *image_path = g_ptr_array_index(app_state->loaded_image_paths, i);
		if (!image_path)
			continue;
		GError *error = NULL;
		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(image_path, 80, 80, TRUE, &error);
		GtkWidget *thumb = gtk_image_new_from_pixbuf(pixbuf);
		GtkWidget *button = gtk_button_new();
		gtk_container_add(GTK_CONTAINER(button), thumb);
		gtk_widget_set_tooltip_text(button, image_path);
		if (i == app_state->current_image_index)
		{
			GtkStyleContext *ctx = gtk_widget_get_style_context(button);
			gtk_style_context_add_class(ctx, "suggested-action");
		}
		ThumbnailData *thumb_data = g_new(ThumbnailData, 1);
		thumb_data->index = i;
		thumb_data->app_state = app_state;
		g_signal_connect(button, "clicked", G_CALLBACK(on_thumbnail_clicked), thumb_data);
		gtk_box_pack_start(GTK_BOX(app_state->thumbnails_box), button, FALSE, FALSE, 0);
		if (pixbuf)
			g_object_unref(pixbuf);
	}
	gtk_widget_show_all(app_state->scrolled_window);
}

// Callback para miniatura
void on_thumbnail_clicked(GtkWidget *button, gpointer user_data)
{
	ThumbnailData *thumb_data = (ThumbnailData *)user_data;
	if (!thumb_data || !thumb_data->app_state)
	{
		g_free(thumb_data);
		return;
	}
	int index = thumb_data->index;
	AppState *app_state = thumb_data->app_state;
	app_state->current_image_index = index;
	// Actualizar imagen principal
	GError *error = NULL;
	char *image_path = g_ptr_array_index(app_state->loaded_image_paths, index);
	GdkPixbuf *new_pixbuf = gdk_pixbuf_new_from_file(image_path, &error);
	if (new_pixbuf)
	{
		if (app_state->current_pixbuf)
			g_object_unref(app_state->current_pixbuf);
		app_state->current_pixbuf = new_pixbuf;
	}
	else
	{
		if (error)
			g_error_free(error);
		app_state->current_pixbuf = NULL;
	}
	gtk_widget_queue_draw(app_state->drawing_area);
	// Actualizar borde de selección
	update_thumbnails(app_state);
	g_free(thumb_data);
}
