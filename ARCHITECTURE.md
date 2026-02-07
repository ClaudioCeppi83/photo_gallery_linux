# Architecture: Photo Gallery Linux 🦀

This project is a modern rewrite of a legacy C photo gallery, using **Rust**, **GTK4**, and **Libadwaita**. It follows the **Model-View-ViewModel (MVVM)** architectural pattern to ensure separation of concerns and maintainability.

## Architectural Pattern: MVVM

### 1. Model (`src/model/`)

The Model layer handles the core data and business logic of the application.

- **`gallery.rs`**: Manages the list of images, directory scanning using `walkdir`, and state (current index, zoomed state).
- **Responsibilities**: File system interaction, image metadata extraction, and maintaining the "source of truth" for the gallery content.

### 2. View (`src/view/`)

The View layer is responsible for the UI structure and presentation.

- **`mod.rs`**: Defines the main `GalleryWindow` using Libadwaita components.
- **Components**: `AdwWindow`, `GtkPicture` for the main image, and a `GtkBox` for the thumbnail strip.
- **Responsibilities**: Layout definition, widget styling, and exposing UI elements for interaction.

### 3. ViewModel (`src/view_model/`)

The ViewModel acts as the bridge between the View and the Model.

- **`gallery_view_model.rs`**: Processes user interactions (next, previous, zoom) and updates the Model. It then propagates changes back to the View.
- **Responsibilities**: Command handling, state coordination, and format conversion (e.g., loading `GdkPixbuf` for the UI).

## Data Flow

1. User clicks a button (View).
2. The View triggers a signal connected to a ViewModel method.
3. The ViewModel updates the Model state.
4. The ViewModel observes Model changes and updates the View's widgets (e.g., updating the displayed image).

## Legacy Reference

The `legacy_c/` directory contains the original C/GTK3 implementation. It serves as a reference for original features but is not part of the active build system.

## Future Plans

- **AI Integration**: The MVVM structure is designed to easily plug in AI models (e.g., for image description or wine pairing) into the ViewModel/Model layers.
- **Enhanced Caching**: Implementation of a more sophisticated image caching system in the Model layer.
