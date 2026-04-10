# TFD to CSV Converter | TFD 转 CSV 转换工具

## English Version

### Project Overview

**TFD to CSV Converter** is a C++ utility that parses TFCalc `.tfd` (Thin Film Design) optical coating design files and exports layer information to structured CSV files. This tool is designed for researchers and engineers working with thin film optical coatings.

### Features

- **Automatic TFD File Detection**: Recursively scans the current directory for all `.tfd` files
- **Complete Data Extraction**: Extracts layer count, materials, optical thickness, physical thickness, and optical properties
- **Smart Encoding**: Converts materials to symbolic representations (H, L, M, etc.) based on refractive index
- **Comprehensive Summary**: Generates detailed metadata including substrate material, reference wavelength, and material totals
- **Smart Caching**: Compares file contents before writing to avoid unnecessary updates
- **Chinese Filename Support**: Full Unicode support for Chinese directory and file names

### Output Format

The converter generates CSV files with the following structure:

| Column 1 | Column 2 | Column 3 | Column 4 | Column 5 | Column 6 |
|----------|----------|----------|----------|----------|----------|
| Layer No. | Material | Optical Thickness | Physical Thickness (nm) | Info Key | Info Value |

**Example Output:**
```csv
1,Y2O3,0.182331,25,,
2,ZNS,1.89019,219.2,,
3,YBF3,0.233049,41.7001,,
...
,,,,Total Layers,56
,,,,Total Physical Thickness,7918.5
,,,,Y2O3 Total,25
,,,,YBF3 Total,4874.4
,,,,ZNS Total,3019.1
,,,,Stack Formula,0.182331M 1.890187H 0.233049L ...
,,,,Substrate Material,ZNS
,,,,H Material,ZNS
,,,,L Material,YBF3
,,,,M Material,Y2O3
,,,,Reference Wavelength,1064
```

### Building and Compilation

**Requirements:**
- C++17 compatible compiler (g++, clang, MSVC)
- Standard C++ library with filesystem support

**Compilation:**
```bash
g++ t2m.cpp -o t2m.exe -std=c++17
```

### Usage

1. Place your `.tfd` files in the working directory
2. Run the executable:
   ```bash
   ./t2m.exe
   ```
3. The tool automatically creates a `tcsv` directory and outputs CSV files
4. Each `.tfd` file produces a corresponding `.csv` file

**Example:**
```
Input:  膜系.tfd, test.tfd
Output: tcsv/膜系.csv, tcsv/test.csv
```

### Output Details

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

### How Stack Formula Works

The stack formula represents the optical coating design using material symbols and optical thickness coefficients:
- **H, L, M, etc.**: Material symbols (H=High RI, L=Low RI, M=Medium RI)
- **Coefficient**: Represents the optical thickness (e.g., `0.182331M` = 0.182331 × quarter-wave at reference wavelength)

Example: `0.182331M 1.890187H 0.233049L` describes three layers with different materials and optical depths.

### File Structure

```
tfc2mac/
├── t2m.cpp           # Main source code
├── t2m.exe           # Compiled executable (Windows)
├── 膜系.tfd           # Input TFD sample file
├── test.tfd          # Input TFD test file
├── README.md         # This file
└── tcsv/             # Auto-generated output directory
    ├── 膜系.csv
    └── test.csv
```

### Technical Details

**File Parsing:** The tool parses TFD format sections:
- `ENVIRON*`: Extracts substrate material (last field)
- `ENVIRON2*`: Reads reference wavelength (4th field)
- `SYMBOL*`: Maps materials to symbolic representations
- `LAYERS*`: Retrieves layer count
- `LAYER*`: Extracts individual layer data

**Material Symbol Assignment:**
1. Materials with predefined symbols (SYMBOL field) retain their designation
2. Unassigned materials are auto-mapped to M, N, O, etc.
3. All materials are guaranteed unique symbols

---

## 中文版本

### 项目概述

**TFD 转 CSV 转换工具**是一个 C++ 实用程序，用于解析 TFCalc `.tfd`（薄膜设计）光学膜系设计文件，并将层信息导出为结构化的 CSV 文件。此工具专为从事薄膜光学涂层研究和工程的人员设计。

### 功能特性

- **自动检测TFD文件**：递归扫描当前目录下所有 `.tfd` 文件
- **完整数据提取**：提取层数、材料、光学厚度、物理厚度和光学特性
- **智能符号编码**：根据折射率将材料转换为符号表示（H、L、M 等）
- **综合汇总信息**：生成基片材料、参考波长、材料总厚度等详细元数据
- **智能缓存机制**：写入前比较文件内容，避免不必要的更新
- **中文文件名支持**：完全的 Unicode 支持，支持中文目录和文件名

### 输出格式

转换工具生成具有以下结构的 CSV 文件：

| 第1列 | 第2列 | 第3列 | 第4列 | 第5列 | 第6列 |
|-------|-------|-------|-------|-------|-------|
| 层号 | 材料 | 光学厚度 | 物理厚度(nm) | 信息键 | 信息值 |

**输出示例：**
```csv
1,Y2O3,0.182331,25,,
2,ZNS,1.89019,219.2,,
3,YBF3,0.233049,41.7001,,
...
,,,,总层数,56
,,,,总物理厚度,7918.5
,,,,Y2O3总厚度,25
,,,,YBF3总厚度,4874.4
,,,,ZNS总厚度,3019.1
,,,,膜堆公式,0.182331M 1.890187H 0.233049L ...
,,,,基片材料,ZNS
,,,,H材料,ZNS
,,,,L材料,YBF3
,,,,M材料,Y2O3
,,,,参考波长,1064
```

### 编译构建

**系统要求：**
- C++17 兼容编译器（g++、clang、MSVC）
- 支持 filesystem 的标准 C++ 库

**编译命令：**
```bash
g++ t2m.cpp -o t2m.exe -std=c++17
```

### 使用方法

1. 将 `.tfd` 文件放置在工作目录中
2. 运行可执行文件：
   ```bash
   ./t2m.exe
   ```
3. 工具自动创建 `tcsv` 目录并输出 CSV 文件
4. 每个 `.tfd` 文件生成对应的 `.csv` 文件

**使用示例：**
```
输入文件：膜系.tfd, test.tfd
输出文件：tcsv/膜系.csv, tcsv/test.csv
```

### 输出说明

**数据行**：每一层包含以下信息：
- 层序号（顺序索引）
- 材料名称
- 光学厚度（浮点值）
- 物理厚度（纳米）
- 第5-6列为空（预留给元数据）

**汇总部分**：位于所有数据行之后，包含：
- 总层数
- 总物理厚度
- 各材料的物理厚度总计
- 膜堆公式（含材料符号和系数）
- 基片材料
- 材料与符号的映射关系
- 参考波长（设计波长）

### 膜堆公式说明

膜堆公式使用材料符号和光学厚度系数表示光学膜系设计：
- **H、L、M 等**：材料符号（H=高折射率、L=低折射率、M=中等折射率）
- **系数值**：表示光学厚度（如 `0.182331M` = 0.182331 × 参考波长处的四分之一波长）

示例：`0.182331M 1.890187H 0.233049L` 描述三层膜，各层材料和光学深度不同。

### 文件结构

```
tfc2mac/
├── t2m.cpp           # 主源代码文件
├── t2m.exe           # 编译后的可执行文件（Windows）
├── 膜系.tfd           # 输入 TFD 示例文件
├── test.tfd          # 输入 TFD 测试文件
├── README.md         # 本文件
└── tcsv/             # 自动生成的输出目录
    ├── 膜系.csv
    └── test.csv
```

### 技术细节

**文件解析**：工具解析 TFD 格式的以下段落：
- `ENVIRON*`：提取基片材料（最后一项）
- `ENVIRON2*`：读取参考波长（第4项）
- `SYMBOL*`：映射材料到符号表示
- `LAYERS*`：获取层总数
- `LAYER*`：提取单层数据

**材料符号分配规则：**
1. 具有预定义符号（SYMBOL字段）的材料保留其符号
2. 未分配的材料自动映射到 M、N、O 等字母
3. 所有材料均保证唯一的符号标识

---

## Version / 版本

- **Current Version / 当前版本**: 0.1
- **Release Date / 发布日期**: 2026-04-10
- **C++ Standard / C++ 标准**: C++17
- **Supported Platforms / 支持平台**: Windows, Linux, macOS

## License / 许可证

This project is provided as-is for educational and research purposes.
本项目按原样提供，用于教育和研究目的。
