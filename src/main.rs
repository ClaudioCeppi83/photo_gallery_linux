mod model;
mod view;
mod view_model;

use adw::prelude::*;
use libadwaita as adw;
use view::GalleryWindow;
use view_model::GalleryViewModel;
use std::rc::Rc;

fn main() {
    let application = adw::Application::builder()
        .application_id("org.claudioceppi.PhotoGallery")
        .build();

    application.connect_activate(build_ui);
    application.run();
}

fn build_ui(app: &adw::Application) {
    let view = Rc::new(GalleryWindow::new(app));
    let vm = Rc::new(GalleryViewModel::new(
        view.picture.clone(),
        view.picture_scrolled.clone(),
        view.thumbnails_box.clone(),
        view.next_btn.clone(),
        view.prev_btn.clone(),
    ));

    // Connect View signals to ViewModel actions
    view.next_btn.connect_clicked(glib::clone!(
        #[strong]
        vm,
        move |_| {
            vm.next();
        }
    ));

    view.prev_btn.connect_clicked(glib::clone!(
        #[strong]
        vm,
        move |_| {
            vm.previous();
        }
    ));

    view.zoom_in_btn.connect_clicked(glib::clone!(
        #[strong]
        vm,
        move |_| {
            vm.zoom_in();
        }
    ));

    view.zoom_out_btn.connect_clicked(glib::clone!(
        #[strong]
        vm,
        move |_| {
            vm.zoom_out();
        }
    ));

    view.open_btn.connect_clicked(glib::clone!(
        #[strong]
        view,
        #[strong]
        vm,
        move |_| {
            let file_chooser = gtk4::FileChooserDialog::new(
                Some("Open Photo or Folder"),
                Some(&view.window),
                gtk4::FileChooserAction::Open,
                &[
                    ("_Cancel", gtk4::ResponseType::Cancel),
                    ("_Open", gtk4::ResponseType::Accept),
                ],
            );

            file_chooser.connect_response(glib::clone!(
                #[strong]
                vm,
                move |dialog, response| {
                    if response == gtk4::ResponseType::Accept {
                        if let Some(file) = dialog.file() {
                            if let Some(mut path) = file.path() {
                                let mut selected_file = None;
                                if path.is_file() {
                                    selected_file = Some(path.clone());
                                    if let Some(parent) = path.parent() {
                                        path = parent.to_path_buf();
                                    }
                                }
                                vm.scan_directory(path, selected_file);
                            }
                        }
                    }
                    dialog.destroy();
                }
            ));

            file_chooser.show();
        }
    ));

    view.show();
}
