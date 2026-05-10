#include "suku.h"
#include <fstream>
#include <iomanip>
#include <iostream>

#include <vector>

bool Suku::readBoard(const std::string filename) {
    int valArray[81];
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Failed to open file" << std::endl;
        return false;
    }
    for (int i = 0; i < 81; ++i) {
        if (!(file >> valArray[i])) {
            std::cout << "Too few values in starting board" << std::endl;
            return false;
        }
    }

    // Check legality
    uint8_t rowValToPosnIndx[9][10];
    uint8_t colValToPosnIndx[9][10];
    uint8_t blkValToPosnIndx[9][10];
    uint8_t posnToVal[81];

    for (int pi = 0; pi < 81; pi++) {
        int v = valArray[pi];
        int r = posn[pi].r;
        int c = posn[pi].c;
        int b = posn[pi].b;
        int ri = posn[pi].ri;
        int ci = posn[pi].ci;
        int bi = posn[pi].bi;
        if (v) {
            if (rowValToPosnIndx[r][v] != 0 && rowValToPosnIndx[r][v] != ri) {
                std::cout << "value " << v << " is duplicated in row " << r << std::endl;
                return false;
            } else {
                rowValToPosnIndx[r][v] = ri;
            }
            if (colValToPosnIndx[c][v] != 0 && colValToPosnIndx[c][v] != ci) {
                std::cout << "value " << v << " is duplicated in col " << c << std::endl;
                return false;
            } else {
                colValToPosnIndx[c][v] = ci;
            }
            if (blkValToPosnIndx[b][v] != 0 && blkValToPosnIndx[b][v] != bi) {
                std::cout << "value " << v << " is duplicated in blk " << b << std::endl;
                return false;
            } else {
                blkValToPosnIndx[b][v] = bi;
            }
            if (posnToVal[pi] != 0 && posnToVal[pi] != v) {
                std::cout << "Attempt to place two values at posn " << pi << std::endl;
                return false;
            } else {
                posnToVal[pi] = v;
            }
        }

        spot[0][r][c] = v;
        add_placement(pi, v);
        stk.emplace_back(pi, 0b0000000000 | 1 << v);
    }
    return true;
}

void Suku::writeBoard() {
    for (int i = 0; i < 81; ++i) {
        int row = i / 9;
        int col = i % 9;

        // Print the number with a consistent width
        std::cout << std::setw(1) << spot[0][row][col] << " ";

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

int Suku::check_placement(const int px, const int vx) {
    /*
    Check the placement. If it:
        is a rule violation, return -1
        is a duplicate, return 1
        is OK at this stage, return 0
    */
    int rx = posn[px].r;
    int cx = posn[px].c;
    int bx = posn[px].b;

    int v = spot[vx][rx][cx];
    if (v > 0) {
        if (v == vx )  { return 1; } // already added
        if (v != vx )  { return -1; } // value clash at position
    }
    if (row[rx].popen[vx].count() == 9) {
        return -1; // would duplicate value in row
    }
    if (col[cx].popen[vx].count() == 9) {
        return -1; // would duplicate value in col
    }
    if (blk[bx].popen[vx].count() == 9) {
        return -1; // would duplicate value in blk
    }

    return 0;
}

void Suku::adjust_for_add(int py, int vx) {
    // py: posn index, vx: value
    spot[vx][posn[py].r][posn[py].c] += 1;
    row[posn[py].r].popen[vx].set(posn[py].ri);
    col[posn[py].c].popen[vx].set(posn[py].ci);
    blk[posn[py].b].popen[vx].set(posn[py].bi);
    posn[py].vopen.set(vx);
}

void Suku::adjust_for_remove(int py, int vx) {
    // py: posn index, vx: value
    spot[vx][posn[py].r][posn[py].c] -= 1;
    if (spot[vx][posn[py].r][posn[py].c] == 0) {
        row[posn[py].r].popen[vx].reset(posn[py].ri);
        col[posn[py].c].popen[vx].reset(posn[py].ci);
        blk[posn[py].b].popen[vx].reset(posn[py].bi);
        posn[py].vopen.reset(vx);
    }
}

int Suku::add_placement(const int px, const int vx) {
    // Check: -1 => rule violation, 0 => OK, 1 => simple duplication
    int indic = check_placement(px, vx);
    if (indic) { return indic; }

    std::cout << "add plcmnt: " << px << ", " << vx << std::endl;

    // add to level 0 for board
    spot[0][posn[px].r][posn[px].c] = vx;

    int rx = posn[px].r;
    int cx = posn[px].c;
    int bx = posn[px].b;
    for (int py : row[rx].membersSet) {
        // Adjust spot, popen and vopen values
        adjust_for_add(py, vx);
    }
    for (int py : col[cx].membersSet) {
        // Adjust spot, popen and vopen values
        adjust_for_add(py, vx);
    }
    for (int py : blk[bx].membersSet) {
        // Adjust spot, popen and vopen values
        adjust_for_add(py, vx);
    }
    
    for (int vv = 1; vv < 10; vv++) {
        spot[vv][posn[px].r][posn[px].c] += 1;
        posn[px].vopen.set(vv);
    }
    row[rx].membersSet.erase(px);
    col[cx].membersSet.erase(px);
    blk[bx].membersSet.erase(px);

    return 0;
}

void Suku::remove_placement(const int px, const int vx) {
    std::cout << "remove plcmnt: " << px << ", " << vx << std::endl;

    // erase from level 0
    spot[0][posn[px].r][posn[px].c] = 0;

    int rx = posn[px].r;
    int cx = posn[px].c;
    int bx = posn[px].b;

    for (int py : row[rx].membersSet) {
        // Adjust spot, popen and vopen values
        adjust_for_remove(py, vx);
    }
    for (int py : col[cx].membersSet) {
        // Adjust spot, popen and vopen values
        adjust_for_remove(py, vx);
    }
    for (int py : blk[bx].membersSet) {
        // Adjust spot, popen and vopen values
        adjust_for_remove(py, vx);
    }

    for (int vv = 1; vv < 10; vv++) {
        spot[vv][posn[px].r][posn[px].c] -= 1;
        if (spot[vv][posn[px].r][posn[px].c] == 0) {
            posn[px].vopen.reset(vv);
        }
    }

    row[rx].membersSet.insert(px);
    col[cx].membersSet.insert(px);
    blk[bx].membersSet.insert(px);

    return;
}
