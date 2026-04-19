#include "SuttaLibrary.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <map>

namespace fs = std::filesystem;
using json = nlohmann::ordered_json;

namespace SuttaLibrary {
    // Hidden internal state
    fs::path library_base_path;
    std::vector<fs::path> current_sutta_paths; 
    std::vector<std::string> current_nikaya_folders; 

    std::map<std::string, std::string> nikaya_dictionary = {
        {"an", "Aṅguttara Nikāya"},
        {"dn", "Dīgha Nikāya"},
        {"kn", "Khuddaka Nikāya"},
        {"mn", "Majjhima Nikāya"},
        {"sn", "Saṃyutta Nikāya"}
    };

    void init(const fs::path& base_path) {
        library_base_path = base_path;
    }

    std::vector<std::string> get_nikayas() {
        std::vector<std::string> display_names;
        current_nikaya_folders.clear();

        if (!fs::exists(library_base_path)) return {"Data not found!"};

        for (const auto& entry : fs::directory_iterator(library_base_path)) {
            if (entry.is_directory()) {
                current_nikaya_folders.push_back(entry.path().filename().string());
            }
        }
        std::sort(current_nikaya_folders.begin(), current_nikaya_folders.end());

        for (const auto& folder : current_nikaya_folders) {
            if (nikaya_dictionary.count(folder)) display_names.push_back(nikaya_dictionary[folder]);
            else display_names.push_back(folder); 
        }
        return display_names;
    }

    std::string get_sutta_title(const fs::path& filepath) {
        std::ifstream file(filepath);
        std::string line;
        for (int i = 0; i < 10 && std::getline(file, line); ++i) {
            if (line.find(":0.2\"") != std::string::npos || line.find(":0.3\"") != std::string::npos) {
                size_t separator = line.find("\": \"");
                if (separator != std::string::npos) {
                    size_t end_quote = line.rfind("\"");
                    if (end_quote != std::string::npos && end_quote > separator + 3) {
                        return line.substr(separator + 4, end_quote - (separator + 4));
                    }
                }
            }
        }
        return "Unknown Title";
    }

    std::vector<std::string> get_suttas(int nikaya_index) {
        std::vector<std::string> display_names;
        current_sutta_paths.clear();

        if (nikaya_index < 0 || nikaya_index >= current_nikaya_folders.size()) return {"No suttas found"};
        fs::path target_path = library_base_path / current_nikaya_folders[nikaya_index];

        // FIX: Tell the iterator to silently skip folders it isn't allowed to read
        auto options = fs::directory_options::skip_permission_denied;
        std::error_code ec; // Holds errors silently

        for (const auto& entry : fs::recursive_directory_iterator(target_path, options, ec)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                current_sutta_paths.push_back(entry.path());
            }
        }
        
        std::sort(current_sutta_paths.begin(), current_sutta_paths.end());

        for (const auto& path : current_sutta_paths) {
            std::string raw_id = path.stem().string();
            size_t pos = raw_id.find("_");
            if (pos != std::string::npos) raw_id = raw_id.substr(0, pos);

            std::string pretty_id = raw_id;
            for (auto& c : pretty_id) c = std::toupper(c);

            std::string title = get_sutta_title(path);
            display_names.push_back(pretty_id + ": " + title);
        }
        
        if (display_names.empty()) display_names.push_back("Empty folder");
        return display_names;
    }

    std::vector<std::string> get_sutta_text(int index) {
        if (index < 0 || index >= current_sutta_paths.size()) return {"No sutta selected."};
        
        std::ifstream file(current_sutta_paths[index]);
        if (!file.is_open()) return {"Error: Could not open file."};

        json j;
        try { file >> j; } 
        catch (...) { return {"Error: Could not parse JSON."}; }

        std::vector<std::string> lines;
        for (auto& [key, value] : j.items()) {
            if (value.is_string()) {
                std::string segment = value.get<std::string>();
                if (!segment.empty()) lines.push_back(segment);
            }
        }
        return lines;
    }
}
