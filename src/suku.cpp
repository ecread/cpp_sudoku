#include "suku.h"

//#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <vector>

bool Suku::readMatrix(const std::string filename, int* flatArray) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Failed to open file" << std::endl;
        return false;
    }
    for (int i = 0; i < 81; ++i) {
        if (!(file >> flatArray[i])) {
            return false;
        }
    }
    return true;
}

void Suku::writeMatrix(int* flatArray) {
    for (int i = 0; i < 81; ++i) {
        int row = i / 9;
        int col = i % 9;

        // Print the number with a consistent width
        std::cout << std::setw(2) << flatArray[i] << " ";

        // extra gap after 3 numbers
        if ((col + 1) % 3 == 0 && (col + 1) % 9 != 0) {
            std::cout << "  "; 
        }

        // row spacing
        if ((col + 1) % 9 == 0) {
            std::cout << "\n";
            
            // extra gap after 3 rows
            if ((row + 1) % 3 == 0 && (row + 1) % 9 != 0) {
                std::cout << "\n"; 
            }
        }
    }
}
