#include "gallery.h"
#include "interface.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Load the last selected directory from the configuration file
    FILE *fp = fopen("last_directory.conf", "r");
    if (fp) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), fp)) {
            buffer[strcspn(buffer, "\n")] = 0; // Remove newline character
            last_directory = g_strdup(buffer);
        }
        fclose(fp);
    } else {
        // Default to the user's Pictures directory
        last_directory = g_strdup(g_get_user_special_dir(G_USER_DIRECTORY_PICTURES));
    }

    // Get the screen resolution
    GdkDisplay *display = gdk_display_get_default();
    if (display == NULL) {
        g_print("Failed to get default display.\n");
        return 1;
    }

    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
    if (monitor == NULL) {
        // Fallback to the first monitor if the primary monitor cannot be obtained
        monitor = gdk_display_get_monitor(display, 0);
        if (monitor == NULL) {
            g_print("Failed to get any monitor.\n");
            return 1;
        }
    }

    GdkRectangle geometry;
    gdk_monitor_get_geometry(monitor, &geometry);

    // Calculate 60% of the screen resolution
    int window_width = geometry.width * 0.6;
    int window_height = geometry.height * 0.6;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Photo Gallery");
    gtk_window_set_default_size(GTK_WINDOW(window), window_width, window_height);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_hexpand(drawing_area, TRUE);
    gtk_widget_set_vexpand(drawing_area, TRUE);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);
    gtk_box_pack_start(GTK_BOX(box), drawing_area, TRUE, TRUE, 0);

    button_next = gtk_button_new_with_label("Next");
    g_signal_connect(button_next, "clicked", G_CALLBACK(on_next_clicked), drawing_area);
    gtk_box_pack_start(GTK_BOX(box), button_next, FALSE, FALSE, 0);

    button_prev = gtk_button_new_with_label("Previous");
    g_signal_connect(button_prev, "clicked", G_CALLBACK(on_prev_clicked), drawing_area);
    gtk_box_pack_start(GTK_BOX(box), button_prev, FALSE, FALSE, 0);

    button_choose_dir = gtk_button_new_with_label("Choose Directory");
    g_signal_connect(button_choose_dir, "clicked", G_CALLBACK(on_choose_dir_clicked), drawing_area);
    gtk_box_pack_start(GTK_BOX(box), button_choose_dir, FALSE, FALSE, 0);

    button_zoom_in = gtk_button_new_with_label("Zoom In");
    g_signal_connect(button_zoom_in, "clicked", G_CALLBACK(on_zoom_in_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box), button_zoom_in, FALSE, FALSE, 0);

    button_zoom_out = gtk_button_new_with_label("Zoom Out");
    g_signal_connect(button_zoom_out, "clicked", G_CALLBACK(on_zoom_out_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box), button_zoom_out, FALSE, FALSE, 0);

    load_images(last_directory);
    if (num_images > 0) {
        current_pixbuf = gdk_pixbuf_new_from_file(image_files[current_image_index], NULL);
        gtk_widget_queue_draw(drawing_area);
    } else {
        g_print("No images found in the directory.\n");
    }

    gtk_widget_show_all(window);
    gtk_main();

    // Free allocated memory
    for (int i = 0; i < num_images; i++) {
        g_free(image_files[i]);
    }
    g_free(image_files);
    g_free(last_directory);
    g_object_unref(current_pixbuf);

    return 0;
}
