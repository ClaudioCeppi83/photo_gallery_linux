#include "gallery.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

void update_thumbnails(AppState *app_state);

#define BATCH_SIZE 100

static gpointer scan_directory_thread_func(gpointer data);
static gboolean on_scan_completed(gpointer data);

typedef struct
{
	int index;
	AppState *app_state;
} ThumbnailData;

void on_thumbnail_clicked(GtkButton *button, gpointer user_data);

// Helper to set image from pixbuf in GTK4
static void set_picture_pixbuf(GtkPicture *picture, GdkPixbuf *pixbuf)
{
	if (pixbuf)
	{
		GdkTexture *texture = gdk_texture_new_for_pixbuf(pixbuf);
		gtk_picture_set_paintable(picture, GDK_PAINTABLE(texture));
		g_object_unref(texture);
	}
	else
	{
		gtk_picture_set_paintable(picture, NULL);
	}
}

static void _load_images_sync(AppState *app_state)
{
	DIR *dir;
	struct dirent *ent;

	if (app_state->loaded_image_paths)
	{
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

	GSList *formats = gdk_pixbuf_get_formats();
	GHashTable *supported_extensions = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

	for (GSList *l = formats; l != NULL; l = l->next)
	{
		GdkPixbufFormat *format = (GdkPixbufFormat *)l->data;
		gchar **extensions = gdk_pixbuf_format_get_extensions(format);
		for (gchar **ext = extensions; *ext != NULL; ext++)
		{
			g_hash_table_insert(supported_extensions, g_strdup(*ext), GINT_TO_POINTER(1));
		}
		g_strfreev(extensions);
	}
	g_slist_free(formats);

	while ((ent = readdir(dir)) != NULL)
	{
		const char *dot = strrchr(ent->d_name, '.');
		if (dot && dot != ent->d_name)
		{
			const char *extension = dot + 1;
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
	g_hash_table_destroy(supported_extensions);

	if (app_state->image_files)
	{
		g_ptr_array_free(app_state->image_files, FALSE);
	}
	app_state->image_files = g_ptr_array_new();

	app_state->loaded_start_index = 0;
	app_state->loaded_end_index = MIN(BATCH_SIZE, app_state->total_image_count);

	for (int i = app_state->loaded_start_index; i < app_state->loaded_end_index; i++)
	{
		g_ptr_array_add(app_state->image_files, g_ptr_array_index(app_state->loaded_image_paths, i));
	}
}

static void load_image_batch(AppState *app_state, int start_index)
{
	if (app_state->image_files)
	{
		g_ptr_array_free(app_state->image_files, FALSE);
	}
	app_state->image_files = g_ptr_array_new();

	app_state->loaded_start_index = MAX(0, start_index);
	app_state->loaded_end_index = MIN(app_state->loaded_start_index + BATCH_SIZE, app_state->total_image_count);

	for (int i = app_state->loaded_start_index; i < app_state->loaded_end_index; i++)
	{
		g_ptr_array_add(app_state->image_files, g_ptr_array_index(app_state->loaded_image_paths, i));
	}
}

void on_next_clicked(GtkWidget *widget, gpointer data)
{
	(void)widget;
	AppState *app_state = (AppState *)data;
	if (app_state->total_image_count == 0) return;

	app_state->current_image_index++;
	if (app_state->current_image_index >= app_state->total_image_count)
	{
		app_state->current_image_index = 0;
	}

	if (app_state->current_image_index >= app_state->loaded_end_index || app_state->current_image_index < app_state->loaded_start_index)
	{
		load_image_batch(app_state, app_state->current_image_index);
	}

	GError *error = NULL;
	char *image_path = (char *)g_ptr_array_index(app_state->loaded_image_paths, app_state->current_image_index);
	GdkPixbuf *new_pixbuf = gdk_pixbuf_new_from_file(image_path, &error);

	if (new_pixbuf != NULL)
	{
		if (app_state->current_pixbuf) g_object_unref(app_state->current_pixbuf);
		app_state->current_pixbuf = new_pixbuf;
		set_picture_pixbuf(GTK_PICTURE(app_state->picture), new_pixbuf);
	}
	else
	{
		g_warning("Error loading image: %s", error->message);
		if (error) g_error_free(error);
		app_state->current_pixbuf = NULL;
		set_picture_pixbuf(GTK_PICTURE(app_state->picture), NULL);
	}
	update_thumbnails(app_state);
}

void on_prev_clicked(GtkWidget *widget, gpointer data)
{
	(void)widget;
	AppState *app_state = (AppState *)data;
	if (app_state->total_image_count == 0) return;

	app_state->current_image_index--;
	if (app_state->current_image_index < 0)
	{
		app_state->current_image_index = app_state->total_image_count - 1;
	}

	if (app_state->current_image_index < app_state->loaded_start_index || app_state->current_image_index >= app_state->loaded_end_index)
	{
		load_image_batch(app_state, MAX(0, app_state->current_image_index - BATCH_SIZE + 1));
	}

	GError *error = NULL;
	char *image_path = (char *)g_ptr_array_index(app_state->loaded_image_paths, app_state->current_image_index);
	GdkPixbuf *new_pixbuf = gdk_pixbuf_new_from_file(image_path, &error);

	if (new_pixbuf != NULL)
	{
		if (app_state->current_pixbuf) g_object_unref(app_state->current_pixbuf);
		app_state->current_pixbuf = new_pixbuf;
		set_picture_pixbuf(GTK_PICTURE(app_state->picture), new_pixbuf);
	}
	else
	{
		g_warning("Error loading image: %s", error->message);
		if (error) g_error_free(error);
		app_state->current_pixbuf = NULL;
		set_picture_pixbuf(GTK_PICTURE(app_state->picture), NULL);
	}
	update_thumbnails(app_state);
}

static void on_file_chooser_response(GtkNativeDialog *dialog, int response_id, gpointer data)
{
	AppState *app_state = (AppState *)data;
	if (response_id == GTK_RESPONSE_ACCEPT)
	{
		GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
		char *path = g_file_get_path(file);
		if (app_state->last_directory) g_free(app_state->last_directory);
		app_state->last_directory = g_strdup(path);
		g_free(path);
		g_object_unref(file);

		FILE *fp = fopen("last_directory.conf", "w");
		if (fp) { fprintf(fp, "%s", app_state->last_directory); fclose(fp); }

		gtk_widget_set_visible(app_state->loading_spinner, TRUE);
		gtk_spinner_start(GTK_SPINNER(app_state->loading_spinner));
		gtk_widget_set_sensitive(app_state->next_button, FALSE);
		gtk_widget_set_sensitive(app_state->prev_button, FALSE);

		g_thread_new("scan_directory_thread", scan_directory_thread_func, app_state);
		gtk_window_set_title(GTK_WINDOW(app_state->window), app_state->last_directory);
	}
	g_object_unref(dialog);
}

void on_choose_dir_clicked(GtkWidget *widget, gpointer data)
{
	(void)widget;
	AppState *app_state = (AppState *)data;
	GtkFileChooserNative *native = gtk_file_chooser_native_new("Choose Directory",
										 GTK_WINDOW(app_state->window),
										 GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
										 "Open", "Cancel");
	g_signal_connect(native, "response", G_CALLBACK(on_file_chooser_response), app_state);
	gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));
}

void display_message(GtkWidget *parent, const char *message)
{
	GtkWidget *dialog = adw_message_dialog_new(GTK_WINDOW(parent), "Information", message);
	adw_message_dialog_add_response(ADW_MESSAGE_DIALOG(dialog), "ok", "OK");
	gtk_window_present(GTK_WINDOW(dialog));
}

static gpointer scan_directory_thread_func(gpointer data)
{
	AppState *app_state = (AppState *)data;
	_load_images_sync(app_state);
	g_idle_add((GSourceFunc)on_scan_completed, app_state);
	return NULL;
}

static gboolean on_scan_completed(gpointer data)
{
	AppState *app_state = (AppState *)data;

	gtk_spinner_stop(GTK_SPINNER(app_state->loading_spinner));
	gtk_widget_set_visible(app_state->loading_spinner, FALSE);
	gtk_widget_set_sensitive(app_state->next_button, TRUE);
	gtk_widget_set_sensitive(app_state->prev_button, TRUE);

	if (app_state->total_image_count > 0)
	{
		gtk_widget_set_visible(app_state->next_button, TRUE);
		gtk_widget_set_visible(app_state->prev_button, TRUE);
		app_state->current_image_index = 0;
		GError *error = NULL;
		char *image_path = (char *)g_ptr_array_index(app_state->loaded_image_paths, 0);
		GdkPixbuf *new_pixbuf = gdk_pixbuf_new_from_file(image_path, &error);

		if (new_pixbuf != NULL)
		{
			if (app_state->current_pixbuf) g_object_unref(app_state->current_pixbuf);
			app_state->current_pixbuf = new_pixbuf;
			set_picture_pixbuf(GTK_PICTURE(app_state->picture), new_pixbuf);
		}
		else
		{
			g_warning("Error loading image: %s", error->message);
			if (error) g_error_free(error);
			app_state->current_pixbuf = NULL;
			set_picture_pixbuf(GTK_PICTURE(app_state->picture), NULL);
		}
	}
	else
	{
		gtk_widget_set_visible(app_state->next_button, FALSE);
		gtk_widget_set_visible(app_state->prev_button, FALSE);
		char message[256];
		sprintf(message, "No images found in: %s", app_state->last_directory);
		display_message(app_state->window, message);
	}
	update_thumbnails(app_state);
	return G_SOURCE_REMOVE;
}

void update_thumbnails(AppState *app_state)
{
	if (!app_state || !app_state->thumbnails_box || !app_state->loaded_image_paths) return;

	GtkWidget *child;
	while ((child = gtk_widget_get_first_child(app_state->thumbnails_box)) != NULL)
	{
		gtk_box_remove(GTK_BOX(app_state->thumbnails_box), child);
	}

	if (app_state->total_image_count == 0)
	{
		gtk_widget_set_visible(app_state->scrolled_window, FALSE);
		return;
	}

	for (int i = 0; i < app_state->total_image_count; i++)
	{
		char *image_path = g_ptr_array_index(app_state->loaded_image_paths, i);
		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(image_path, 80, 80, TRUE, NULL);
		if (!pixbuf) continue;

		GdkTexture *texture = gdk_texture_new_for_pixbuf(pixbuf);
		GtkWidget *thumb = gtk_image_new_from_paintable(GDK_PAINTABLE(texture));
		g_object_unref(texture);
		
		GtkWidget *button = gtk_button_new();
		gtk_button_set_child(GTK_BUTTON(button), thumb);
		gtk_widget_set_tooltip_text(button, image_path);

		if (i == app_state->current_image_index)
		{
			gtk_widget_add_css_class(button, "suggested-action");
		}

		ThumbnailData *thumb_data = g_new(ThumbnailData, 1);
		thumb_data->index = i;
		thumb_data->app_state = app_state;
		// Use GCallback cast for signal
		g_signal_connect_data(button, "clicked", G_CALLBACK(on_thumbnail_clicked), thumb_data, (GClosureNotify)g_free, 0);

		gtk_box_append(GTK_BOX(app_state->thumbnails_box), button);
		g_object_unref(pixbuf);
	}
	gtk_widget_set_visible(app_state->scrolled_window, TRUE);
}

void on_thumbnail_clicked(GtkButton *button, gpointer user_data)
{
	(void)button;
	ThumbnailData *thumb_data = (ThumbnailData *)user_data;
	AppState *app_state = thumb_data->app_state;
	int index = thumb_data->index;

	app_state->current_image_index = index;
	GError *error = NULL;
	char *image_path = g_ptr_array_index(app_state->loaded_image_paths, index);
	GdkPixbuf *new_pixbuf = gdk_pixbuf_new_from_file(image_path, &error);

	if (new_pixbuf)
	{
		if (app_state->current_pixbuf) g_object_unref(app_state->current_pixbuf);
		app_state->current_pixbuf = new_pixbuf;
		set_picture_pixbuf(GTK_PICTURE(app_state->picture), new_pixbuf);
	}
	else
	{
		if (error) g_error_free(error);
		app_state->current_pixbuf = NULL;
		set_picture_pixbuf(GTK_PICTURE(app_state->picture), NULL);
	}
	update_thumbnails(app_state);
}
