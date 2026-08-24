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

### [2026-08-24] - Version 1.1.7 & Windows UI Visual Polishing
- [x] **Resolved Split Button Black Rectangle on Windows**:
  - Explicitly styled `QToolButton::menu-button` and `QToolButton::menu-arrow` with clean transparent borders, hover backgrounds, and pure CSS vector arrows.
  - Eliminated the solid black block next to **Incolla** and **Immagine**.
- [x] **Resolved QComboBox Popup Item Visibility**:
  - Bound explicit `QListView` views to `cbFontFamily`, `cbFontSize`, `cbWordWrap`, and `cbUnits`.
  - Added dedicated styling for `QComboBox::drop-down` and `QComboBox::down-arrow`.
  - Fully resolved white-on-white text rendering in font sizes and family dropdowns on Windows.
- [x] **Eliminated Vertical Overlap in Ribbon Groups**:
  - Adjusted RibbonBar height to 154px and unified button heights to 66px.
  - Centered group labels with dedicated spacing, completely preventing overlap with button text.
- [x] **100% CTest Suite Validation**:
  - All 4 test suites passed with zero errors.
