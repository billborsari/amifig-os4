# AmiFIG (ZuneFig AmigaOS 4 Port) - Version 3.5

Welcome to **AmiFIG**, the native AmigaOS 4 port of the classic vector graphics editor. This software is a direct evolution of **ZuneFIG**, now optimized and rebranded to better serve the Amiga community.

## 🚀 AmigaOS 4 Enhancements (v3.5)
This version introduces several critical updates to ensure AmiFIG runs smoothly on modern AmigaOS 4 systems:

- **Rebranding:** Completed the transition from ZuneFIG to **AmiFIG**, including updated window titles, documentation, and metadata.
- **Custom UI Rendering:** Implemented the `ColorSwatch` custom MUI class. This performs direct truecolor rendering, bypassing MUI's standard pen allocation system which frequently ran out of colors on high-density gadget windows.
- **Improved Stability:** Fixed multiple NULL pointer dereferences during window initialization and gadget attribute updates.
- **Palette Standardization:** Updated the default user color palette to a standard neutral grey (RGB 170, 170, 170) for a more professional initial workspace.
- **Modern ARexx Support:** Rewrote the ARexx port initialization to use modern OS4 `AllocSysObjectTags` APIs, resolving compatibility issues with recent OS updates.
- **Tool Modernization:** Converted the original Lua-based tools with MUI interfaces to Python powered by the **ProAction GUI Engine**. Included an updated **NetPBM** tool package to support high-quality image conversions and tracing.
- **Build System:** Updated Makefiles for compatibility with the latest AmigaOS 4 SDK (v54.16).

## 🛠️ Installation
AmiFIG 3.5.0 is designed as an optimized "overlay" for the standard ZuneFIG distribution. To install:

1.  **Download the base ZuneFIG package:** If you don't have it already, download the latest version from the [ZuneFIG SourceForge page](https://sourceforge.net/projects/zunefig/).
2.  **Extract ZuneFIG:** Extract the archive to your preferred location (e.g., `Work:Graphics/AmiFIG`).
3.  **Apply the AmiFIG 3.5.0 Upgrade:** Copy the contents of this release folder into your extracted ZuneFIG directory, overwriting all existing files and directories when prompted.
4.  **Requirements:** Ensure you have **Python 2.5** and the **ProAction** library installed on your system.

Once copied, launch the application using the **AmiFIG** icon.

## 🏆 Acknowledgments and Original Authors
AmiFIG stands on the shoulders of decades of development in the XFig and Amiga communities. We would like to express our deepest gratitude to the foundational authors:

- **Yannick Erb:** For his monumental effort maintaining and porting AmiFIG between 2010–2016.
- **Andreas Schmidt:** Original author of the AmiFIG 3.1 project.
- **AROS Foundation:** This branch is a direct continuation of the ZuneFIG project originally developed for AROS.
- **Supoj Sutanthavibul:** Original creator (1985) of the **XFig** vector engine.
- **Brian V. Smith & Paul King:** Principal maintainers of the XFig project throughout its most influential years.

### ZuneFIG and Previous AmigaOS 4 Porting Work
We would like to explicitly acknowledge the foundational work and porting efforts that made this version possible:
- **Original AmiFIG:** Andreas Schmidt
- **ZuneFIG:** Yannick Erb
- **AmigaOS 4 Porting:** Kas1e and Xenic

Their years of dedication provided the foundation upon which this modern AmigaOS 4 version is built.

### 💖 Support the Authors
If you appreciate this software, please note that any donations for this work should be routed to the original authors.

## 📜 Licenses
AmiFIG is released under the **AROS Public License**. 

Certain foundational algorithms and XFig engine components retain their original unrestricted, royalty-free copyrights extending back to the 1980s. As per XFig's original license, you have the full, unrestricted right to deal in this software, including the rights to use, copy, modify, merge, publish, and/or distribute copies, provided that the original copyright notices remain intact. 

For the complete historical project files and context, visit the [AmiFIG SourceForge page](http://sourceforge.net/projects/amifig/) and the [ZuneFIG project page](http://sourceforge.net/projects/zunefig/).

## 🌐 Development and Source Code
The official development repository for AmiFIG can be found on GitHub:
[https://github.com/billborsari/amifig-os4](https://github.com/billborsari/amifig-os4)

Feel free to report issues or contribute to the project through the GitHub interface.
