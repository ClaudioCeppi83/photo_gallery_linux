#include "gallery.h"
#include "interface.h"

// Function to free the AppState structure
void free_app_state(gpointer data)
{
	AppState *app_state = (AppState *)data;
	if (app_state->loaded_image_paths)
	{
		g_ptr_array_free(app_state->loaded_image_paths, TRUE);
	}
	if (app_state->image_files)
	{
		g_ptr_array_free(app_state->image_files, FALSE);
	}
	if (app_state->last_directory)
	{
		g_free(app_state->last_directory);
	}
	if (app_state->current_pixbuf)
	{
		g_object_unref(app_state->current_pixbuf);
	}
	g_free(app_state);
}

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	// Create the application state structure
	AppState *app_state = g_new0(AppState, 1);
	app_state->zoom_level = 1.0;
	app_state->current_image_index = 0;

	// Load the last selected directory from the configuration file
	FILE *fp = fopen("last_directory.conf", "r");
	if (fp)
	{
		char buffer[256];
		if (fgets(buffer, sizeof(buffer), fp))
		{
			buffer[strcspn(buffer, "\n")] = 0; // Remove newline character
			app_state->last_directory = g_strdup(buffer);
		}
		fclose(fp);
	}
	else
	{
		// Default to the user's Pictures directory
		app_state->last_directory = g_strdup(g_get_user_special_dir(G_USER_DIRECTORY_PICTURES));
	}

	// Get the screen resolution
	GdkDisplay *display = gdk_display_get_default();
	if (display == NULL)
	{
		g_print("Failed to get default display.\n");
		return 1;
	}

	GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
	if (monitor == NULL)
	{
		// Fallback to the first monitor if the primary monitor cannot be obtained
		monitor = gdk_display_get_monitor(display, 0);
		if (monitor == NULL)
		{
			g_print("Failed to get any monitor.\n");
			return 1;
		}
	}

	GdkRectangle geometry;
	gdk_monitor_get_geometry(monitor, &geometry);

	// Calculate 60% of the screen resolution
	int window_width = geometry.width * 0.6;
	int window_height = geometry.height * 0.6;

	app_state->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(app_state->window), "Photo Gallery");
	gtk_window_set_default_size(GTK_WINDOW(app_state->window), window_width, window_height);
	g_signal_connect(app_state->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_object_set_data_full(G_OBJECT(app_state->window), "app_state", app_state, free_app_state);

	app_state->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(app_state->window), app_state->box);

	app_state->drawing_area = gtk_drawing_area_new();
	gtk_widget_set_hexpand(app_state->drawing_area, TRUE);
	gtk_widget_set_vexpand(app_state->drawing_area, TRUE);
	g_signal_connect(app_state->drawing_area, "draw", G_CALLBACK(on_draw), app_state);
	gtk_box_pack_start(GTK_BOX(app_state->box), app_state->drawing_area, TRUE, TRUE, 0);

	GtkWidget *button_next = gtk_button_new_with_label("Next");
	g_signal_connect(button_next, "clicked", G_CALLBACK(on_next_clicked), app_state);
	gtk_box_pack_start(GTK_BOX(app_state->box), button_next, FALSE, FALSE, 0);

	GtkWidget *button_prev = gtk_button_new_with_label("Previous");
	g_signal_connect(button_prev, "clicked", G_CALLBACK(on_prev_clicked), app_state);
	gtk_box_pack_start(GTK_BOX(app_state->box), button_prev, FALSE, FALSE, 0);

	GtkWidget *button_choose_dir = gtk_button_new_with_label("Choose Directory");
	g_signal_connect(button_choose_dir, "clicked", G_CALLBACK(on_choose_dir_clicked), app_state);
	gtk_box_pack_start(GTK_BOX(app_state->box), button_choose_dir, FALSE, FALSE, 0);

	GtkWidget *button_zoom_in = gtk_button_new_with_label("Zoom In");
	g_signal_connect(button_zoom_in, "clicked", G_CALLBACK(on_zoom_in_clicked), app_state);
	gtk_box_pack_start(GTK_BOX(app_state->box), button_zoom_in, FALSE, FALSE, 0);

	GtkWidget *button_zoom_out = gtk_button_new_with_label("Zoom Out");
	g_signal_connect(button_zoom_out, "clicked", G_CALLBACK(on_zoom_out_clicked), app_state);
	gtk_box_pack_start(GTK_BOX(app_state->box), button_zoom_out, FALSE, FALSE, 0);

	// Initialize loading spinner and assign buttons to app_state
	app_state->loading_spinner = gtk_spinner_new();
	gtk_box_pack_start(GTK_BOX(app_state->box), app_state->loading_spinner, FALSE, FALSE, 0);
	gtk_widget_set_visible(app_state->loading_spinner, FALSE); // Initially hidden

	app_state->next_button = button_next;
	app_state->prev_button = button_prev;

	gtk_widget_show_all(app_state->window);
	gtk_main();

	return 0;
}