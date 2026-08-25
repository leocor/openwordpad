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

### [2026-08-25] - Version 1.3.3 - Windows MSI Installer Package & WiX Integration
- [x] **Windows MSI Installer (`.msi`) Support**:
  - Integrated WiX Toolset pipeline in GitHub Actions release workflow.
  - Generates `OpenWordPad-Windows-x64-Setup.msi` installing to `C:\Program Files\OpenWordPad\`.
  - Automatically creates Start Menu and Desktop shortcuts with custom multi-resolution `.ico` icon.
  - Registers publisher ("Leonardo Corato"), version, and clean uninstallation in Windows Control Panel / Settings.
- [x] **100% CTest Suite Validation**:
  - All 5 test suites passed 100%.

### [2026-08-24] - Version 1.3.2 - Translation Context Resolution for Namespaced Qt Classes
- [x] **Namespaced MOC Translation Context Fix (`OpenWordPad::RibbonBar`)**:
  - Resolved MOC context name mismatch where `RibbonBar` and child widgets generated `OpenWordPad::RibbonBar` context.
  - Recompiled all 21 `.qm` dictionaries with comprehensive namespaced and global class contexts.
  - Validated with unit tests confirming Italian, Spanish, and all other languages translate all ribbon tabs, buttons, and status messages automatically on both Linux and Windows.
- [x] **100% CTest Suite Validation**:
  - All 5 test suites passed 100%.

### [2026-08-24] - Version 1.3.1 - Windows User Session & Multi-layer Locale Detection Fix
- [x] **Windows Server / Multi-User Session Display Language Resolution**:
  - Integrated native Windows Win32 API `GetUserDefaultUILanguage()` and `QLocale::system().uiLanguages()` checks.
  - Automatically respects user session language packs on Windows Server / Windows 10/11 even when the underlying server install image defaults to English.
- [x] **100% CTest Suite Validation**:
  - All 5 test suites passed 100%.

### [2026-08-24] - Version 1.3.0 - Full Multilingual Internationalization (22 Languages)
- [x] **Native 22-Language Internationalization (i18n)**:
  - Integrated `TranslationManager` with automatic system locale detection (`QLocale::system()`) and persistent user preference in `QSettings`.
  - Added full built-in dictionaries for 22 languages: 🇮🇹 Italiano, 🇬🇧 English, 🇪🇸 Español, 🇫🇷 Français, 🇩🇪 Deutsch, 🇵🇹 Português, 🇷🇺 Русский, 🇨🇳 简体中文, 🇹🇼 繁體中文, 🇯🇵 日本語, 🇰🇷 한국어, 🇸🇦 العربية, 🇮🇳 हिन्दी, 🇳🇱 Nederlands, 🇵🇱 Polski, 🇹🇷 Türkçe, 🇸🇪 Svenska, 🇬🇷 Ελληνικά, 🇨🇿 Čeština, 🇭🇺 Magyar, 🇺🇦 Українська, 🇮🇩 Bahasa Indonesia.
  - Added dynamic Language selector in the View tab under Settings with instant live retranslation (`retranslateUi()`).
- [x] **100% CTest Suite Validation**:
  - Added `TranslationTests` verifying 22 languages load cleanly without error. All 5 test suites pass 100%.

### [2026-08-24] - Version 1.2.3 - View Tab 100% Zoom Button Padding Polish
- [x] **Eliminated 100% Zoom Button Label Border Clipping**:
  - Expanded `100%`, `Ingrandisci`, and `Riduci` buttons in the View tab to fixed size `68x66px`.
  - Expanded `viewLayout` margins to 6px and spacing to 6px, completely preventing the "100%" text from clipping against the Zoom group border separator.
- [x] **100% CTest Suite Validation**:
  - All 4 test suites passed with zero errors.

### [2026-08-24] - Version 1.2.2 - Standalone Universal Linux AppImage Pipeline
- [x] **Full Linux Standalone AppImage (`.AppImage`) Deployment**:
  - Integrated AppDir packaging with `appimagetool` in GitHub Actions.
  - Bundled all Qt6 shared libraries, plugins (`platforms/libqxcb.so`, `imageformats/`, `iconengines/`), translations, and runtimes into a single portable `OpenWordPad-Linux-x86_64.AppImage` (~30-50 MB).
  - Works on any Linux distribution without installing Qt6 or any external dependencies.
- [x] **100% CTest Suite Validation**:
  - All 4 test suites passed with zero errors.

### [2026-08-24] - Version 1.2.0 - Centralized Dynamic Versioning & Full License/Doc Sync
- [x] **Dynamic Single-Source-of-Truth Versioning**:
  - Configured `CMakeLists.txt` with `project(OpenWordPad VERSION 1.2.0)` and passed `OPENWORDPAD_VERSION` compile definition.
  - Linked `AboutDialog` and `QApplication::applicationVersion()` to dynamically read the application version at runtime, completely eliminating version desynchronization.
- [x] **Complete Documentation & License Synchronization**:
  - Synchronized `docs/MANUAL.md`, `README.md`, `requirements.md`, and `AboutDialog` to **Version 1.2.0** and **GNU GPLv2**.
- [x] **100% CTest Suite Validation**:
  - All 4 test suites passed with zero errors.
