#pragma once // Prevents this file from being loaded twice
#include <vector>
#include <string>
#include <filesystem>
#include <set>

// This is our "Backend" tool kit
namespace SuttaLibrary {
    // 1. Tell the library where the data folder is
    void init(const std::filesystem::path& base_path);

    // 2. The tools the UI will use to get text
    std::vector<std::string> get_nikayas();
    std::vector<std::string> get_suttas(int nikaya_index);
    std::vector<std::string> get_sutta_text(int sutta_index);

    bool is_read(const std::string& id);
}
