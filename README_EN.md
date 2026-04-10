# TFD to CSV Converter

[中文文档](README.md) | **English**

## Project Overview

**TFD to CSV Converter** is a C++ utility for parsing TFCalc `.tfd` (Thin Film Design) optical coating design files and exporting layer information to structured CSV files.

## Features

- **Automatic TFD File Detection**: Scans the current directory for all `.tfd` files
- **Data Extraction**: Extracts layer count, materials, optical thickness, physical thickness, and optical properties
- **Converts to Stack Formula**: Converts the coating system into stack formula representation
- **Comprehensive Summary**: Generates detailed metadata including substrate material, reference wavelength, and material totals
- **Smart Caching**: Compares file contents before writing to avoid unnecessary updates
- **Chinese Filename Support**: Full Unicode support for Chinese directory and file names

## Output Format

Generate CSV files with the following structure:

| Column 1 | Column 2 | Column 3 | Column 4 | Column 5 | Column 6 |
|----------|----------|----------|----------|----------|----------|
| Layer No. | Material | Optical Thickness | Physical Thickness (nm) | Info Key | Info Value |

**Example Output:**
```csv
1,Y2O3,0.1767531,32.5,,
2,ZNS,2.45019,275.8,,
3,YBF3,1.893049,52.4,,
...
,,,,Total Layers,62
,,,,Total Physical Thickness,9245.3
,,,,Y2O3 Total,32.5
,,,,YBF3 Total,6464.6
,,,,ZNS Total,4123.2
,,,,Stack Formula,0.1767531M 2.45019H 1.893049L ...
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

### Command Line

1. Place your `.tfd` files in the working directory
2. Run the executable:
   ```bash
   ./tcsv.exe
   ```

**Example:**
```
Input:  test.tfd
Output: tcsv/test.csv
```

## Output Details

**Data Rows:** Each layer contains the following information:
- Layer number
- Material name
- Optical thickness
- Physical thickness (nanometers)

**Summary Section:** Following all data rows, includes:
- Total number of layers
- Total physical thickness
- Per-material physical thickness totals
- Stack formula with material symbols and coefficients
- Substrate material
- Material-to-symbol mapping
- Reference wavelength (design wavelength)

## File Structure

```
tcsv/
├── tcsv.cpp          # Main source code
├── README.md         # Chinese documentation
└── README_EN.md      # English documentation
```

---

## Version

- **Current Version**: 0.1
- **Release Date**: 2026-04-10
- **C++ Standard**: C++17

## License

This project is provided as-is for educational and research purposes.
