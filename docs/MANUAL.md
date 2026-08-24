# OpenWordPad - User Manual & Technical Documentation

**Author**: Leonardo Corato  
**License**: GNU General Public License v2.0 (GPLv2)  
**Version**: 1.3.1  

---

## 1. Introduction
OpenWordPad is a faithful, pixel-precise recreation of the iconic Microsoft WordPad text editor. Built with modern C++20 and Qt 6, OpenWordPad brings back the simplicity, speed, and clean interface of classic desktop document editing to Linux, Windows, and macOS.

---

## 2. Key Features

### 2.1 The Ribbon Interface
OpenWordPad utilizes the classic ribbon layout divided into functional tabs and command groups:
- **Application (File) Menu**:
  - `New` (`Ctrl+N`): Create a fresh document.
  - `Open` (`Ctrl+O`): Open RTF, DOCX, ODT, TXT, or HTML files.
  - `Save` (`Ctrl+S`) / `Save As`: Save to rich text or export to standard office formats.
  - `Print` (`Ctrl+P`) / `Quick Print` / `Print Preview`: Full print pipeline support.
  - `Page Setup`: Adjust paper size, orientation, and margins.
  - `Send in e-mail`: Forward the document via your default email client.
  - `About WordPad`: View version, author (Leonardo Corato), and license details.
- **Home Tab**:
  - *Clipboard*: Paste, Paste Special, Cut, Copy.
  - *Font*: Font Family, Font Size, Grow/Shrink Font, Bold (`Ctrl+B`), Italic (`Ctrl+I`), Underline (`Ctrl+U`), Strikethrough, Subscript, Superscript, Text Color, Text Highlight Color.
  - *Paragraph*: Decrease Indent, Increase Indent, Bullet/Numbered/Roman Lists, Line Spacing (1.0 to 2.0 + 10pt space after paragraph), Alignments (Left, Center, Right, Justify), Paragraph Properties dialog.
  - *Insert*: Insert Picture, Change Picture, Resize Picture, Paint Drawing (built-in sketch canvas), Date and Time, Insert Object.
  - *Editing*: Find (`Ctrl+F`), Replace (`Ctrl+H`), Select All (`Ctrl+A`).
- **View Tab**:
  - *Zoom*: Zoom in, Zoom out, 100% zoom.
  - *Show or Hide*: Toggle Ruler, Toggle Status bar.
  - *Settings*: Word Wrap modes (No wrap, Wrap to window, Wrap to ruler), Measurement units (Inches, Centimeters, Points, Picas).

### 2.2 Interactive Measurement Ruler
- Drag First Line Indent (top triangle marker).
- Drag Left Indent / Hanging Indent (bottom left triangle and rectangle markers).
- Drag Right Margin Indent (bottom right triangle marker).
- Switch dynamically between Centimeters, Inches, Points, and Picas.

### 2.3 Status Bar & Zoom Controls
- Real-time line and column tracker.
- Selected character count indicator.
- Interactive Zoom Slider from 10% to 500% with zoom menu shortcuts.

---

## 3. Supported File Formats

| Format | Extension | Read Support | Write Support |
| :--- | :--- | :---: | :---: |
| Rich Text Format | `.rtf` | Yes (Native) | Yes (Native) |
| Office Open XML | `.docx` | Yes | Yes |
| OpenDocument Text | `.odt` | Yes | Yes |
| Plain Text (UTF-8/ANSI) | `.txt`, `.text`, `.log` | Yes | Yes |
| HTML Document | `.html`, `.htm` | Yes | Yes |
| PDF Document | `.pdf` | N/A | Yes (Export) |

---

## 4. Building & Installation

### Requirements:
- C++20 compatible compiler (GCC 11+, Clang 13+, or MSVC 2022)
- Qt 6.5+ (Widgets, Core, Gui, PrintSupport, Svg)
- CMake 3.16+
- Ninja build system (optional, recommended)

### Build Instructions:
```bash
# Clone the repository
git clone https://github.com/leocorato/openwordpad.git
cd openwordpad

# Configure and compile
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C build

# Run OpenWordPad
./build/openwordpad

# Run automated test suite
./build/test_engine
```

### Installation:
```bash
sudo ninja -C build install
```
