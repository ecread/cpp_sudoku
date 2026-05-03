#include "suku.h"
#include <iostream>

int main() {
    int flatArray[81] = {};
    Suku suku;

    suku.readMatrix("data/pz001.txt", flatArray);
    suku.writeMatrix(flatArray);

    Suku::Placement plcmnt = {53, 0b0010010000};
    suku.add_placement(plcmnt);

    return 0;
}
