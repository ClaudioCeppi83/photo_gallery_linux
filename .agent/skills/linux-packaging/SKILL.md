---
name: linux-packaging
description: Expert guide for creating high-quality Linux packages, focusing on Flatpak and FreeDesktop.org standards. Use this skill when the user needs to distribute their application, create manifests, or fix integration issues.
license: MIT
---

# Linux Packaging Expert

This skill guides you through packaging Linux applications, with a strong focus on Flatpak as the primary distribution format.

## Flatpak: The Gold Standard

### Manifest Structure (YAML)
Flatpak manifests define how to build your application and its dependencies in a sandboxed environment.

```yaml
app-id: org.example.MyApp
runtime: org.gnome.Platform
runtime-version: '45'
sdk: org.gnome.Sdk
command: my-app
finish-args:
  - --share=ipc
  - --socket=fallback-x11
  - --socket=wayland
  - --device=dri
modules:
  - name: my-app
    buildsystem: meson
    sources:
      - type: dir
        path: .
```

### Key Considerations
1.  **Sandbox Permissions:** Start strict. Only add permissions (`finish-args`) that are absolutely necessary.
2.  **Filesystem Access:** Prefer portals (via `GtkFileChooserNative`) over direct filesystem access (`--filesystem=host`).
3.  **Network Access:** Only enable network if needed (`--share=network`).

## FreeDesktop.org Integration

Your application must integrate seamlessly with the desktop environment.

### 1. Desktop Entry (`.desktop`)
This file tells the desktop environment how to launch your app.
**Location:** `/usr/share/applications/org.example.MyApp.desktop` (inside the sandbox: `/app/share/applications/...`)

```ini
[Desktop Entry]
Name=My App
Exec=my-app
Icon=org.example.MyApp
Type=Application
Categories=Utility;GTK;
```

### 2. AppStream Metadata (`.metainfo.xml`)
This file provides descriptions and screenshots for software centers (GNOME Software, Discover).
**Location:** `/usr/share/metainfo/org.example.MyApp.metainfo.xml`

- **ID:** Must match the App ID and Desktop Entry file name.
- **Screenshots:** Include high-quality screenshots with captions.
- **Releases:** Keep a history of version releases with changelogs.

### 3. Icons
Provide scalable SVG icons.
**Location:** `/usr/share/icons/hicolor/scalable/apps/org.example.MyApp.svg`
