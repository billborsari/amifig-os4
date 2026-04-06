# AmiFIG (AmigaOS 4 Port)

Welcome to the **AmiFIG** repository, an AmigaOS 4 port of the classic vector graphics editor. **AmiFIG** was formerly known as **ZuneFIG** and has been completely renamed and optimized for AmigaOS 4, bringing the robust capabilities of the XFig ecosystem to modern Amiga environments.

This repository primarily hosts the OS 4 specific fixes, including hardware-specific UI layout adjustments, compiler updates, stability enhancements (such as patching NULL gadget pointer dereferences), and native `DebugPrintF` infrastructure for real hardware tracking.

## Why AmiFIG?
AmiFIG serves as a native vector drafting board. Whether drawing simple diagrams or complex layered figures, AmiFIG brings the precision of the legendary *XFig* software right to your AmigaOS 4 desktop, utilizing the modern Magic User Interface (MUI) class system (originally ported as ZuneFIG for AROS).

## 🏆 Acknowledgments and Original Authors
This software stands on the shoulders of giants. We would like to express our deepest gratitude to the original authors whose foundational work makes AmiFIG possible:

- **ZuneFIG Foundation:** AmiFIG is a direct rename and continuation of **ZuneFIG**.
- **Yannick Erb:** For his vast work maintaining and porting AmiFIG between 2010–2016.
- **Andreas Schmidt:** Original author of AmiFIG 3.1.
- **Supoj Sutanthavibul:** Original Copyright (c) 1985 for the foundational **XFig** vector drawing application.
- **Brian V. Smith & Paul King:** For their immense contributions to the XFig project in the 90s.
- **Darel Rex Finley:** Polygon gradient fill algorithm (Public Domain, 2007).
- **Alan Richardson:** For the text rotation algorithm (`w_rottext.c`).
- **Anthony Dekker:** For the Kohonen neural network color optimization code.
- **C. Blanc and C. Schlick:** For the X-Spline model implementation (Universite Bordeaux, France).

### Contributors
A huge thank you to the many contributors across the Amiga community who brought ZuneFIG alive on AROS, OS4, and MorphOS:
**Kas1e**, **Xenic**, **Polluks**, **Saimon69**, **Frank Ruthe**, **Mathias Rustler**, **Samir Hawamdeh**, **Manu**, and **Tomasz Paul**.

*Without your initial button images, catalogs, ports, and bug testing, this branch would not exist.*

## 📜 Licenses
AmiFIG remains fully open source and is released under the **AROS Public License**. 

Certain foundational algorithms and XFig engine components retain their original unrestricted, royalty-free copyrights extending back to the 1980s and 1990s. As per XFig's original license, you have the full, unrestricted right to deal in this software, including the rights to use, copy, modify, merge, publish, and/or distribute copies, provided that the original copyright notices properly remain intact in the source documentation. 

For the complete historical project files and historical context on AROS, you can visit the [historical sourceforge page](http://sourceforge.net/projects/amifig/).
