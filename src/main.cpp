#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include "SuttaLibrary.hpp"

int main() {
    using namespace ftxui;
    auto screen = ScreenInteractive::Fullscreen();

    SuttaLibrary::init("data/bilara-data/translation/en/sujato/sutta");

    int current_screen = 0; // 0 = Welcome, 1 = Ranger
    bool zen_mode = false;

    std::vector<std::string> baskets = {
        "Basket of Discourses (Sutta Piṭaka)",
        "Basket of Monastic Law (Vinaya Piṭaka) - Coming Soon",
        "Basket of Systematic Treatises (Abhidhamma Piṭaka) - Coming Soon"
    };
    int sel_basket = 0;

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

    MenuOption welcome_options;
    welcome_options.entries_option.transform = [](const EntryState& state) {
        if (state.focused) return text(">> " + state.label) | bold | color(Color::RGB(32, 27, 19));
        return text("  " + state.label) | color(Color::RGB(32, 27, 19));
    };

    MenuOption reader_options;
    reader_options.entries_option.transform = [](const EntryState& state) {
        if (state.focused) {
            return vbox({
                hbox({
                    text(">> ") | bold | color(Color::RGB(32, 27, 19)), 
                    paragraph(state.label) | bold | color(Color::RGB(32, 27, 19))
                }), 
                text("")
            }); 
        }
        return vbox({
            hbox({
                text("  "), 
                paragraph(state.label) | color(Color::RGB(32, 27, 19))
            }), 
            text("")
        });
    };

    // 4. Create the Components
    auto menu_welcome = Menu(&baskets, &sel_basket, welcome_options);
    auto menu_nikaya = Menu(&nikayas, &sel_nikaya);
    auto menu_sutta = Menu(&suttas, &sel_sutta);
    auto menu_reader = Menu(&cached_text, &sel_paragraph, reader_options);

    // 5. Logic Bridges
    menu_welcome |= CatchEvent([&](Event event) {
        if (event == Event::Return && sel_basket == 0) {
            current_screen = 1; // Swap to the Ranger view!
            return true; 
        }
        return false;
    });

    menu_nikaya |= CatchEvent([&](Event event) {
        if (sel_nikaya >= 0 && sel_nikaya < nikayas.size()) {
            suttas = SuttaLibrary::get_suttas(sel_nikaya);
            sel_sutta = 0;
            sel_paragraph = 0; 
        }
        return false; 
    });

    // 6. Group the Containers
    auto ranger_container = Container::Horizontal({ menu_nikaya, menu_sutta, menu_reader });
    
    // The main router that swaps between the Welcome Screen and Ranger Screen
    auto main_container = Container::Tab({ menu_welcome, ranger_container }, &current_screen);

    // 7. The Dynamic Renderer
    auto renderer = Renderer(main_container, [&] {
        if (current_screen == 0) {
            // --- DRAW WELCOME SCREEN ---
            return vbox({
                text(""), text(""), text(""), text(""), // Push down from the top
                text("Sutta Central TUI") | bold | center,
                text("Read the Buddha's words in your terminal") | dim | center,
                separatorEmpty(), separatorEmpty(),
                menu_welcome->Render() | center 
            }) | center | color(Color::RGB(32, 27, 19)) | bgcolor(Color::RGB(255, 248, 243));
        } 
        else {
            // --- DRAW RANGER SCREEN ---
            if (sel_sutta != last_read_sutta) {
                cached_text = SuttaLibrary::get_sutta_text(sel_sutta);
                sel_paragraph = 0; 
                last_read_sutta = sel_sutta;
            }

            auto col_1 = vbox({
                text(" Collections ") | bold | center,
                separatorLight(),
                menu_nikaya->Render()
            }) | borderRounded | size(WIDTH, EQUAL, 25); 

            auto col_2 = vbox({
                text(" Suttas ") | bold | center,
                separatorLight(),
                menu_sutta->Render() | vscroll_indicator | yframe | flex
            }) | borderRounded | size(WIDTH, EQUAL, 35); 

            std::string current_title = suttas.empty() ? "None" : suttas[sel_sutta];
            auto col_3 = vbox({
                text(" " + current_title + " ") | bold | center,
                text(" Translated by Bhikkhu Sujato ") | dim | center,
                separatorLight(),
                text(""), 

                hbox({
                    text("") | flex, 
                    menu_reader->Render() | vscroll_indicator | yframe | size(WIDTH, LESS_THAN, 80), 
                    text("") | flex  
                }) | flex

            }) | borderRounded | flex; 

            if (zen_mode) {
                // Renders ONLY the reading pane, perfectly centered
                return col_3 
                       | color(Color::RGB(32, 27, 19))
                       | bgcolor(Color::RGB(255, 248, 243));
            } else {
                // Renders the standard 3-column Ranger view
                return hbox({ col_1, col_2, col_3 }) 
                       | color(Color::RGB(32, 27, 19))
                       | bgcolor(Color::RGB(255, 248, 243));
            }
        }
    });

    // 8. Global Keybinds
    renderer |= CatchEvent([&](Event event) {
        
        // NEW: Toggle Zen Mode when 'z' is pressed in the Ranger view
        if (event == Event::Character('z') && current_screen == 1) {
            zen_mode = !zen_mode;
            return true;
        }

        // UPDATED: Backspace logic
        if (event == Event::Backspace && current_screen == 1) {
            if (zen_mode) {
                zen_mode = false; // If in Zen Mode, just exit Zen Mode
            } else {
                current_screen = 0; // Otherwise, go back to Welcome Screen
            }
            return true;
        }
        
        if (event == Event::Character('q') || event == Event::Escape) {
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(renderer);
    return 0;
}
