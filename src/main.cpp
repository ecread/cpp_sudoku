#include "suku.h"
#include <iostream>

int main() {
    Suku suku;
    int rslt;
    std::string inFile = "data/pz001.txt";

    if (suku.readBoard(inFile)) {
        std::cout << std::endl;
        suku.writeBoard();
        std::cout << std::endl;

        rslt = suku.solve();

        if (rslt >= 0) {
            suku.writeBoard();
            if (rslt == 0) {
                std::cout << "Solution is unique." << std::endl;
                return 0;
            } else {
                std::cout << "Solution may not be unique." << std::endl;
            }
        } else {
            std::cout << "No solution." << std::endl;
            return 1;
        }
    
        return 0;
    } else {
        return -1;
    }
}
