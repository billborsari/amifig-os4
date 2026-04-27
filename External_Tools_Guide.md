# AmiFIG External Tools Guide

AmiFIG integrates several powerful external tools to handle complex file conversions and high-quality exports. These tools are accessible via the **File** menu.

---

## 🖼️ Fig2Dev (High-Quality Export)
Accessible via: `File -> Export (High Quality)...`

The **Fig2Dev** interface provides a bridge to the XFig ecosystem's standard export engine. It allows you to convert your `.fig` files into dozens of formats.

### Key Options
- **Format:** Choose from over 30 formats including PNG, SVG, EPS, PDF, and LaTeX.
- **Magnification:** Scale the output percentage (10% to 1000%).
- **Dimension Clipping:** Specify a maximum width/height to constrain the output.
- **Grid Options:** Add a superimposed grid to the export (Solid or Major/Minor).
- **Page Settings:** Set paper size (A4, Letter, etc.) and Orientation (Portrait/Landscape) for document-based formats like PostScript or PDF.

### Command Line Generation
The "Generate" button allows you to preview the exact shell command that will be executed. This is useful for advanced users who wish to run batch exports from the CLI.

---

## ⚡ RAS2VEC (Raster to Vector Tracing)
Accessible via: `File -> Import (Trace Raster)...`

This tool converts bitmap images (PNG, JPEG, GIF) into editable vector paths. It features two specialized engines:

### 1. PoTrace (Black & White)
Optimized for logos, icons, and line art.
- **Threshold:** Controls the cutoff between black and white pixels.
- **TurdSize:** Filters out small "noise" speckles (small areas of pixels).
- **Alphamax:** Controls the smoothness of curves.
- **Invert:** Flips the black and white areas of the source image.

### 2. PAMtoSVG (Color)
Optimized for photos and multi-colored artwork.
- **Centerline:** Attempts to find the center of strokes rather than outlining them.
- **Preserve Width:** Improves the accuracy of variable-width lines.
- **Error Threshold:** Higher values simplify the paths, lower values increase detail.
- **Omit Background:** Allows you to specify a color (e.g., White `FFFFFF`) to be treated as transparent.

---

## 📐 SVG Import
Accessible via: `File -> Import SVG...`

AmiFIG uses the `svg2fig` engine to translate W3C Standard SVG files into native Fig objects.

### Options
- **Scale:** Scales the incoming SVG to fit your document.
- **Interpolation Steps:** Controls how many line segments are used to approximate a Bezier curve. Higher values result in smoother curves but more complex documents.

---

## Technical Notes

### ProAction GUI Engine
All external tool interfaces (Fig2Dev and RAS2VEC) are powered by the **ProAction GUI Engine**. This allows for a modern, OS-compliant interface while maintaining extremely low memory overhead. 

**Requirements:**
- **ProAction Library:** Must be in your `LIBS:` or `APPDIR:`. 
- **Python 2.5+**: The scripts are optimized for the standard AmigaOS 4 Python distribution.

AmiFIG will attempt to start the ProAction server automatically if it is not running when an external tool is launched.

### Temporary Files
All tools use `T:` (Ram Disk) for intermediate conversion steps. Ensure you have sufficient space in your T: directory when processing extremely large high-resolution images.
