use crate::model::GalleryModel;
use gtk::prelude::*;
use gtk4 as gtk;
use std::cell::RefCell;
use std::path::{Path, PathBuf};
use std::rc::Rc;
use std::sync::mpsc;
use std::thread;

pub struct GalleryViewModel {
    pub model: Rc<RefCell<GalleryModel>>,
    // Widgets that need to be updated directly from VM logic
    pub picture: gtk::Picture,
    pub picture_scrolled: gtk::ScrolledWindow,
    pub thumbnails_box: gtk::Box,
    pub next_btn: gtk::Button,
    pub prev_btn: gtk::Button,
}

impl GalleryViewModel {
    pub fn new(
        picture: gtk::Picture,
        picture_scrolled: gtk::ScrolledWindow,
        thumbnails_box: gtk::Box,
        next_btn: gtk::Button,
        prev_btn: gtk::Button,
    ) -> Self {
        Self {
            model: Rc::new(RefCell::new(GalleryModel::new())),
            picture,
            picture_scrolled,
            thumbnails_box,
            next_btn,
            prev_btn,
        }
    }

    pub fn scan_directory(&self, path: PathBuf, selected_file: Option<PathBuf>) {
        let (sender, receiver) = mpsc::channel::<(Vec<PathBuf>, usize)>();
        let _ = self.model.clone();
        
        // Clone pointers for the timeout
        let picture = self.picture.clone();
        let picture_scrolled = self.picture_scrolled.clone();
        let thumbnails_box = self.thumbnails_box.clone();
        let next_btn = self.next_btn.clone();
        let prev_btn = self.prev_btn.clone();
        let vm_model = self.model.clone();

        thread::spawn(move || {
            let images = Self::scan_worker(path);
            let mut found_index = 0;

            if let Some(selected) = selected_file {
                found_index = images.iter().position(|p| p == &selected).unwrap_or(0);
            }

            let _ = sender.send((images, found_index));
        });

        glib::timeout_add_local(
            std::time::Duration::from_millis(50),
            move || {
                if let Ok((images, index)) = receiver.try_recv() {
                    let mut m = vm_model.borrow_mut();
                    m.set_images(images, index);
                    
                    let has_images = !m.image_paths.is_empty();
                    next_btn.set_sensitive(has_images);
                    prev_btn.set_sensitive(has_images);

                    if has_images {
                        if let Some(path) = m.image_paths.get(m.current_index) {
                            picture.set_filename(Some(path));
                        }
                    } else {
                        picture.set_paintable(None::<&gtk::gdk::Paintable>);
                    }
                    
                    drop(m);
                    Self::rebuild_thumbnails_static(
                        vm_model.clone(), 
                        thumbnails_box.clone(), 
                        picture.clone(),
                        picture_scrolled.clone()
                    );
                    glib::ControlFlow::Break
                } else {
                    glib::ControlFlow::Continue
                }
            }
        );
    }

    pub fn next(&self) {
        let mut m = self.model.borrow_mut();
        m.next();
        self.update_view(&m);
    }

    pub fn previous(&self) {
        let mut m = self.model.borrow_mut();
        m.previous();
        self.update_view(&m);
    }

    pub fn zoom_in(&self) {
        let mut m = self.model.borrow_mut();
        m.zoom_level *= 1.2;
        self.apply_zoom(&m);
    }

    pub fn zoom_out(&self) {
        let mut m = self.model.borrow_mut();
        let new_zoom = m.zoom_level / 1.2;
        m.set_zoom(new_zoom);
        self.apply_zoom(&m);
    }

    fn update_view(&self, model: &GalleryModel) {
        if let Some(path) = model.image_paths.get(model.current_index) {
            self.picture.set_filename(Some(path));
        }
        self.update_highlights(model);
    }

    fn apply_zoom(&self, model: &GalleryModel) {
        let zoom = model.zoom_level;
        
        let haddj = self.picture_scrolled.hadjustment();
        let vaddj = self.picture_scrolled.vadjustment();

        // 1. Capture current state before change
        let old_upper_h = haddj.upper();
        let old_upper_v = vaddj.upper();
        let old_value_h = haddj.value();
        let old_value_v = vaddj.value();
        let page_h = haddj.page_size();
        let page_v = vaddj.page_size();

        // Calculate relative center (where the user is looking)
        let rel_x = (old_value_h + page_h / 2.0) / old_upper_h;
        let rel_y = (old_value_v + page_v / 2.0) / old_upper_v;

        // 2. Set new alignment and size
        // We use Align::Center to keep it pretty when smaller than view
        self.picture.set_halign(gtk::Align::Center);
        self.picture.set_valign(gtk::Align::Center);

        let (new_w, new_h) = if zoom <= 1.0 {
            self.picture.set_hexpand(true);
            self.picture.set_vexpand(true);
            self.picture.set_size_request(-1, -1);
            // We don't know the exact size yet, but GTK will fit it.
            // For zoom <= 1, centering at 0,0 or mid is usually fine.
            (page_h, page_v) 
        } else {
            self.picture.set_hexpand(false);
            self.picture.set_vexpand(false);
            let w = zoom * 1000.0;
            let h = zoom * 750.0;
            self.picture.set_size_request(w as i32, h as i32);
            (w, h)
        };

        // 3. Predict new upper bounds to avoid the "one-frame glitch"
        // GTK upper bound is the maximum of the child size and the viewport size.
        let new_upper_h = new_w.max(page_h);
        let new_upper_v = new_h.max(page_v);

        // 4. Apply new scroll values immediately
        let new_value_h = (rel_x * new_upper_h) - (page_h / 2.0);
        let new_value_v = (rel_y * new_upper_v) - (page_v / 2.0);

        haddj.set_value(new_value_h);
        vaddj.set_value(new_value_v);

        // 5. Also run in idle just in case the manual prediction was slightly off 
        // due to CSS padding/margins or scrollbar visibility changes.
        glib::idle_add_local(glib::clone!(
            #[strong] haddj,
            #[strong] vaddj,
            move || {
                let current_rel_x = (haddj.value() + haddj.page_size() / 2.0) / haddj.upper();
                // If it's significantly off, we adjust, but usually the immediate set fixed the glitch.
                let target_x = (rel_x * haddj.upper()) - (haddj.page_size() / 2.0);
                let target_y = (rel_y * vaddj.upper()) - (vaddj.page_size() / 2.0);
                haddj.set_value(target_x);
                vaddj.set_value(target_y);
                glib::ControlFlow::Break
            }
        ));
    }

    fn update_highlights(&self, model: &GalleryModel) {
        let mut i = 0;
        let mut child = self.thumbnails_box.first_child();

        while let Some(widget) = child {
            if i == model.current_index {
                widget.add_css_class("suggested-action");
            } else {
                widget.remove_css_class("suggested-action");
            }
            child = widget.next_sibling();
            i += 1;
        }
    }

    fn rebuild_thumbnails_static(
        model_rc: Rc<RefCell<GalleryModel>>, 
        box_widget: gtk::Box, 
        picture: gtk::Picture,
        picture_scrolled: gtk::ScrolledWindow
    ) {
        while let Some(child) = box_widget.first_child() {
            box_widget.remove(&child);
        }

        let model = model_rc.borrow();
        for (i, path) in model.image_paths.iter().enumerate().take(100) {
            let thumb = gtk::Image::builder()
                .file(path.to_str().unwrap_or_default())
                .pixel_size(80)
                .build();

            let btn = gtk::Button::builder()
                .child(&thumb)
                .css_classes(vec!["flat"])
                .build();

            let model_inner = model_rc.clone();
            let box_inner = box_widget.clone();
            let pic_inner = picture.clone();
            let scrolled_inner = picture_scrolled.clone();

            btn.connect_clicked(move |_| {
                let mut m = model_inner.borrow_mut();
                m.current_index = i;
                if let Some(p) = m.image_paths.get(m.current_index) {
                    pic_inner.set_filename(Some(p));
                }
                
                // Reset scroll and zoom on image change? Or keep zoom?
                // Let's reset scroll for a fresh view
                scrolled_inner.hadjustment().set_value(0.0);
                scrolled_inner.vadjustment().set_value(0.0);
                
                // Update highlights
                let mut idx = 0;
                let mut child = box_inner.first_child();
                while let Some(w) = child {
                    if idx == m.current_index {
                        w.add_css_class("suggested-action");
                    } else {
                        w.remove_css_class("suggested-action");
                    }
                    child = w.next_sibling();
                    idx += 1;
                }
            });

            if i == model.current_index {
                btn.add_css_class("suggested-action");
            }

            box_widget.append(&btn);
        }
    }

    fn scan_worker(path: PathBuf) -> Vec<PathBuf> {
        let mut images = Vec::new();
        if let Ok(entries) = std::fs::read_dir(path) {
            for entry in entries.flatten() {
                let path = entry.path();
                if is_supported_image(&path) {
                    images.push(path);
                }
            }
        }
        images.sort();
        images
    }
}


pub fn is_supported_image(path: &Path) -> bool {
    path.extension()
        .and_then(|ext| ext.to_str())
        .map(|s| match s.to_lowercase().as_str() {
            "jpg" | "jpeg" | "png" | "gif" | "webp" | "bmp" => true,
            _ => false,
        })
        .unwrap_or(false)
}
