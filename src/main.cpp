#include "suku.h"
#include <iostream>

int main() {
    int flatArray[81] = {};
    Suku suku;

    suku.readMatrix("data/pz001.txt", flatArray);
    suku.writeMatrix(flatArray);
    
    return 0;
}

