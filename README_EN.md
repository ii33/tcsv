# TFD to CSV Converter

[中文文档](README.md) | **English**

## Project Overview

**TFD to CSV Converter** is a C++ utility that parses TFCalc `.tfd` (Thin Film Design) optical coating design files and exports layer information to structured CSV files. This tool is designed for researchers and engineers working with thin film optical coatings.

## Features

- **Automatic TFD File Detection**: Recursively scans the current directory for all `.tfd` files
- **Complete Data Extraction**: Extracts layer count, materials, optical thickness, physical thickness, and optical properties
- **Smart Encoding**: Converts materials to symbolic representations (H, L, M, etc.) based on refractive index
- **Comprehensive Summary**: Generates detailed metadata including substrate material, reference wavelength, and material totals
- **Smart Caching**: Compares file contents before writing to avoid unnecessary updates
- **Chinese Filename Support**: Full Unicode support for Chinese directory and file names

## Output Format

The converter generates CSV files with the following structure:

| Column 1 | Column 2 | Column 3 | Column 4 | Column 5 | Column 6 |
|----------|----------|----------|----------|----------|----------|
| Layer No. | Material | Optical Thickness | Physical Thickness (nm) | Info Key | Info Value |

**Example Output:**
```csv
1,Y2O3,0.182331,32.5,,
2,ZNS,1.89019,275.8,,
3,YBF3,0.233049,52.4,,
...
,,,,Total Layers,56
,,,,Total Physical Thickness,9245.3
,,,,Y2O3 Total,32.5
,,,,YBF3 Total,5821.6
,,,,ZNS Total,3391.2
,,,,Stack Formula,0.182331M 1.890187H 0.233049L ...
,,,,Substrate Material,ZNS
,,,,H Material,ZNS
,,,,L Material,YBF3
,,,,M Material,Y2O3
,,,,Reference Wavelength,1064
```

## Building and Compilation

**Requirements:**
- C++17 compatible compiler (g++, clang, MSVC)
- Standard C++ library with filesystem support

**Compilation:**
```bash
g++ tcsv.cpp -o tcsv.exe -std=c++17
```

## Usage

### Graphical Interface (Recommended)

1. Place your `.tfd` files in the program directory
2. Double-click `tcsv.exe` to run
3. The program automatically scans all `.tfd` files and creates a `tcsv` directory for output

### Command Line (For Output Feedback or Automation)

1. Place your `.tfd` files in the working directory
2. Run the executable:
   ```bash
   ./tcsv.exe
   ```
3. The program prints processing feedback and can be integrated into automation workflows

**Example:**
```
Input:  膜系.tfd, test.tfd
Output: tcsv/膜系.csv, tcsv/test.csv
```

## Output Details

**Data Rows:** Each layer is represented with:
- Layer number (sequential index)
- Material name
- Optical thickness (floating-point value)
- Physical thickness in nanometers
- Empty columns 5-6 (reserved for metadata)

**Summary Section:** Following all data rows:
- Total number of layers
- Total physical thickness sum
- Per-material physical thickness totals
- Stack formula with material symbols and coefficients
- Substrate material identification
- Material-to-symbol mapping
- Reference wavelength (design wavelength)

## File Structure

```
tcsv/
├── tcsv.cpp          # Main source code
└── README.md         # This file
```

---

## Version

- **Current Version**: 0.1
- **Release Date**: 2026-04-10
- **C++ Standard**: C++17
- **Supported Platforms**: Windows, Linux, macOS

## License

This project is provided as-is for educational and research purposes.
