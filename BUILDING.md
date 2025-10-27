# Building E01 Hash Verification Tool

## Build Environment

This project is built using cross-compilation from Linux (WSL2) to Windows using MinGW-w64.

### Prerequisites

- WSL2 or Linux system
- MinGW-w64 cross-compiler (x86_64-w64-mingw32)
- CMake 3.20+
- Git

### Dependency Locations

All build dependencies are installed in `/tmp/` for this development session:

| Component | Location | Version | Notes |
|-----------|----------|---------|-------|
| Qt Static | `/tmp/qt5-static-install/` | 5.15.17 | MSYS2 pre-built static libraries |
| libewf | `/tmp/libewf-install/` | latest | Built from source (GitHub: libyal/libewf) |
| Windows CryptoAPI | Built into Windows | N/A | Link with `-ladvapi32`, provides MD5/SHA1/SHA256 |

### Build Environment Setup

Source the environment file before building:

```bash
source /tmp/e01-hasher-build-env.sh
```

This sets the following environment variables:

- `QT_STATIC_DIR`: Qt 5.15.17 static installation path
- `LIBEWF_DIR`: libewf installation path
- `MINGW_PREFIX`: MinGW-w64 toolchain prefix (x86_64-w64-mingw32)
- `CC`, `CXX`, `AR`, `RANLIB`: Cross-compiler tools

### Building Qt 5.15 Static (Already Done)

Qt 5.15.17 static libraries were obtained from MSYS2:

```bash
wget https://mirror.msys2.org/mingw/mingw64/mingw-w64-x86_64-qt5-static-5.15.17-1-any.pkg.tar.zst
mkdir -p qt5-static-msys2
cd qt5-static-msys2
tar --use-compress-program=unzstd -xf ../mingw-w64-x86_64-qt5-static-5.15.17-1-any.pkg.tar.zst
ln -sfn $(pwd)/mingw64/qt5-static /tmp/qt5-static-install
```

### Building libewf (Already Done)

libewf was built from source:

```bash
cd /tmp
git clone https://github.com/libyal/libewf.git
cd libewf
./synclibs.sh
./autogen.sh
./configure --host=x86_64-w64-mingw32 \\
            --prefix=/tmp/libewf-install \\
            --enable-static \\
            --disable-shared \\
            --with-zlib=no \\
            --with-bzip2=no \\
            --without-libuuid \\
            --without-openssl
make -j$(nproc)
make install
```

### Building the E01 Hash Verification Tool

(Instructions will be added in Task 2.0)

1. Navigate to project directory
2. Source the build environment
3. Run qmake to generate Makefile
4. Run make to build the application

```bash
cd /home/dave/e01-hasher
source /tmp/e01-hasher-build-env.sh
# (qmake and make commands to be added)
```

### Dependencies for Hash Calculation

The application uses **Windows CryptoAPI** for hash calculations:
- **MD5**: `CALG_MD5`
- **SHA1**: `CALG_SHA1`
- **SHA256**: `CALG_SHA_256`
- **Link library**: `-ladvapi32`

No OpenSSL required - CryptoAPI is built into Windows.

### Static Linking

All dependencies are statically linked to create a standalone `.exe` with no external DLL dependencies (except Windows system DLLs).

### Target Platform

- **OS**: Windows 10 (1809+) and Windows 11
- **Architecture**: x86_64 (64-bit)
- **Compiler**: MinGW-w64 GCC 13

### Verifying the Build

After building, verify the executable has no unexpected DLL dependencies:

```bash
x86_64-w64-mingw32-objdump -p e01hasher.exe | grep "DLL Name"
```

Expected DLLs (Windows system libraries only):
- KERNEL32.dll
- msvcrt.dll
- ADVAPI32.dll
- USER32.dll
- GDI32.dll
- etc.

No Qt or libewf DLLs should be listed (they're statically linked).

## Troubleshooting

### Missing qmake

If `qmake` is not found, use the full path:

```bash
/tmp/qt5-static-install/bin/qmake.exe
```

### Linker Errors

Ensure all environment variables are set correctly by re-sourcing the build environment file.

### Permission Errors on WSL

If you encounter permission issues accessing Windows paths, ensure WSL2 is properly configured to access Windows filesystems.

## Clean Build

To perform a clean build:

```bash
make clean
rm -rf build/
# Re-run qmake and make
```
