#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

// --- HELPER 1: Get Nikayas ---
std::vector<std::string> get_directories(const fs::path& root) {
    std::vector<std::string> dirs;
    if (!fs::exists(root)) return {"Data not found!"};

    for (const auto& entry : fs::directory_iterator(root)) {
        if (entry.is_directory()) dirs.push_back(entry.path().filename().string());
    }
    std::sort(dirs.begin(), dirs.end());
    return dirs;
}

// Global state for our actual file paths
std::vector<fs::path> current_sutta_paths; 

// --- HELPER 2: Get Suttas ---
std::vector<std::string> get_sutta_files(const fs::path& nikaya_path) {
    std::vector<std::string> display_names;
    current_sutta_paths.clear();

    if (!fs::exists(nikaya_path)) return {"No suttas found"};

    for (const auto& entry : fs::recursive_directory_iterator(nikaya_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            current_sutta_paths.push_back(entry.path());
        }
    }
    
    std::sort(current_sutta_paths.begin(), current_sutta_paths.end());

    for (const auto& path : current_sutta_paths) {
        std::string name = path.stem().string();
        size_t pos = name.find("_");
        if (pos != std::string::npos) name = name.substr(0, pos);
        display_names.push_back(name);
    }
    
    if (display_names.empty()) display_names.push_back("Empty folder");
    return display_names;
}

// --- HELPER 3: Dummy Text Loader (JSON Removed) ---
std::string get_sutta_text(int index) {
    if (index < 0 || index >= current_sutta_paths.size()) return "No sutta selected.";
    return "JSON parsing disabled for UI testing.\n\nFile: " + current_sutta_paths[index].filename().string();
}

// --- MAIN PROGRAM ---
int main() {
    using namespace ftxui;
    auto screen = ScreenInteractive::Fullscreen();

    fs::path base_path = "data/bilara-data/translation/en/sujato/sutta";

    std::vector<std::string> nikayas = get_directories(base_path);
    int sel_nikaya = 0;

    std::vector<std::string> suttas = {"Loading..."};
    if (nikayas[0] != "Data not found!") {
        suttas = get_sutta_files(base_path / nikayas[sel_nikaya]);
    }
    int sel_sutta = 0;

    std::string cached_text = get_sutta_text(sel_sutta);
    int last_read_sutta = sel_sutta;

    auto menu_nikaya = Menu(&nikayas, &sel_nikaya);
    auto menu_sutta = Menu(&suttas, &sel_sutta);

    menu_nikaya |= CatchEvent([&](Event event) {
        if (sel_nikaya >= 0 && sel_nikaya < nikayas.size()) {
            suttas = get_sutta_files(base_path / nikayas[sel_nikaya]);
            sel_sutta = 0;
        }
        return false; 
    });

    auto container = Container::Horizontal({
        menu_nikaya,
        menu_sutta,
    });

    auto renderer = Renderer(container, [&] {
        if (sel_sutta != last_read_sutta) {
            cached_text = get_sutta_text(sel_sutta);
            last_read_sutta = sel_sutta;
        }

        auto col_1 = vbox({
            text(" Collections ") | bold | center | color(Color::Orange1),
            separatorLight() | color(Color::Orange1),
            menu_nikaya->Render()
        }) | borderRounded | size(WIDTH, EQUAL, 25); 

        auto col_2 = vbox({
            text(" Suttas ") | bold | center | color(Color::Orange1),
            separatorLight() | color(Color::Orange1),
            menu_sutta->Render()
        }) | borderRounded | size(WIDTH, EQUAL, 30); 

        std::string current_title = suttas.empty() ? "None" : suttas[sel_sutta];
        auto col_3 = vbox({
            text(" " + current_title + " ") | bold | center | color(Color::YellowLight),
            text(" Translated by Bhikkhu Sujato ") | dim | center,
            separatorLight() | color(Color::Orange1),
            text(""), 
            paragraph(cached_text) | color(Color::Grey93)
        }) | borderRounded | flex; 

        return hbox({ col_1, col_2, col_3 });
    });

    renderer |= CatchEvent([&](Event event) {
        if (event == Event::Character('q') || event == Event::Escape) {
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(renderer);
    return 0;
}
