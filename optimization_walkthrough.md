# AmiFIG Optimization Walkthrough

This document outlines the recent optimizations and tools added to the AmiFIG environment on AmigaOS 4.

## 1. Batch Export System (`ExportAll.amigados`)

We've created a batch script to automate the export of a FIG file into multiple formats simultaneously.

**Usage:**
```amigados
Execute ExportAll.amigados
```

**Features:**
- Hardcoded paths to ensure reliability.
- Outputs 10 formats: **GIF, PNG, JPEG, PCX, TIFF, XBM, XPM, SVG, EPS, PDF**.
- Automatically saves results to `RAM:`.
- Manages the AmigaDOS `Path` internally so all Netpbm helper tools are found.

## 2. Expanded Netpbm Suite

The `fig2dev` directory now contains a full suite of natively compiled PowerPC binaries for AmigaOS 4, including:
- `fig2dev`: The core conversion engine.
- `pamtosvg`: **Vectorization engine** (derived from Autotrace).
- `ppmquant`: A custom-built wrapper for high-quality color quantization on AmigaOS.
- Support tools: `ppmtopcx`, `ppmtoxpm`, `pbmtoxbm`, `ppmtoacad`, etc.

## 3. Dedicated RAS2VEC (Python + ProAction)

The `RAS2VEC.lua` script has been replaced with a high-performance Python 2.5 + ProAction implementation.

**File:** `AmiFIG_App/RAS2VEC.py`

**Key Features:**
- **Natively Compiled Engine**: Uses the natively built `pamtosvg` tool, which contains the stable **Autotrace** core.
- **Universal Input**: Automatically handles conversion from JPG, PNG, GIF, and TIFF using the integrated Netpbm suite.
- **Advanced Control**: Provides full control over thresholds (Error, Corner, Line), despeckling (Filter Iterations), and geometry settings.
- **SVG Excellence**: Optimized for high-quality SVG output, perfect for importing into AmiFIG.
- **Atomic Execution**: Robust handling of Amiga shell commands and temporary storage in `T:`.
