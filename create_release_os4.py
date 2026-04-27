#!/usr/bin/env python3
import os
import shutil
import re

def get_version():
    version = "unknown"
    commit = "unknown"
    try:
        with open("version.h", "r") as f:
            content = f.read()
            v_match = re.search(r'#define\s+AMIFIG_VERSION\s+"([^"]+)"', content)
            c_match = re.search(r'#define\s+AMIFIG_COMMIT\s+"([^"]+)"', content)
            if v_match: version = v_match.group(1)
            if c_match: commit = c_match.group(1)
    except Exception as e:
        print(f"Error reading version.h: {e}")
    return version, commit

def create_release():
    version, commit = get_version()
    release_name = f"Amifig-{version}-{commit}"
    release_dir = os.path.join("Release", release_name)
    
    if os.path.exists(release_dir):
        print(f"Directory {release_dir} already exists. Overwriting...")
        shutil.rmtree(release_dir)
    
    os.makedirs(release_dir)
    print(f"Creating release: {release_name}")

    # Root files
    shutil.copy2("AmiFIG_App/AmiFIG", release_dir)
    if os.path.exists("AmiFIG_App/AmiFIG.info"):
        shutil.copy2("AmiFIG_App/AmiFIG.info", release_dir)
    shutil.copy2("AmiFIG_App/FIG2DEV.py", release_dir)
    if os.path.exists("AmiFIG_App/FIG2DEV.py.info"):
        shutil.copy2("AmiFIG_App/FIG2DEV.py.info", release_dir)
    shutil.copy2("AmiFIG_App/RAS2VEC.py", release_dir)
    if os.path.exists("AmiFIG_App/RAS2VEC.py.info"):
        shutil.copy2("AmiFIG_App/RAS2VEC.py.info", release_dir)
    shutil.copy2("amifig-readme.md", release_dir)
    shutil.copy2("amifig-readme.txt", release_dir)
    if os.path.exists("AmiFIG_App/AmiFIG.cfg"):
        shutil.copy2("AmiFIG_App/AmiFIG.cfg", release_dir)
    if os.path.exists("AmiFIG_App/logo.png"):
        shutil.copy2("AmiFIG_App/logo.png", release_dir)

    # Directories
    dirs_to_copy = {
        "AmiFIG_App/Arexx": "Arexx",
        "AmiFIG_App/fig2dev": "fig2dev",
        "AmiFIG_App/potrace": "potrace",
        "AmiFIG_App/svg2fig": "svg2fig",
        "AmiFIG_App/autotrace": "autotrace",
        "AmiFIG_App/Catalogs": "Catalogs",
        "AmiFIG_App/Skins": "Skins"
    }

    for src, dst in dirs_to_copy.items():
        if os.path.exists(src):
            shutil.copytree(src, os.path.join(release_dir, dst))
            print(f"Copied directory: {src} -> {dst}")

    # Docs directory
    docs_dir = os.path.join(release_dir, "Docs")
    os.makedirs(docs_dir)
    shutil.copy2("ARexx_Guide.md", docs_dir)
    shutil.copy2("AREXX_SUPPORT.md", docs_dir)
    shutil.copy2("External_Tools_Guide.md", docs_dir)
    print(f"Copied documentation to Docs/")

    print(f"\nRelease created successfully in: {release_dir}")

if __name__ == "__main__":
    create_release()
