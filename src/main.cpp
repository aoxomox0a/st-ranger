#include <algorithm>
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem; // FIX 1: Added missing semicolon

// Helper to get directory names (Nikāyas)
std::vector<std::string> get_directories(const fs::path &root) {
  std::vector<std::string> dirs;
  if (!fs::exists(root))
    return {"Data not found!"};

  for (const auto &entry : fs::directory_iterator(root)) {
    if (entry.is_directory()) {
      dirs.push_back(entry.path().filename().string());
    }
  }
  std::sort(dirs.begin(), dirs.end()); // Keep them in order
  return dirs;
}

// Helper to get sutta files in a specific Nikāya
std::vector<std::string> get_sutta_files(const fs::path &nikaya_path) {
  std::vector<std::string> files;
  if (!fs::exists(nikaya_path))
    return {"No suttas found"}; // Safety check

  for (const auto &entry : fs::directory_iterator(nikaya_path)) {
    if (entry.is_regular_file() && entry.path().extension() == ".json") {
      // Clean up name
      std::string name = entry.path().stem().string();
      size_t pos = name.find("_");
      if (pos != std::string::npos)
        name = name.substr(0, pos);
      files.push_back(name);
    }
  }
  std::sort(files.begin(), files.end());
  return files;
}

// FIX 2: Everything below moved INSIDE main()
int main() {
  using namespace ftxui;
  auto screen = ScreenInteractive::Fullscreen();

      // 1. Setup paths
      fs::path base_path = "data/bilara-data/translation/en/sujato/sutta";

  // 2. Initial State
  std::vector<std::string> nikayas = get_directories(base_path);
  int sel_nikaya = 0;

  // Safety check so the app doesn't crash if the folder isn't found
  std::vector<std::string> suttas = {"Select a Nikaya"};
  if (nikayas[0] != "Data not found!") {
    suttas = get_sutta_files(base_path / nikayas[sel_nikaya]);
  }
  int sel_sutta = 0;

  // 3. Create the UI Menus
  auto menu_nikaya = Menu(&nikayas, &sel_nikaya);
  auto menu_sutta = Menu(&suttas, &sel_sutta);

  // 4. The Logic Bridge (Ranger behavior!)
  menu_nikaya |= CatchEvent([&](Event event) {
    // When the user interacts with the left column, update the middle column
    if (sel_nikaya >= 0 && sel_nikaya < nikayas.size() &&
        nikayas[0] != "Data not found!") {
      suttas = get_sutta_files(base_path / nikayas[sel_nikaya]);
      sel_sutta = 0; // Reset the sutta cursor to the top of the new list
    }
    return false; // Let the event bubble up so the cursor still moves
  });

  // 5. The Layout (Miller Columns)
  auto container = Container::Horizontal({
      menu_nikaya,
      menu_sutta,
  });

  auto renderer = Renderer(container, [&] {
    return hbox({
        window(text(" Collections "), menu_nikaya->Render()) | flex,
        window(text(" Suttas "), menu_sutta->Render()) | flex,
        window(
            text(" Reading Pane "),
            vbox({text("Selected: " + (suttas.empty() ? std::string("None")
                                                      : suttas[sel_sutta])) |
                      bold,
                  separator(), paragraph("The JSON reader will go here...")})) |
            flex, // Makes the reading pane wider
    });
  });

  // FIX 2: Add a global event catcher to listen for the exit keys
  renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('q') || event == Event::Escape) {
      screen.Exit(); // Tells the FTXUI loop to stop
      return true;   // Tells FTXUI we successfully handled the event
    }
    return false; // If it wasn't 'q' or 'Esc', ignore it and let the menus
                  // handle it
  });

  // FIX 3: Start the actual UI loop!
  screen.Loop(renderer); // This is what keeps the terminal app running

  return 0;
}
