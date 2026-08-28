#include <iostream>
#include "font_loader.h"
#include <vector>
#include <fstream>

void load_font(std::string file_path, std::vector<std::string> *font){
    std::ifstream inputFile(file_path);
    
    // Check if the file was oppened succefully
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file at: \""<<file_path<<"\"!."<< std::endl;
    }

    std::string new_number = "";
    std::string line;

    // Add each line of the file into new number and if there is a empty line
    // it adds new_number into the font array
    while (std::getline(inputFile, line)) {
        if(line != ""){
            new_number += line + '\n';
        }
        else{
            font->push_back(new_number);
            new_number = "";
        }
    }
    font->push_back(new_number);
}