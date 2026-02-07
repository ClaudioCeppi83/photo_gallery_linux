# Photo Gallery Linux (Rust) 🦀

Modern, memory-safe photo gallery for Linux built with **Rust**, **GTK4**, and **Libadwaita**.

## Overview

Photo Gallery is a lightweight and fluid application designed for the GNOME desktop. Rewritten from scratch in Rust, it provides a stable and fast experience for browsing local image collections.

## Features

* **Modern UI**: Built with Libadwaita for a native GNOME 45+ look and feel.
* **Memory Safety**: Written 100% in Rust, eliminating segmentation faults and memory leaks.
* **Dynamic Scanning**: Efficiently scans directories for images (supports JPG, PNG, WEBP, BMP, etc.).
* **Thumbnail Strip**: Interactive horizontal strip for direct image selection.
* **Adaptive Design**: Window and layout adjust gracefully to different sizes.
* **Dark Mode Support**: Automatically respects system-wide dark/light theme preferences.

## Installation

### Prerequisites

You will need the following installed on your system:

* **Rust** (latest stable via `rustup`)
* **GTK 4** development libraries (e.g., `libgtk-4-dev` on Ubuntu)
* **Libadwaita** development libraries (e.g., `libadwaita-1-dev` on Ubuntu)
* **GCC** or another C linker.

### Build Instructions

1. **Clone the repository**:

   ```sh
   git clone https://github.com/ClaudioCeppi83/photo_gallery_linux.git
   cd photo_gallery_linux
   ```

2. **Run the application**:

   ```sh
   cargo run
   ```

3. **Build for production**:

   ```sh
   cargo build --release
   ```

   The binary will be located at `./target/release/photo_gallery_rs`.

## Code Structure

The application follows the **MVVM (Model-View-ViewModel)** architectural pattern:

* `src/main.rs`: Entry point, application initialization, and connection between View and ViewModel.
* `src/model/`: Contains data structures and core business logic (e.g., image loading, directory scanning).
* `src/view/`: UI components and layout definitions using GTK4/Libadwaita.
* `src/view_model/`: Bridge between Model and View, handling user interactions and state updates.
* `legacy_c/`: Contains the original C implementation for historical reference.
* `Cargo.toml`: Project metadata and Rust dependencies.

## License

This project is licensed under the **GNU Affero General Public License (AGPL) v3.0**. See the [LICENSE](LICENSE) file for details.

---

### History

The project was originally developed in C (GTK3) and was modernized and rewritten in Rust in early 2026 to ensure long-term stability and easier integration of future AI features.