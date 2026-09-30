#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "SuttaLibrary.hpp"

#define SUTTA_PATH "data/translation/en/sujato/sutta"

using namespace std;
namespace fs = std::filesystem;
using json = nlohmann::json;

struct Sutta {
        std::string id;
        std::string title;
        std::string filepath;
}

// get json metadata first
// create file
// using json to get metadata
// save to file
// return filename to main
string parse_suttas() {
        string file = "sutta_lib.json";
        string raw_data_path = SUTTA_PATH;

        if (!fs::exists(raw_data_path)){
                cerr << "Error: Could not find raw data path.\n";
                // abort
                return None;
        }

        if (fs::exists(file)){
                cout << "json metdata file exists. Returning.\n";
        } else {
                std::ofstream pen(file);
                pen.close();
                cout << "file created, filling";
        }

        for (auto const& dir_entry : fs::recursive_directory_iterator(raw_data_path))
        {
                cout << dir_entry << '\n';
                if (!error_code && entry.is_regular_file() && entry.path().extension()== "json"){
                        fs::path p = entry.path()
                        string id = p.stem().string()
                }
        }
        // now fill file
        return file
}

int main() {
        json_file = parse_suttas()
        if (json_file == None){
                // abort
                return 1;
        }
        // vector to hold metadata
        Sutta my_sutta;
        // loop through json metadata file
        for (const auto &){
                my_sutta.id = 
                my_sutta.title = 
        }

    return 0;
}
