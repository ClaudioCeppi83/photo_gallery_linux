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
	gtk_window_set_default_size(GTK_WINDOW(app_state->window), window_width, window_height);
	g_signal_connect(app_state->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_object_set_data_full(G_OBJECT(app_state->window), "app_state", app_state, free_app_state);
	gtk_window_set_decorated(GTK_WINDOW(app_state->window), FALSE);

	// Load the CSS file
	GtkCssProvider *provider = gtk_css_provider_new();
	GdkScreen *screen = gdk_screen_get_default();
	gtk_css_provider_load_from_path(provider, "style.css", NULL);
	gtk_style_context_add_provider_for_screen(screen,
																GTK_STYLE_PROVIDER(provider),
																GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	// Create HeaderBar
	GtkWidget *header_bar = gtk_header_bar_new();
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header_bar), TRUE);
	gtk_header_bar_set_title(GTK_HEADER_BAR(header_bar), "Ubuntw");
	gtk_window_set_titlebar(GTK_WINDOW(app_state->window), header_bar);

	// Add icon to header bar (at the start)
	GtkWidget *icon = gtk_image_new_from_icon_name("ubuntu-logo-icon-symbolic", GTK_ICON_SIZE_MENU);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(header_bar), icon);

	// Add "Choose Directory" and "More" buttons to the end of the header bar
	GtkWidget *button_more = gtk_button_new_from_icon_name("view-list-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), button_more);
	GtkWidget *button_choose_dir = gtk_button_new_from_icon_name("folder-open-symbolic", GTK_ICON_SIZE_BUTTON);
	g_signal_connect(button_choose_dir, "clicked", G_CALLBACK(on_choose_dir_clicked), app_state);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), button_choose_dir);

	// Create a central box for the zoom controls
	GtkWidget *center_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	
	// Add "Zoom" buttons to the central box
	GtkWidget *button_zoom_in = gtk_button_new_from_icon_name("zoom-in-symbolic", GTK_ICON_SIZE_BUTTON);
	GtkWidget *button_zoom_out = gtk_button_new_from_icon_name("zoom-out-symbolic", GTK_ICON_SIZE_BUTTON);
	g_signal_connect(button_zoom_in, "clicked", G_CALLBACK(on_zoom_in_clicked), app_state);
	g_signal_connect(button_zoom_out, "clicked", G_CALLBACK(on_zoom_out_clicked), app_state);

	gtk_box_pack_start(GTK_BOX(center_box), gtk_label_new("Zoop-in"), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(center_box), button_zoom_in, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(center_box), gtk_label_new("Zoop-out"), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(center_box), button_zoom_out, FALSE, FALSE, 0);

    // Set the central box as the custom title area
	gtk_header_bar_set_custom_title(GTK_HEADER_BAR(header_bar), center_box);

	// Crear la caja vertical principal
	GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(app_state->window), main_vbox);

	// Overlay para la imagen principal
	app_state->overlay = gtk_overlay_new();
	gtk_box_pack_start(GTK_BOX(main_vbox), app_state->overlay, TRUE, TRUE, 0);

	app_state->drawing_area = gtk_drawing_area_new();
	gtk_widget_set_hexpand(app_state->drawing_area, TRUE);
	gtk_widget_set_vexpand(app_state->drawing_area, TRUE);
	g_signal_connect(app_state->drawing_area, "draw", G_CALLBACK(on_draw), app_state);
	gtk_container_add(GTK_CONTAINER(app_state->overlay), app_state->drawing_area);

	// Navigation Buttons
	app_state->next_button = gtk_button_new_from_icon_name("go-next-symbolic", GTK_ICON_SIZE_LARGE_TOOLBAR);
	app_state->prev_button = gtk_button_new_from_icon_name("go-previous-symbolic", GTK_ICON_SIZE_LARGE_TOOLBAR);
	g_signal_connect(app_state->next_button, "clicked", G_CALLBACK(on_next_clicked), app_state);
	g_signal_connect(app_state->prev_button, "clicked", G_CALLBACK(on_prev_clicked), app_state);

	gtk_widget_set_valign(app_state->next_button, GTK_ALIGN_CENTER);
	gtk_widget_set_halign(app_state->next_button, GTK_ALIGN_END);
	gtk_widget_set_valign(app_state->prev_button, GTK_ALIGN_CENTER);
	gtk_widget_set_halign(app_state->prev_button, GTK_ALIGN_START);

	// Assign custom CSS classes to buttons
	gtk_style_context_add_class(gtk_widget_get_style_context(app_state->next_button), "nav-button");
	gtk_style_context_add_class(gtk_widget_get_style_context(app_state->prev_button), "nav-button");

	gtk_overlay_add_overlay(GTK_OVERLAY(app_state->overlay), app_state->next_button);
	gtk_overlay_add_overlay(GTK_OVERLAY(app_state->overlay), app_state->prev_button);

	// Initialize loading spinner
	app_state->loading_spinner = gtk_spinner_new();
	gtk_widget_set_halign(app_state->loading_spinner, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(app_state->loading_spinner, GTK_ALIGN_CENTER);
	gtk_overlay_add_overlay(GTK_OVERLAY(app_state->overlay), app_state->loading_spinner);
	gtk_widget_set_visible(app_state->loading_spinner, FALSE); // Initially hidden

	// Crear la tira de miniaturas
	app_state->thumbnails_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	app_state->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(app_state->scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
	gtk_container_add(GTK_CONTAINER(app_state->scrolled_window), app_state->thumbnails_box);
	gtk_box_pack_end(GTK_BOX(main_vbox), app_state->scrolled_window, FALSE, FALSE, 0);
	gtk_widget_set_size_request(app_state->scrolled_window, -1, 96); // Altura fija para miniaturas
	gtk_widget_hide(app_state->scrolled_window);					 // Ocultar al inicio

	gtk_widget_show_all(app_state->window);
	// Initially hide navigation buttons until images are loaded
	gtk_widget_hide(app_state->next_button);
	gtk_widget_hide(app_state->prev_button);

	gtk_main();

	return 0;
}
