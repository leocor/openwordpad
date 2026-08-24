# OpenWordPad - Development & Progress Log

**Author**: Leonardo Corato  
**License**: GNU General Public License v3 (GPLv3)  
**Project**: OpenWordPad (Cross-platform WordPad clone in C++20 / Qt6)  

---

## Milestone Log

### [2026-08-24] - Initial Architecture & Setup
- [x] Initialized project repository and workspace structure.
- [x] Drafted GPLv3 License attributing author Leonardo Corato.
- [x] Created specification documents `requirements.md` and `requirements.ms`.
- [x] Created development tracking log `log.md`.
- [x] Designed vector icons for WordPad application branding and ribbon controls.
- [x] Created CMake build configuration with Qt6 Widgets, Core, Gui, PrintSupport, and Svg.

### [2026-08-24] - Core Engine & Format Handlers
- [x] Implemented RTF (Rich Text Format) Parser and Exporter.
- [x] Implemented Plain Text handler with UTF-8, ANSI, and UTF-16 support.
- [x] Integrated OpenDocument Text (.odt) and Office Open XML (.docx) / HTML loaders.
- [x] Implemented Document Manager with dirty tracking, file associations, and MRU list.

### [2026-08-24] - Ribbon UI & Windows Fidelity
- [x] Implemented WordPad Ribbon bar (Tabs: Home, View, File Menu, Quick Access Toolbar).
- [x] Implemented Home tab groups: Clipboard, Font, Paragraph, Insert, Editing.
- [x] Implemented View tab groups: Zoom, Show/Hide, Settings (Wrap modes & Units).
- [x] Implemented Interactive Horizontal Ruler with margin and indent drags.
- [x] Implemented Status Bar with interactive Zoom slider (- [==] +) and line/column tracker.

### [2026-08-24] - Dialogs & Rich Features
- [x] Implemented Page Setup dialog with margins and paper size.
- [x] Implemented Paragraph properties dialog.
- [x] Implemented Date and Time insertion dialog.
- [x] Implemented Modeless Find & Replace dialog.
- [x] Implemented Image resize & properties dialog.
- [x] Implemented Embedded Paint Drawing dialog.
- [x] Implemented About WordPad dialog with author and license details.

### [2026-08-24] - UI Refinement & Authentic Pixel Fidelity
- [x] Implemented dynamic horizontal centering for both the document paper sheet and the ruler coordinates, matching Microsoft WordPad's centered page layout.
- [x] Fixed button widths in the Inserisci group (68px) to eliminate text truncation (`Immagine`).
- [x] Converted all icons into scalable crisp vector SVGs matching original Microsoft WordPad graphics.
- [x] Perfectly aligned page sheet rendering with the ruler margin coordinates.
- [x] Rebuilt application and verified all tests pass with 100% success rate.

### [2026-08-24] - Version 1.1.9 & Immagine Split Button Layout Tuning
- [x] **Eliminated Immagine Split Button Divider Overlap**:
  - Expanded `Immagine` split button width from 70px to 82px, ensuring ample 8px horizontal padding between the word "Immagine" and the vertical dropdown separator.
  - Expanded `Incolla` to 68px, `Disegno di Paint` to 76px, and `Inserisci oggetto` to 78px for generous spacing across all DPI scalings.
- [x] **100% CTest Suite Validation**:
  - All 4 test suites passed with zero errors.
