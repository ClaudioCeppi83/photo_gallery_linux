use std::path::PathBuf;

/// Represents the data model for the photo gallery.
pub struct GalleryModel {
    pub image_paths: Vec<PathBuf>,
    pub current_index: usize,
    pub zoom_level: f64,
}

impl GalleryModel {
    pub fn new() -> Self {
        Self {
            image_paths: Vec::new(),
            current_index: 0,
            zoom_level: 1.0,
        }
    }

    pub fn set_images(&mut self, paths: Vec<PathBuf>, initial_index: usize) {
        self.image_paths = paths;
        self.current_index = initial_index;
    }

    pub fn next(&mut self) {
        if !self.image_paths.is_empty() {
            self.current_index = (self.current_index + 1) % self.image_paths.len();
        }
    }

    pub fn previous(&mut self) {
        let len = self.image_paths.len();
        if len > 0 {
            self.current_index = if self.current_index == 0 {
                len - 1
            } else {
                self.current_index - 1
            };
        }
    }

    pub fn set_zoom(&mut self, level: f64) {
        self.zoom_level = level.max(0.1);
    }
}
