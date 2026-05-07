#include "suku.h"
#include <iostream>

int main() {
    Suku suku;

    suku.readBoard("data/pz001.txt");
    suku.writeBoard();

    Suku::Placement plcmnt = {53, 0b0010010000};
    int vx = plcmnt.alts._Find_first();
    std::cout << "vx: " << vx << std::endl;
    suku.add_placement( 53, vx );

    return 0;
}
