/**
 * @file t2m.cpp
 * @brief TFD to CSV Converter - Converts TFCalc optical design files to structured CSV format
 * @version 0.1
 * @date 2026-04-10
 * 
 * This program parses TFCalc .tfd (Thin Film Design) optical coating files and exports
 * layer information to CSV format. It extracts material properties, optical thickness,
 * physical thickness, and generates stack formulas using material symbols.
 * 
 * Features:
 * - Parse layer structure from TFD files
 * - Extract material information and thickness values
 * - Generate symbolic layer stack formula (H, L, M, etc.)
 * - Export comprehensive CSV with summary statistics
 * - Support for Unicode filenames (Chinese characters)
 * - Smart content checking to avoid redundant writes
 */

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <locale>
#include <codecvt>
#include <map>
#include <set>

namespace fs = std::filesystem;

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * @brief Split string by delimiter
 * @param s Input string
 * @param delimiter Character to split by
 * @return Vector of split tokens
 */
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// ============================================================================
// Data Structures
// ============================================================================

/**
 * @struct Layer
 * @brief Represents a single optical layer in the coating design
 */
struct Layer {
    int number;                 ///< Layer index (1-based)
    std::string material;       ///< Material name (e.g., ZNS, YBF3, Y2O3)
    double optical_thickness;   ///< Optical thickness (relative to reference wavelength)
    double physical_thickness;  ///< Physical thickness in nanometers
};

// ============================================================================
// Core Parsing Functions
// ============================================================================

/**
 * @brief Parse TFD file and extract layer information
 * 
 * Reads a TFCalc .tfd file and extracts:
 * - Layer structure (count, material, thickness)
 * - ENVIRON information (substrate material)
 * - ENVIRON2 data (reference wavelength)
 * - SYMBOL mappings (material to symbolic representations)
 * 
 * @param[in]  filepath               Path to the TFD file
 * @param[out] layer_count            Total number of layers
 * @param[out] layers                 Vector of Layer structures
 * @param[out] total_physical         Sum of all physical thicknesses
 * @param[out] material_totals        Physical thickness per material
 * @param[out] stack_formula          Symbol-based formula string
 * @param[out] h_material             Material assigned to 'H' symbol
 * @param[out] l_material             Material assigned to 'L' symbol
 * @param[out] substrate_material     Substrate material name
 * @param[out] reference_wavelength   Design reference wavelength
 * @param[out] symbol_to_material     Map from symbol to material name
 * 
 * @return true if parsing succeeded, false on file error
 */
bool parse_tfd(const fs::path& filepath, int& layer_count, std::vector<Layer>& layers, 
               double& total_physical, std::map<std::string, double>& material_totals, 
               std::string& stack_formula, std::string& h_material, std::string& l_material, 
               std::string& substrate_material, double& reference_wavelength, 
               std::map<char, std::string>& symbol_to_material) {
    std::wifstream file(filepath);
    if (!file.is_open()) {
        std::wcerr << L"Cannot open file: " << filepath.wstring() << std::endl;
        return false;
    }

    std::locale utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
    file.imbue(utf8_locale);

    std::map<std::string, char> material_to_symbol;
    std::set<std::string> all_materials;
    std::wstring wline;
    
    // Parse file line by line
    while (std::getline(file, wline)) {
        std::string line(wline.begin(), wline.end());
        
        // Extract substrate material from ENVIRON section
        if (line.find("ENVIRON*") == 0) {
            auto parts = split(line, '*');
            if (parts.size() >= 8) {
                substrate_material = parts[7];
            }
        }
        
        // Extract reference wavelength from ENVIRON2 section
        if (line.find("ENVIRON2*") == 0) {
            auto parts = split(line, '*');
            if (parts.size() >= 5) {
                reference_wavelength = std::stod(parts[4]);
            }
        }
        
        // Parse material symbols (H, L, M, etc.)
        if (line.find("SYMBOL*") == 0) {
            auto parts = split(line, '*');
            if (parts.size() >= 4) {
                material_to_symbol[parts[3]] = parts[2][0];
                symbol_to_material[parts[2][0]] = parts[3];
                if (parts[2] == "H") h_material = parts[3];
                else if (parts[2] == "L") l_material = parts[3];
            }
        }
        
        // Parse layer count
        if (line.find("LAYERS*") == 0) {
            auto parts = split(line, '*');
            if (parts.size() >= 2) {
                layer_count = std::stoi(parts[1]);
            }
        }
        
        // Parse individual layer data
        if (line.find("LAYER*") == 0) {
            auto parts = split(line, '*');
            if (parts.size() >= 5) {
                Layer layer;
                layer.number = std::stoi(parts[1]);
                layer.material = parts[2];
                layer.optical_thickness = std::stod(parts[3]);
                layer.physical_thickness = std::stod(parts[4]);
                layers.push_back(layer);
                total_physical += layer.physical_thickness;
                material_totals[layer.material] += layer.physical_thickness;
                all_materials.insert(layer.material);
            }
        }
        
        // End of layers section
        if (line.find("LAYERS2*") == 0) {
            break;
        }
    }
    file.close();

    // Auto-assign symbols to materials not in SYMBOL section
    char next_symbol = 'M';
    for (const auto& mat : all_materials) {
        if (!material_to_symbol.count(mat)) {
            material_to_symbol[mat] = next_symbol;
            symbol_to_material[next_symbol] = mat;
            next_symbol++;
        }
    }

    // Generate stack formula: optical_thickness + symbol for each layer
    for (const auto& layer : layers) {
        char symbol = material_to_symbol[layer.material];
        stack_formula += std::to_string(layer.optical_thickness) + symbol + " ";
    }

    return true;
}

// ============================================================================
// Output Functions
// ============================================================================

/**
 * @brief Generate and write CSV output file
 * 
 * Creates a CSV file with layer data and comprehensive summary information.
 * Includes smart content comparison to skip unchanged files.
 * 
 * CSV Format:
 * - Rows 1-N: Layer data (number, material, optical thickness, physical thickness)
 * - Following rows: Summary information (totals, formulas, metadata)
 * 
 * @param[in] csv_path                Output CSV file path
 * @param[in] layer_count             Total number of layers
 * @param[in] layers                  Vector of layer structures
 * @param[in] total_physical          Sum of physical thicknesses
 * @param[in] material_totals         Per-material thickness sums
 * @param[in] stack_formula           Symbolic layer formula
 * @param[in] h_material              Material name for H symbol
 * @param[in] l_material              Material name for L symbol
 * @param[in] substrate_material      Substrate material
 * @param[in] reference_wavelength    Design reference wavelength
 * @param[in] symbol_to_material      Symbol-to-material mapping
 */
void output_to_csv(const fs::path& csv_path, int layer_count, const std::vector<Layer>& layers, 
                    double total_physical, const std::map<std::string, double>& material_totals, 
                    const std::string& stack_formula, const std::string& h_material, 
                    const std::string& l_material, const std::string& substrate_material, 
                    double reference_wavelength, const std::map<char, std::string>& symbol_to_material) {
    
    // Ensure parent directory exists
    fs::path parent = csv_path.parent_path();
    if (!parent.empty()) {
        fs::create_directories(parent);
    }

    // Generate CSV content
    std::wstringstream wss;
    std::locale utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
    wss.imbue(utf8_locale);

    // Write layer data rows
    for (const auto& layer : layers) {
        std::wstring wmaterial(layer.material.begin(), layer.material.end());
        wss << layer.number << L"," << wmaterial << L"," 
            << layer.optical_thickness << L"," << layer.physical_thickness << L",,\n";
    }

    // Write summary section
    wss << L",,,," << L"Total Layers" << L"," << layer_count << L"\n";
    wss << L",,,," << L"Total Physical Thickness" << L"," << total_physical << L"\n";
    for (const auto& p : material_totals) {
        std::wstring wmat(p.first.begin(), p.first.end());
        wss << L",,,," << wmat << L" Total" << L"," << p.second << L"\n";
    }
    std::wstring wstack(stack_formula.begin(), stack_formula.end());
    wss << L",,,," << L"Stack Formula" << L"," << wstack << L"\n";
    std::wstring wsub(substrate_material.begin(), substrate_material.end());
    wss << L",,,," << L"Substrate Material" << L"," << wsub << L"\n";
    
    // Write material symbol mappings
    for (const auto& p : symbol_to_material) {
        std::string symbol_str(1, p.first);
        std::wstring wsym(symbol_str.begin(), symbol_str.end());
        std::wstring wmat(p.second.begin(), p.second.end());
        wss << L",,,," << wsym << L" Material" << L"," << wmat << L"\n";
    }
    wss << L",,,," << L"Reference Wavelength" << L"," << reference_wavelength << L"\n";

    std::wstring content = wss.str();

    // Check if file content has changed (avoid unnecessary rewrites)
    if (fs::exists(csv_path)) {
        std::wifstream existing(csv_path);
        existing.imbue(utf8_locale);
        std::wstring existing_content((std::istreambuf_iterator<wchar_t>(existing)), 
                                     std::istreambuf_iterator<wchar_t>());
        if (existing_content == content) {
            std::cout << "File unchanged, skipped: " << csv_path << std::endl;
            return;
        }
    }

    // Write CSV file
    std::wofstream csv_file(csv_path);
    if (!csv_file.is_open()) {
        std::wcerr << L"Cannot create CSV file: " << csv_path.wstring() << std::endl;
        return;
    }
    csv_file.imbue(utf8_locale);
    csv_file << content;
    csv_file.close();
    std::cout << "Output to: " << csv_path << std::endl;
}

// ============================================================================
// Main Program
// ============================================================================

/**
 * @brief Main entry point
 * 
 * Program flow:
 * 1. Initialize UTF-8 locale for proper Unicode handling
 * 2. Create 'tcsv' output directory
 * 3. Scan current directory for .tfd files
 * 4. For each .tfd file:
 *    - Parse layer and material information
 *    - Generate symbolic stack formula
 *    - Output to CSV in tcsv directory
 */
int main() {
    // Set UTF-8 locale for proper Unicode/Chinese character handling
    try {
        std::locale::global(std::locale("en_US.UTF-8"));
    } catch (const std::runtime_error&) {
        // Locale not available on this system, continue with default
    }

    // Create output directory
    fs::create_directory("tcsv");

    fs::path current_dir = fs::current_path();
    std::cout << "Scanning directory: " << current_dir << std::endl;

    // Process all TFD files in current directory
    for (const auto& entry : fs::directory_iterator(current_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".tfd") {
            fs::path tfd_path = entry.path();
            std::wcout << L"Processing file: " << tfd_path.wstring() << std::endl;

            // Initialize variables for parsing
            int layer_count = 0;
            std::vector<Layer> layers;
            double total_physical = 0;
            std::map<std::string, double> material_totals;
            std::string stack_formula;
            std::string h_material;
            std::string l_material;
            std::string substrate_material;
            double reference_wavelength = 0.0;
            std::map<char, std::string> symbol_to_material;
            
            // Parse TFD file
            if (parse_tfd(tfd_path, layer_count, layers, total_physical, material_totals, 
                         stack_formula, h_material, l_material, substrate_material, 
                         reference_wavelength, symbol_to_material)) {
                // Generate output CSV path
                fs::path csv_path = fs::path("tcsv") / tfd_path.filename();
                csv_path.replace_extension(".csv");
                
                // Write CSV output
                output_to_csv(csv_path, layer_count, layers, total_physical, material_totals, 
                             stack_formula, h_material, l_material, substrate_material, 
                             reference_wavelength, symbol_to_material);
            }
        }
    }

    std::cout << "Processing complete." << std::endl;
    return 0;
}

