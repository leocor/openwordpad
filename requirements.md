# OpenWordPad - Specification and Requirements Document

**Product Name**: OpenWordPad  
**Author**: Leonardo Corato  
**License**: GNU General Public License v2.0 (GPLv2)  
**Version**: 1.3.3  
**Target Framework**: Qt 6 (Qt 6.5+)  
**Language**: C++20  
**Target Platforms**: Linux (CachyOS / Arch / Ubuntu / Fedora), Windows (10/11), macOS (12+)  

---

## 1. Overview & Objective
Microsoft has deprecated and removed WordPad from Windows. The goal of OpenWordPad is to provide a 100% complete, faithful, high-performance, and cross-platform recreation of Microsoft WordPad using modern C++ and Qt6. It features the exact classic Windows Ribbon interface, ruler, rich formatting tools, format compatibility (RTF, ODT, DOCX, TXT, HTML, PDF), and a familiar user experience.

---

## 6. QA Verification & Test Suite
- **Automated Document Batch Test Suite** (`tests/test_batch_100.cpp`):
  - 100 diverse sample documents covering TXT, RTF (embedded pictures, color tables, unicode), DOCX (drawings, styles), HTML (tables, rich styling), and edge cases (empty, long lines, 1000 lines, nested brackets).
  - Verifies 0 crashes, memory safety, format parsing integrity, and round-trip export.
- **Engine Unit Tests** (`tests/test_engine.cpp`):
  - RTF parser & exporter unit tests.
  - DOCX zip archive & XML document parser unit tests.
  - Plain text & DocumentManager format handlers.

---

## 2. Functional Requirements

### 2.1 File Operations
- **New Document** (`Ctrl+N`): Creates a blank rich text document.
- **Open Document** (`Ctrl+O`): Opens files in supported formats:
  - Rich Text Format (`.rtf`)
  - OpenDocument Text (`.odt`)
  - Office Open XML (`.docx`)
  - Plain Text (`.txt`, `.text`, `.log`, etc.) with UTF-8, ANSI/System, UTF-16LE, UTF-16BE encodings
  - HTML Documents (`.html`, `.htm`)
- **Save** (`Ctrl+S`): Saves to the current file path in its format.
- **Save As**:
  - Rich Text Document (`*.rtf`)
  - Office Open XML Document (`*.docx`)
  - OpenDocument Text (`*.odt`)
  - Plain Text Document (`*.txt`)
  - Other Formats (`*.html`, `*.htm`)
- **Print & Print Preview**:
  - Print dialog (`Ctrl+P`): Full printer selection, range, copies, collate.
  - Quick Print: Instant print to default printer.
  - Print Preview: Interactive page preview with zoom and page navigation.
- **Page Setup**:
  - Paper Size (A4, Letter, Legal, A3, A5, B5, Executive, etc.)
  - Orientation: Portrait, Landscape
  - Margins: Left, Right, Top, Bottom (configurable in Inches, Centimeters, Points, Picas)
  - Print page numbers checkbox
- **Recent Documents**:
  - Persistent list of recently opened files with quick access.
- **Send in email**:
  - Launches the default system email client with the document attached or in body.
- **Exit** (`Alt+F4`): Prompts to save unsaved changes before exiting.

### 2.2 Editing & Formatting (Home Tab)
- **Clipboard Group**:
  - Paste (`Ctrl+V`), Cut (`Ctrl+X`), Copy (`Ctrl+C`), Paste Special.
- **Font Group**:
  - Font Family combobox with live font previews and system fonts.
  - Font Size combobox (8pt to 72pt + editable arbitrary size).
  - Grow Font (`Ctrl+Shift+>`) and Shrink Font (`Ctrl+Shift+<`).
  - Bold (`Ctrl+B`), Italic (`Ctrl+I`), Underline (`Ctrl+U`), Strikethrough.
  - Subscript (`Ctrl+=`), Superscript (`Ctrl+Shift++`).
  - Font Color palette dropdown with custom color dialog.
  - Text Highlight Color dropdown with color swatches.
- **Paragraph Group**:
  - Decrease Indent (`Ctrl+Shift+M` or `Shift+Tab`) and Increase Indent (`Ctrl+M` or `Tab`).
  - Lists: None, Bullet (Disc/Circle/Square), Numbered (1, 2, 3), Lowercase alphabetic (a, b, c), Uppercase alphabetic (A, B, C), Roman numerals (i, ii, iii / I, II, III).
  - Line Spacing: 1.0, 1.15, 1.5, 2.0, with "Add 10pt space after paragraph" toggle.
  - Text Alignment: Align Left (`Ctrl+L`), Align Center (`Ctrl+E`), Align Right (`Ctrl+R`), Justify (`Ctrl+J`).
  - Paragraph Properties Dialog: Exact Left Indent, Right Indent, First Line Indent, Line Spacing, After Spacing, Alignment.
- **Insert Group**:
  - Insert Picture: Insert from file (`.png`, `.jpg`, `.jpeg`, `.bmp`, `.svg`, `.webp`, `.gif`).
  - Change Picture & Resize Picture (Width/Height % and pixels, Aspect ratio lock).
  - Paint Drawing: Integrated drawing canvas editor allowing freehand drawing, lines, shapes, and colors, embedding directly into the document.
  - Date and Time (`F5` or Ribbon): Dialog with 10+ standard date and time formats.
  - Insert Object: Dialog to insert vector drawings, shapes, or attachments.
- **Editing Group**:
  - Find (`Ctrl+F`): Find next, match case, match whole word, search backwards/forwards.
  - Replace (`Ctrl+H`): Replace, Replace All, match case, whole word.
  - Select All (`Ctrl+A`).

### 2.3 View Options (View Tab)
- **Zoom Group**:
  - Zoom In (`Ctrl++` / `Ctrl+WheelUp`), Zoom Out (`Ctrl+-` / `Ctrl+WheelDown`), 100% Zoom.
- **Show or Hide Group**:
  - Ruler toggle checkbox.
  - Status Bar toggle checkbox.
- **Settings Group**:
  - Word Wrap: No Wrap, Wrap to Window, Wrap to Ruler.
  - Measurement Units: Inches (in), Centimeters (cm), Points (pt), Picas (pi).

### 2.4 Interactive Ruler
- Horizontal measurement ruler in the selected unit.
- Visual display of left margin, right margin, printable page area.
- Draggable First-Line Indent marker (top triangle).
- Draggable Left Indent / Hanging Indent marker (bottom triangle & rectangle).
- Draggable Right Indent marker (bottom right triangle).
- Interactive Tab Stops: Click ruler to insert tabs.

### 2.5 Status Bar & Zoom Slider
- Document status message / ready state.
- Cursor Line and Column position, selection character count.
- Zoom Slider: Range 10% to 500%, with `-` and `+` step buttons and clickable percentage indicator.

---

## 3. Non-Functional Requirements
- **Performance**: Instant startup (<150ms), smooth 60fps scrolling and typing on huge documents (>100 pages).
- **Fidelity**: Accurate reproduction of Windows WordPad look and feel with clean Qt6 modern styling.
- **Portability**: Native compilation on Linux (X11 & Wayland), Windows, and macOS with zero external non-standard dependencies beyond Qt6.
- **Memory Footprint**: Lightweight (<40MB resident memory for typical documents).
- **Stability**: Robust handling of corrupt or malformed RTF/DOCX/ODT files without crashing.
