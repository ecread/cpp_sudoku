#include "suku.h"
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
        std::cout << std::setw(1) << flatArray[i] << " ";

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

void Suku::add_placement(const Placement& plcmnt) {
    int px = (int)plcmnt.p;
    int v = plcmnt.alts._Find_first();
    std::cout << "plcmnt: " << px << ", " << v << std::endl;
    return;
}
