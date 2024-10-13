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
   git@github.com:ClaudioCeppi83/photo_gallery_linux.git
   cd photo_gallery_linux

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

## License

This project is licensed under the **GNU Affero General Public License (AGPL) v3.0**. See the [full text of the GNU Affero General Public License (AGPL) v3.0](https://www.gnu.org/licenses/agpl-3.0.txt) for details.

### GNU Affero General Public License (AGPL) v3.0

The GNU Affero General Public License (AGPL) is a strong copyleft license that ensures the software remains free and open-source. It requires that any modifications or derivative works be shared under the same license. Additionally, it includes a provision that requires users who interact with the software over a network to have access to the source code.

### Additional Terms

1. **Philosophy and Brand Image**: The philosophy and brand image of this software must be maintained in any derivative works. Any changes to the software's core philosophy or brand image require explicit written permission from the original author.

2. **Naming Convention**: The name of the software may not be changed without explicit written permission from the original author. If you create a variation of this software, you must use a suffix to differentiate it from the original. For example, "Photo Gallery Plus" or "Photo Gallery Extended".

### No Warranty

This software is provided "AS IS", without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages, or other liability, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.
