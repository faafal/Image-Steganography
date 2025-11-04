# Image-Steganography

C++ (CMake) implementation of basic image steganography techniques to **embed** and **extract** hidden messages in images (e.g., LSB on RGB channels).  
Includes a Windows executable and a portable CLI/CMake build system for cross-platform use.

## About
Originally developed as an **academic project**, later refined and corrected.  
It **earned the maximum grade** during project evaluation.

## Features
- Hide text payloads inside lossless images (PPM/BMP).
- Extract and verify hidden payloads.
- CLI tool; Windows binary provided via Releases.

## How to Execute (no build required)

**Windows:** grab the prebuilt executable from the Releases page:

- Latest: [ImageSteganography.exe (latest)](https://github.com/faafal/Image-Steganography/releases/latest/download/ImageSteganography.exe)
- All versions: [Releases](https://github.com/faafal/Image-Steganography/releases)

Run from a terminal:
```bash
ImageSteganography.exe --help
```


## Prerequisites (If you want to build it yourself)

You need a C++17 compiler, [CMake](https://cmake.org/download/), and [Git](https://git-scm.com/downloads).

Follow these official guides to install them for your system:

| Platform | Setup Guide |
|-----------|-------------|
| **Windows** | [Install MinGW-w64](https://www.mingw-w64.org/downloads/) **or** [Install Visual Studio Community 2022](https://visualstudio.microsoft.com/downloads/) |
| **Linux (Debian/Ubuntu)** | [Install CMake & GCC on Ubuntu](https://cmake.org/install/) |
| **macOS** | [Install Xcode Command Line Tools](https://developer.apple.com/xcode/resources/) and [Homebrew](https://brew.sh/) |

Once tools are installed and available in your PATH, run:

```bash
git clone https://github.com/faafal/Image-Steganography.git
cd Image-Steganography
cmake -S ImageSteganography -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```
**Resulting binary:**

  build/ImageSteganography.exe   # Windows
  
  build/ImageSteganography       # Linux/macOS

## Quick examples (Windows exe or self-built binary)
### Show image info
```bash 
ImageSteganography.exe --info Resources\testimg.bmp
```

### Check if a message fits
```bash 
ImageSteganography.exe --check Resources\testimg.bmp "Hello, world!"
```

### Encrypt (in-place writes into the image file)
```bash 
ImageSteganography.exe --encrypt Resources\testimg.bmp "Top secret"
```

### Decrypt / read hidden message
```bash 
ImageSteganography.exe --decrypt Resources\testimg.bmp
```


## Notes
- BMP must be **24-bit** and uncompressed
- PPM supports **P3** (ASCII) and **P6** (binary)
- `--encrypt` modifies the image **in place** so keep a backup copy if needed

