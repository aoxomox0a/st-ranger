#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include "SuttaLibrary.hpp" // <-- Bringing in our backend tool kit!

int main() {
    using namespace ftxui;
    auto screen = ScreenInteractive::Fullscreen();

    // 1. Initialize our backend
    SuttaLibrary::init("data/bilara-data/translation/en/sujato/sutta");

    // 2. Setup the state
    std::vector<std::string> nikayas = SuttaLibrary::get_nikayas();
    int sel_nikaya = 0;

    std::vector<std::string> suttas = {"Loading..."};
    if (nikayas[0] != "Data not found!") {
        suttas = SuttaLibrary::get_suttas(sel_nikaya);
    }
    int sel_sutta = 0;

    std::vector<std::string> cached_text = SuttaLibrary::get_sutta_text(sel_sutta);
    int last_read_sutta = sel_sutta;
    int sel_paragraph = 0; 

    // 3. Create the UI Menus
    auto menu_nikaya = Menu(&nikayas, &sel_nikaya);
    auto menu_sutta = Menu(&suttas, &sel_sutta);

    MenuOption reader_options;
    reader_options.entries_option.transform = [](const EntryState& state) {
        auto p = paragraph(state.label);
        if (state.focused) return vbox({p | color(Color::White) | bold, text("")});
        return vbox({p | color(Color::Grey93), text("")});
    };
    auto menu_reader = Menu(&cached_text, &sel_paragraph, reader_options);

    // 4. The Logic Bridge
    menu_nikaya |= CatchEvent([&](Event event) {
        if (sel_nikaya >= 0 && sel_nikaya < nikayas.size()) {
            suttas = SuttaLibrary::get_suttas(sel_nikaya);
            sel_sutta = 0;
            sel_paragraph = 0; 
        }
        return false; 
    });

    auto container = Container::Horizontal({ menu_nikaya, menu_sutta, menu_reader });

    // 5. The Renderer
    auto renderer = Renderer(container, [&] {
        if (sel_sutta != last_read_sutta) {
            cached_text = SuttaLibrary::get_sutta_text(sel_sutta);
            sel_paragraph = 0; 
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
        }) | borderRounded | size(WIDTH, EQUAL, 35); 

        std::string current_title = suttas.empty() ? "None" : suttas[sel_sutta];
        auto col_3 = vbox({
            text(" " + current_title + " ") | bold | center | color(Color::YellowLight),
            text(" Translated by Bhikkhu Sujato ") | dim | center,
            separatorLight() | color(Color::Orange1),
            text(""), 

            // THE UPDATE: Sandwich the reader between two flexible empty spaces
            hbox({
                text("") | flex, // Invisible left cushion

                // The text column, locked to a max width of 80 characters
                menu_reader->Render() | vscroll_indicator | frame | size(WIDTH, LESS_THAN, 80), 

                text("") | flex  // Invisible right cushion
            }) | flex

        }) | borderRounded | flex; 

        return hbox({ col_1, col_2, col_3 }) | bgcolor(Color::RGB(26, 27, 38));
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
