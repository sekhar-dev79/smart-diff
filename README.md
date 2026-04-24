<div align="center">

<img src="screenshots/Intro_light.png" alt="SmartDiff Banner" width="700"/>

# ⚡ SmartDiff

**A high-performance, professional-grade file comparison and merge tool built with C++17 and Qt 6.7.3.**

[![C++17](https://img.shields.io/badge/C++-17-00599C.svg?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-6.7.3-41CD52.svg?style=for-the-badge&logo=qt)](https://www.qt.io/)
[![License](https://img.shields.io/badge/License-MIT-F5A623.svg?style=for-the-badge)](LICENSE.md)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey.svg?style=for-the-badge)]()
[![Build](https://img.shields.io/badge/Build-qmake-blue.svg?style=for-the-badge)]()

*Precise. Fast. Minimal. The diff tool built for developers who care about correctness.*

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Key Features](#-key-features)
- [Screenshots](#-screenshots)
- [Technical Deep Dive](#-technical-deep-dive)
- [Architecture](#-architecture)
- [Build & Deployment](#-build--deployment)
- [License](#-license)

---

## 🧭 Overview

**SmartDiff** is a native desktop application engineered for precise file comparison and merging, targeting developer and code-review workflows where accuracy and speed are non-negotiable.

At its core, SmartDiff implements the **Bounded Myers Shortest Edit Script** algorithm with **Prefix/Suffix optimization** — the same family of algorithms used by Git — delivering fast, correct diffs even on large, highly divergent files. Its ultra-minimalistic, compact UI is deliberately designed to keep the focus on the content, not the chrome, featuring a **28px compact header architecture** and professional Light and Dark themes with zero visual noise.

---

## 🚀 Key Features

### ⚙️ Advanced Diff Engine
Implements the **Bounded Myers Shortest Edit Script (SES)** algorithm with **Prefix/Suffix trimming** for optimal performance on real-world files. A configurable edit-distance cap prevents Out-of-Memory errors on massive, highly dissimilar inputs.

### 🔬 Word-Level Precision
Goes beyond line-level diffing to highlight specific intra-line word additions and removals, giving you the granular insight needed for meticulous code review without switching to a separate tool.

### 🔗 Synchronized Navigation
Side-by-side panels feature perfectly linked scrollbars with a **re-entrancy guard**, preventing feedback loops and ensuring both panels always stay in lock-step as you navigate through changes.

### 🛡️ Safe Merge Strategy
Uses a **Document Reconstruction** approach — computing the complete final document state in memory before performing a single atomic replacement — guaranteeing data integrity and a clean, linear undo stack.

### 🎨 Professional Design System
Dual-theme support with a clinical **Light** mode and a neutral high-contrast **Dark** mode featuring neon cyan accents. Both themes are built on a compact 28px header architecture optimized for focused, distraction-free workflows.

### 📐 DPI-Aware Assets
SVG-based `IconManager` ensures all UI icons render crisply at any display scale or resolution, from 1080p workstations to 4K monitors.

---

## 📸 Screenshots

### ☀️ Light Theme — Comparison Workflows

SmartDiff provides distinct visual cues for both mismatched and identical file states.

| Mismatched Logic | Identical State |
|:---:|:---:|
| ![Mismatches](screenshots/Unidentical_files_light.png) | ![Identical](screenshots/Identical_files_light.png) |

### 🌙 Dark Theme — Merge & Save Workflows

The Dark theme uses deep charcoal surfaces and high-contrast neon cyan accents to minimize eye strain during extended review sessions.

| Merging Hunks | Saving Results |
|:---:|:---:|
| ![Merge](screenshots/Merge_files_dark.png) | ![Save](screenshots/Saving_mergefile_dark.png) |

---

## 🛠️ Technical Deep Dive

### The Diff Algorithm

The core comparison logic in `DiffEngine` runs a two-stage pipeline designed to minimize unnecessary computation:

**Stage 1 — Prefix/Suffix Trimming**
Before the main algorithm runs, identical lines at the start and end of both files are detected and skipped entirely. For files that share large common headers or footers (e.g., configuration files, boilerplate-heavy source files), this single optimization can eliminate the majority of the comparison work upfront.

**Stage 2 — Bounded Myers SES**
The trimmed region is then processed by a Myers Shortest Edit Script algorithm, which computes the minimum set of insertions and deletions needed to transform one file into the other. A configurable maximum edit distance (D) cap is enforced throughout, ensuring the engine never exhausts memory on pathological inputs such as two completely unrelated binary-heavy files.

### The Merge Engine

SmartDiff deliberately avoids cursor-based surgical insertion, which is notorious for off-by-one errors and phantom whitespace when handling Qt's internal paragraph model. Instead, it uses a **Document Reconstruction** strategy:

1. The full final document state is computed as a `QStringList` entirely in memory.
2. The target editor is cleared.
3. The reconstructed content is written back in a **single atomic operation**, giving the undo system a clean, single-step boundary.

A notable edge case handled explicitly: `removeSelectedText()` is called before each insertion to prevent Qt from silently preserving hidden paragraph separators that would otherwise cause progressive line-count drift across multiple merge operations — a subtle bug known as the **Ghost-Line Problem**.

### Word-Level Diffing

After line-level hunks are computed, each changed line pair is passed through a secondary tokenizer that splits content into word tokens. A second Myers pass over the token stream produces word-granularity edit scripts, which are then mapped back onto the rendered line to drive the intra-line highlight delegates.

---

## 📂 Architecture

SmartDiff follows a strict modular separation between logic, presentation, and infrastructure:

```
SmartDiff/
├── core/           # DiffEngine and MergeEngine — pure logic, no Qt UI dependencies
├── ui/             # Main window, file panels, custom line-number gutter widgets
├── theme/          # ThemeManager — QSS loading and runtime Light/Dark palette switching
├── models/         # Data structures for hunks, diff lines, and word tokens
├── icons/          # SVG-based IconManager for DPI-aware asset rendering
├── utils/          # FileLoader — safe, encoding-aware file I/O utilities
└── screenshots/    # UI preview images
```

| Module | Responsibility |
|--------|----------------|
| `core/` | Myers SES diff algorithm, prefix/suffix optimization, merge reconstruction |
| `ui/` | All QWidget subclasses, synchronized scroll logic, re-entrancy guard |
| `theme/` | QSS stylesheet management, hot-swap Light/Dark theming |
| `models/` | `Hunk`, `DiffLine`, `WordToken` — immutable data structures |
| `icons/` | SVG rendering pipeline, DPI scale resolution |
| `utils/` | File open/save with encoding detection, error propagation |

---

## 🏗️ Build & Deployment

### Prerequisites

| Requirement | Version |
|-------------|---------|
| Qt (Widgets module) | 6.7.3 or higher |
| C++ Compiler | C++17 compatible (`llvm-mingw`, `MSVC 2019+`, `GCC 9+`, `Clang 10+`) |
| Build System | `qmake` (ships with Qt) |

### Compilation

```bash
# 1. Clone the repository
git clone https://github.com/your-username/smartdiff.git
cd smartdiff

# 2. Generate the Makefile
qmake SmartDiff.pro

# 3. Build
make              # Linux / macOS
mingw32-make      # Windows (MinGW)
```

### Release Build

```bash
qmake SmartDiff.pro CONFIG+=release
make
```

### Deployment

**Windows — Portable Package**

Use `windeployqt` to bundle the necessary Qt runtime, then trim unused modules to keep the package lean:

```bash
windeployqt --release SmartDiff.exe

# Remove unused modules to reduce package size (~24 MB target)
del opengl32sw.dll
del Qt6Pdf.dll
del Qt6Network.dll
```

**macOS**

```bash
macdeployqt SmartDiff.app -dmg
```

**Linux**

Ensure Qt 6.7.3 Widgets libraries are available on the target system, or use `linuxdeployqt` to produce a self-contained AppImage.

---

## 📜 License

This project is licensed under the **MIT License** — see the [LICENSE.md](LICENSE.md) file for full details.

---

<div align="center">

Built with ❤️ using **C++17** and **Qt 6.7.3**

<br/>

⭐ If SmartDiff saves you time, consider giving it a star!

</div>
