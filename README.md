# Photo Gallery v0.2.5-alpha

## Overview

Photo Gallery is a simple GTK-based application that allows users to browse images stored in a specified directory. This version introduces significant improvements in stability, memory management, error handling, and user experience.

## Features

* **Directory Selection**: Users can choose a directory containing images.
* **Image Navigation**: Users can navigate through images using "Next" and "Previous" buttons.
* **Image Zoom**: Users can zoom in and out of images.
* **Thumbnail Strip**: A modern horizontal strip of image thumbnails is displayed at the bottom of the window, allowing direct selection of any image by clicking its thumbnail.
* **Selection Highlight**: The currently selected image's thumbnail is visually highlighted for clarity.
* **Message Display**: Displays "No image available" text directly in the viewing area if no images are found or if an image fails to load.
* **Window Title Update**: The window title is updated to show the current directory.
* **Dynamic Image Format Support**: Automatically detects and supports all image formats recognized by GdkPixbuf.
* **Asynchronous Loading**: Scans directories asynchronously to keep the UI responsive.
* **Enhanced Stability**: Fixed critical memory management bugs and segmentation faults, especially when selecting directories and navigating images.
* **Efficient Memory Management**: Utilizes `GPtrArray` for optimized handling of image file lists.
* **Robust Callbacks**: Navigation and thumbnail selection callbacks now use a dedicated struct to ensure safe access to application state, preventing segmentation faults.
* **Modular Code Structure**: Encapsulates application state in a dedicated structure, enhancing code organization and maintainability.

## Installation

### Prerequisites

- GTK+ 3.0 or later
- GCC (GNU Compiler Collection)
- Make

### Build Instructions

1.  **Clone the repository**:
    ```sh
    git clone https://github.com/ClaudioCeppi83/photo_gallery_linux.git
    cd photo_gallery_linux
    ```

2.  **Compile the program**:
    ```sh
    make
    ```

3.  **Run the program**:
    ```sh
    ./photo_gallery
    ```

## Usage

1.  **Choose Directory**: Click the "Choose Directory" button to select a directory containing images.
    *   **Note**: If no images are found in the selected directory, or if an image cannot be loaded, a "No image available" message will be displayed in the viewing area.

2.  **Thumbnail Strip**: Select any image directly by clicking its thumbnail in the strip at the bottom of the window. The currently selected image is highlighted.

3.  **Navigate Images**: Use the "Next" and "Previous" buttons to navigate through the images sequentially.

4.  **Zoom Images**: Use the "Zoom In" and "Zoom Out" buttons to adjust the image view.

## Code Structure

* **`gallery.h`**: Header file containing the `AppState` structure definition and function prototypes for core gallery logic.
* **`gallery.c`**: Source file containing the main logic for loading images, navigation, directory selection, and thumbnail strip management. Includes robust callback handling for navigation and thumbnail selection.
* **`interface.h`**: Header file for the user interface components and related function prototypes.
* **`interface.c`**: Source file for handling user interface interactions, including image drawing and zoom functionality.
* **`main.c`**: Main entry point of the application, responsible for initializing GTK, setting up the `AppState`, and connecting signals.
* **`Makefile`**: Makefile for compiling the program.

## Known Issues

* **Resolved**: The critical issue causing the application to crash upon selecting a directory and segmentation faults during navigation/thumbnail selection have been fixed by robust pointer management and callback design.
* Initial scanning of very large directories might still cause a brief delay, although the UI remains responsive due to asynchronous processing.

## Future Work

- Implement a directory tree view for easier navigation.

## Changelog

### v0.2.5-alpha (2025-09-08)

- **Features:**
    - Added a modern horizontal thumbnail strip at the bottom of the window for direct image selection.
    - Thumbnails are highlighted to indicate the currently selected image.
- **Improvements:**
    - Navigation and thumbnail selection callbacks now use a dedicated struct for safe access to application state, preventing segmentation faults.
    - Improved user experience and UI/UX consistency with GNOME/Ubuntu guidelines.
- **Bug Fixes:**
    - Fixed segmentation fault when clicking thumbnails or navigating images due to unsafe pointer access.

### v0.2.4-alpha (2025-09-08)

- **Bug Fixes:**
    - Fixed a critical segmentation fault that occurred when selecting a directory, caused by improper memory management during image loading and application shutdown.
    - Corrected compilation errors by ensuring all necessary members of the `AppState` struct were accessible.
- **Improvements:**
    - Significantly improved application stability and reliability.

### v0.2.3-alpha (2025-09-08)

- **Improvements:**
    - Implemented asynchronous directory scanning to improve UI responsiveness when selecting large directories. A loading spinner is now displayed during the scan.

### v0.2.2-alpha (2025-09-08)

- **Bug Fixes:**
    - Resolved issue where application failed to recognize and load image files due to incorrect GHashTable key management in `load_images` function.

### v0.2.1-alpha (2025-09-08)

- **Features:**
    - Implemented dynamic detection and support for all GdkPixbuf-recognized image formats.
    - Added image zoom functionality (Zoom In/Out).
- **Improvements:**
    - Refactored `load_images` for efficient memory management using `GPtrArray` and improved error handling.
    - Encapsulated all global variables into a new `AppState` structure for better code organization.
    - Enhanced error handling for image loading, preventing application crashes on corrupted or unsupported image files.
    - Improved user feedback by displaying "No image available" text when images cannot be loaded.
    - Added a `.gitignore` file to exclude build artifacts and temporary files.
- **Bug Fixes:**
    - Addressed potential crashes related to `NULL` pixbuf pointers during image loading and drawing.

## License

This project is licensed under the **GNU Affero General Public License (AGPL) v3.0**. See the [full text of the GNU Affero General Public License (AGPL) v3.0](https://www.gnu.org/licenses/agpl-3.0.txt) for details.

### GNU Affero General Public License (AGPL) v3.0

The GNU Affero General Public License (AGPL) is a strong copyleft license that ensures the software remains free and open-source. It requires that any modifications or derivative works be shared under the same license. Additionally, it includes a provision that requires users who interact with the software over a network to have access to the source code.

### Additional Terms

1.  **Philosophy and Brand Image**: The philosophy and brand image of this software must be maintained in any derivative works. Any changes to the software's core philosophy or brand image require explicit written permission from the original author.

2.  **Naming Convention**: The name of the software may not be changed without explicit written permission from the original author. If you create a variation of this software, you must use a suffix to differentiate it from the original. For example, "Photo Gallery Plus" or "Photo Gallery Extended".

### No Warranty

This software is provided "AS IS", without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages, or other liability, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.