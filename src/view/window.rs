use adw::prelude::*;
use gtk4 as gtk;
use libadwaita as adw;

pub struct GalleryWindow {
    pub window: adw::ApplicationWindow,
    pub picture: gtk::Picture,
    pub picture_scrolled: gtk::ScrolledWindow,
    pub thumbnails_box: gtk::Box,
    pub next_btn: gtk::Button,
    pub prev_btn: gtk::Button,
    pub zoom_in_btn: gtk::Button,
    pub zoom_out_btn: gtk::Button,
    pub open_btn: gtk::Button,
}

impl GalleryWindow {
    pub fn new(app: &adw::Application) -> Self {
        let open_btn = gtk::Button::builder()
            .icon_name("folder-open-symbolic")
            .tooltip_text("Open Folder")
            .build();

        let zoom_out_btn = gtk::Button::builder()
            .icon_name("zoom-out-symbolic")
            .tooltip_text("Zoom Out")
            .build();

        let zoom_in_btn = gtk::Button::builder()
            .icon_name("zoom-in-symbolic")
            .tooltip_text("Zoom In")
            .build();

        let next_btn = gtk::Button::builder()
            .icon_name("go-next-symbolic")
            .sensitive(false)
            .build();

        let prev_btn = gtk::Button::builder()
            .icon_name("go-previous-symbolic")
            .sensitive(false)
            .build();

        let header_bar = adw::HeaderBar::new();
        header_bar.pack_start(&open_btn);
        header_bar.pack_start(&zoom_out_btn);
        header_bar.pack_start(&zoom_in_btn);
        header_bar.pack_end(&next_btn);
        header_bar.pack_end(&prev_btn);

        let picture = gtk::Picture::builder()
            .hexpand(true)
            .vexpand(true)
            .can_shrink(true)
            .build();

        let picture_scrolled = gtk::ScrolledWindow::builder()
            .hexpand(true)
            .vexpand(true)
            .child(&picture)
            .build();

        let thumbnails_box = gtk::Box::new(gtk::Orientation::Horizontal, 5);
        let thumbnails_scrolled = gtk::ScrolledWindow::builder()
            .hscrollbar_policy(gtk::PolicyType::Always)
            .vscrollbar_policy(gtk::PolicyType::Never)
            .min_content_height(100)
            .child(&thumbnails_box)
            .build();

        let main_box = gtk::Box::new(gtk::Orientation::Vertical, 0);
        main_box.append(&header_bar);
        main_box.append(&picture_scrolled);
        main_box.append(&thumbnails_scrolled);

        let window = adw::ApplicationWindow::builder()
            .application(app)
            .title("Photo Gallery")
            .default_width(900)
            .default_height(600)
            .content(&main_box)
            .build();

        Self {
            window,
            picture,
            picture_scrolled: picture_scrolled.clone(),
            thumbnails_box,
            next_btn,
            prev_btn,
            zoom_in_btn,
            zoom_out_btn,
            open_btn,
        }
    }

    pub fn show(&self) {
        self.window.present();
    }
}
