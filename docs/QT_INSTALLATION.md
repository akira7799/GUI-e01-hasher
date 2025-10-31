# Qt Installation Guide for E01 Hash Verification Tool

## Download Qt

1. **Go to Qt Downloads:**
   https://www.qt.io/download-qt-installer-oss

2. **Download:** Qt Online Installer for Windows

3. **Run the installer** (Qt Online Installer.exe)

## Installation Steps

1. **Create/Login** to Qt Account (free for open source)

2. **Select Components** - Choose:
   - Qt 6.8.3 (or latest 6.x version)
   - Under Qt 6.8.3, check:
     - ☑ MinGW 13.4.0 64-bit
     - ☑ Qt Charts (optional)
     - ☑ Sources (optional, for debugging)

   - Under Developer and Designer Tools:
     - ☑ MinGW 13.4.0 64-bit (same version as above)
     - ☑ Qt Creator (optional, useful IDE)

3. **Installation Path:**
   - Default: `C:\Qt`
   - Or choose your preferred location

4. **Wait for installation** (may take 10-30 minutes)

## After Installation

You should have:
- **Qt installation at:** `C:\Qt\6.8.3\mingw_64\`
- **MinGW at:** `C:\Qt\Tools\mingw1340_64\`

## Important Paths

Note these paths for the build configuration:

```
QT_DIR = C:\Qt\6.8.3\mingw_64
MINGW_DIR = C:\Qt\Tools\mingw1340_64
```

(Adjust version numbers if you installed a different version)

## Next Steps

After installation is complete, we'll:
1. Update the build scripts to use your Qt installation
2. Rebuild the E01 Hash Verification Tool
3. The executable will include Qt DLLs (10-15MB total)
