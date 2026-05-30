#include "suku.h"
#include <iostream>

int main() {
    Suku suku;

    if (suku.readBoard("data/pz001.txt")) {
        std::cout << std::endl;
        suku.writeBoard();

        std::cout << std::endl;
        suku.find_placements();
    
        // Suku::Placement plcmnt = {53, 0b0010010000};
        // int vx = plcmnt.alts._Find_first();
        // std::cout << "vx: " << vx << std::endl;

        return 0;
    } else {
        return -1;
    }
}
