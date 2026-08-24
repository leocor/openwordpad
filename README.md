# OpenWordPad

[![License: GPL v2](https://img.shields.io/badge/License-GPLv2-blue.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Qt6](https://img.shields.io/badge/Qt-6.5+-green.svg)](https://www.qt.io/)
[![Build Windows x64](https://github.com/leocor/openwordpad/actions/workflows/build-windows.yml/badge.svg)](https://github.com/leocor/openwordpad/actions/workflows/build-windows.yml)
[![GitHub Release](https://img.shields.io/github/v/release/leocor/openwordpad?color=success)](https://github.com/leocor/openwordpad/releases)

A faithful, full-featured, high-performance recreation of **Microsoft WordPad** built in modern C++ (C++20) and Qt6. OpenWordPad brings back the simplicity, speed, and clean interface of classic Windows document editing across Linux, Windows, and macOS.

**Author**: Leonardo Corato  
**License**: GNU General Public License v2.0 (GPLv2)  

---

## 📥 Download Pronti all'uso (Releases)

Puoi scaricare l'ultima versione precompilata direttamente dalla sezione **[Releases](https://github.com/leocor/openwordpad/releases)**:

* 🪟 **[Windows x64 (.zip)](https://github.com/leocor/openwordpad/releases/latest)**: Pacchetto portabile autonomo (non richiede installazione, include tutte le DLL di runtime).
* 🐧 **[Linux x86_64 (.tar.gz)](https://github.com/leocor/openwordpad/releases/latest)**: Eseguibile pronto per qualsiasi distribuzione Linux (CachyOS, Arch, Ubuntu, Debian, Fedora, Mint).

---

## ✨ Features

- 🎨 **Faithful Ribbon Interface**: Quick Access Toolbar, Blue Application Menu, Home tab, View tab, and interactive status bar.
- 📐 **Interactive Measurement Ruler**: Draggable Left/Right margins, First Line Indent, Hanging Indent, and dynamic unit switching (Centimeters, Inches, Points, Picas).
- 📝 **Rich Text Formatting**: Font families, font sizes, bold, italic, underline, strikethrough, subscript, superscript, color palettes, and highlight swatches.
- 📄 **Paragraph & Layout Tools**: Line spacing (1.0 to 2.0 + 10pt space after), multi-level bullet and numbered lists, alignments, and full Page Setup.
- 🖼️ **Insert Features**: Insert / Change / Resize pictures, embedded Paint sketch canvas, and Date & Time dialog.
- 🔍 **Full Editing & Search**: Modeless Find & Replace dialogs with whole word and case sensitivity matching.
- 🔍 **Smooth Zooming**: Real-time Zoom slider from 10% to 500% with percentage presets.
- 📂 **Comprehensive Format Support**:
  - Rich Text Format (`.rtf`)
  - Office Open XML (`.docx`)
  - OpenDocument Text (`.odt`)
  - Plain Text (`.txt`) with UTF-8 / ANSI encoding
  - HTML (`.html`, `.htm`)
  - PDF Export (`.pdf`)

---

## 🚀 Building & Running

### Prerequisites
- CMake 3.16+
- Qt 6 (Widgets, Core, Gui, PrintSupport, Svg)
- Modern C++20 Compiler (GCC, Clang, or MSVC)
- Ninja (recommended)

### Compile on Linux (CachyOS / Arch / Ubuntu / Fedora)
```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C build
./build/openwordpad
```

### Run Tests
```bash
./build/test_engine -platform offscreen
```

---

## 📜 Documentation & Requirements
- [User Manual](docs/MANUAL.md)
- [Requirements Specification (requirements.md / requirements.ms)](requirements.md)
- [Development Log (log.md)](log.md)

---

## ⚖️ License
OpenWordPad is free software licensed under the [GNU General Public License v3 (GPLv3)](LICENSE).  
Copyright (C) 2026 Leonardo Corato.
