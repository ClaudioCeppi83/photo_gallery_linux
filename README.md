write all this in a single readme markdown:

# Photo Gallery v0.1

## Overview

Photo Gallery is a simple GTK-based application that allows users to browse images stored in a specified directory. The application provides basic navigation features to view images and displays a message if no images are found in the selected directory.

## Features

- **Directory Selection**: Users can choose a directory containing images.
- **Image Navigation**: Users can navigate through images using "Next" and "Previous" buttons.
- **Message Display**: If no images are found in the selected directory, a message dialog is displayed.
- **Window Title Update**: The window title is updated to show the current directory.

## Installation

### Prerequisites

- GTK+ 3.0 or later
- GCC (GNU Compiler Collection)
- Make

### Build Instructions

1. **Clone the repository**:
   ```sh
   git clone https://github.com/yourusername/photo-gallery.git
   cd photo-gallery

2. **Compile the program**:
make

3. **Run the program**:
./photo_gallery

## Usage:

1. **Choose Directory**: Click the "Choose Directory" button to select a directory containing images.

2. **Navigate Images**: Use the "Next" and "Previous" buttons to navigate through the images.

**Note**: If no images are found in the selected directory, a message dialog will appear.

## Code Structure:

**gallery.h**: Header file containing function prototypes and global variables.

**gallery.c**: Source file containing the main logic for loading images, navigation, and directory selection.

**interface.h**: Header file for the user interface.

**interface.c**: Source file for the user interface.

**main.c**: Main entry point of the application.

**Makefile**: Makefile for compiling the program.

## Known Issues:

The application currently does not handle large directories efficiently.

Error handling for file operations is minimal.

## Future Work:

Implement a directory tree view for easier navigation.

Add support for more image formats.

Improve error handling and user feedback.
