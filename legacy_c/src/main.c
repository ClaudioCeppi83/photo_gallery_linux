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

static void on_activate(AdwApplication *app, gpointer user_data)
{
	AppState *app_state = (AppState *)user_data;

	// Create the main window
	app_state->window = adw_application_window_new(GTK_APPLICATION(app));
	gtk_window_set_title(GTK_WINDOW(app_state->window), "Photo Gallery");
	gtk_window_set_default_size(GTK_WINDOW(app_state->window), 900, 600);

	g_object_set_data_full(G_OBJECT(app_state->window), "app_state", app_state, NULL);

	// Create HeaderBar
	GtkWidget *header_bar = adw_header_bar_new();
	
	// Add "Choose Directory" button to the header bar
	GtkWidget *button_choose_dir = gtk_button_new_from_icon_name("folder-open-symbolic");
	g_signal_connect(button_choose_dir, "clicked", G_CALLBACK(on_choose_dir_clicked), app_state);
	adw_header_bar_pack_start(ADW_HEADER_BAR(header_bar), button_choose_dir);

	// Add "Zoom" buttons to the header bar
	GtkWidget *zoom_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_add_css_class(zoom_box, "linked");
	GtkWidget *button_zoom_in = gtk_button_new_from_icon_name("zoom-in-symbolic");
	GtkWidget *button_zoom_out = gtk_button_new_from_icon_name("zoom-out-symbolic");
	g_signal_connect(button_zoom_in, "clicked", G_CALLBACK(on_zoom_in_clicked), app_state);
	g_signal_connect(button_zoom_out, "clicked", G_CALLBACK(on_zoom_out_clicked), app_state);
	gtk_box_append(GTK_BOX(zoom_box), button_zoom_in);
	gtk_box_append(GTK_BOX(zoom_box), button_zoom_out);
	adw_header_bar_pack_end(ADW_HEADER_BAR(header_bar), zoom_box);

	// Principal box using AdwToolbarView
	GtkWidget *toolbar_view = adw_toolbar_view_new();
	adw_toolbar_view_add_top_bar(ADW_TOOLBAR_VIEW(toolbar_view), header_bar);
	adw_application_window_set_content(ADW_APPLICATION_WINDOW(app_state->window), toolbar_view);

	// Vertical box for content
	GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(toolbar_view), main_vbox);

	// Overlay for main image
	app_state->overlay = gtk_overlay_new();
	gtk_widget_set_vexpand(app_state->overlay, TRUE);
	gtk_box_append(GTK_BOX(main_vbox), app_state->overlay);

	app_state->picture = gtk_picture_new();
	gtk_widget_set_hexpand(app_state->picture, TRUE);
	gtk_widget_set_vexpand(app_state->picture, TRUE);
	gtk_overlay_set_child(GTK_OVERLAY(app_state->overlay), app_state->picture);

	// Navigation Buttons
	app_state->next_button = gtk_button_new_from_icon_name("go-next-symbolic");
	app_state->prev_button = gtk_button_new_from_icon_name("go-previous-symbolic");
	gtk_widget_add_css_class(app_state->next_button, "circular");
	gtk_widget_add_css_class(app_state->prev_button, "circular");
	
	g_signal_connect(app_state->next_button, "clicked", G_CALLBACK(on_next_clicked), app_state);
	g_signal_connect(app_state->prev_button, "clicked", G_CALLBACK(on_prev_clicked), app_state);

	gtk_widget_set_valign(app_state->next_button, GTK_ALIGN_CENTER);
	gtk_widget_set_halign(app_state->next_button, GTK_ALIGN_END);
	gtk_widget_set_valign(app_state->prev_button, GTK_ALIGN_CENTER);
	gtk_widget_set_halign(app_state->prev_button, GTK_ALIGN_START);

	// Set margins for buttons
	gtk_widget_set_margin_end(app_state->next_button, 12);
	gtk_widget_set_margin_start(app_state->prev_button, 12);

	gtk_overlay_add_overlay(GTK_OVERLAY(app_state->overlay), app_state->next_button);
	gtk_overlay_add_overlay(GTK_OVERLAY(app_state->overlay), app_state->prev_button);

	// Initialize loading spinner
	app_state->loading_spinner = gtk_spinner_new();
	gtk_widget_set_halign(app_state->loading_spinner, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(app_state->loading_spinner, GTK_ALIGN_CENTER);
	gtk_overlay_add_overlay(GTK_OVERLAY(app_state->overlay), app_state->loading_spinner);
	gtk_widget_set_visible(app_state->loading_spinner, FALSE);

	// Thumbnail strip
	app_state->thumbnails_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	app_state->scrolled_window = gtk_scrolled_window_new();
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(app_state->scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(app_state->scrolled_window), app_state->thumbnails_box);
	
	adw_toolbar_view_add_bottom_bar(ADW_TOOLBAR_VIEW(toolbar_view), app_state->scrolled_window);
	gtk_widget_set_size_request(app_state->scrolled_window, -1, 110);
	gtk_widget_set_visible(app_state->scrolled_window, FALSE);

	// Initially hide navigation buttons
	gtk_widget_set_visible(app_state->next_button, FALSE);
	gtk_widget_set_visible(app_state->prev_button, FALSE);

	gtk_window_present(GTK_WINDOW(app_state->window));
}

int main(int argc, char *argv[])
{
	AdwApplication *app;
	int status;

	// Create the application state structure
	AppState *app_state = g_new0(AppState, 1);
	app_state->zoom_level = 1.0;
	app_state->current_image_index = 0;

	// Load the last selected directory
	FILE *fp = fopen("last_directory.conf", "r");
	if (fp)
	{
		char buffer[256];
		if (fgets(buffer, sizeof(buffer), fp))
		{
			buffer[strcspn(buffer, "\n")] = 0;
			app_state->last_directory = g_strdup(buffer);
		}
		fclose(fp);
	}
	else
	{
		app_state->last_directory = g_strdup(g_get_user_special_dir(G_USER_DIRECTORY_PICTURES));
	}

	app = adw_application_new("org.claudioceppi.PhotoGallery", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(on_activate), app_state);
	g_signal_connect_swapped(app, "shutdown", G_CALLBACK(free_app_state), app_state);

	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}