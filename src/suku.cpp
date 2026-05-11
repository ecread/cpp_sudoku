#include "suku.h"
#include <fstream>
#include <iomanip>
#include <iostream>

#include <vector>

bool Suku::readBoard(const std::string filename) {
    int valArray[81] = {};
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
    int rowValToPosnIndx[9][10] = {};
    int colValToPosnIndx[9][10] = {};
    int blkValToPosnIndx[9][10] = {};
    int posnToVal[81] = {};

    candidatePlcmnts.clear();

    for (int pi = 0; pi < 81; pi++) {
        int v = valArray[pi];
        int r = posn[pi].rcb[0];
        int c = posn[pi].rcb[1];
        int b = posn[pi].rcb[2];
        int ri = posn[pi].rcbIdx[0];
        int ci = posn[pi].rcbIdx[1];
        int bi = posn[pi].rcbIdx[2];
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
            //add_placement(pi, v);
            candidatePlcmnts.emplace_back(pi, 0b0000000000 | 1 << v);
        }
    }

    for (size_t i = 0; i < candidatePlcmnts.size(); i++) {
        Placement plcmnt = candidatePlcmnts[i];
        add_placement(plcmnt.p, plcmnt.alts._Find_first());
    }

    return true;
}

void Suku::writeBoard() {
    for (int i = 0; i < 81; ++i) {
        int r = i / 9;
        int c = i % 9;

        // Print the number with a consistent width
        std::cout << std::setw(1) << spot[0][r][c] << " ";

        // extra gap after 3 numbers
        if ((c + 1) % 3 == 0 && (c + 1) % 9 != 0) {
            std::cout << "  "; 
        }

        // row spacing
        if ((c + 1) % 9 == 0) {
            std::cout << "\n";
            
            // extra gap after 3 rows
            if ((r + 1) % 3 == 0 && (r + 1) % 9 != 0) {
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
    int rx = posn[px].rcb[0];
    int cx = posn[px].rcb[1];
    int bx = posn[px].rcb[2];

    int v = spot[vx][rx][cx];
    if (v > 0) {
        if (v == vx )  { return 1; } // already added
        if (v != vx )  { return -1; } // value clash at position
    }
    if (grp[0][rx].popen[vx].count() == 9) {
        return -1; // would duplicate value in row
    }
    if (grp[1][cx].popen[vx].count() == 9) {
        return -1; // would duplicate value in col
    }
    if (grp[2][bx].popen[vx].count() == 9) {
        return -1; // would duplicate value in grp[2]
    }

    return 0;
}

void Suku::adjust_for_add(int py, int vx) {
    // py: posn index, vx: value
    spot[vx][posn[py].rcb[0]][posn[py].rcb[1]] += 1;
    for (int i = 0; i < 3; i++) {
        grp[i][posn[py].rcb[i]].popen[vx].set(posn[py].rcbIdx[i]);
    }
    posn[py].vopen.set(vx);
}

void Suku::adjust_for_remove(int py, int vx) {
    // py: posn index, vx: value
    spot[vx][posn[py].rcb[0]][posn[py].rcb[1]] -= 1;
    if (spot[vx][posn[py].rcb[0]][posn[py].rcb[1]] == 0) {
        for (int i = 0; i < 3; i++) {
            grp[i][posn[py].rcb[i]].popen[vx].reset(posn[py].rcbIdx[i]);
        }
        posn[py].vopen.reset(vx);
    }
}

void Suku::add_placement(const int px, const int vx) {
    // Check: -1 => rule violation, 0 => OK, 1 => simple duplication
    // int indic = check_placement(px, vx);
    // if (indic) { return indic; }

    std::cout << "add plcmnt: " << px << ", " << vx << std::endl;

    int rcbx[3];
    for (int i = 0; i < 3; i++) {
        rcbx[i] = posn[px].rcb[i];
    }

    // add to level 0 for board
    spot[0][rcbx[0]][rcbx[1]] = vx;

    // int rx = posn[px].rcb[0];
    // int cx = posn[px].rcb[1];
    // int bx = posn[px].rcb[2];
    // for (int py : grp[0][rx].membersSet) {
    //     // Adjust spot, popen and vopen values
    //     adjust_for_add(py, vx);
    // }
    // for (int py : grp[1][cx].membersSet) {
    //     // Adjust spot, popen and vopen values
    //     adjust_for_add(py, vx);
    // }
    // for (int py : grp[2][bx].membersSet) {
    //     // Adjust spot, popen and vopen values
    //     adjust_for_add(py, vx);
    // }

    for (int i = 0; i < 3; i++) {
        for (int py : grp[i][rcbx[i]].membersSet) {
            // Adjust spot, popen and vopen values
            adjust_for_add(py, vx); 
        } 
    }

    // for (int vv = 1; vv < 10; vv++) {
    //     spot[vv][posn[px].r][posn[px].c] += 1;
    //     posn[px].vopen.set(vv);
    // }

    for (int vv = 1; vv < 10; vv++) {
        spot[vv][rcbx[0]][rcbx[1]] += 1;
        posn[px].vopen.set(vv);
    }

    // grp[0][rx].membersSet.erase(px);
    // grp[1][cx].membersSet.erase(px);
    // grp[2][bx].membersSet.erase(px);

    for (int i = 0; i < 3; i++) {
        grp[i][rcbx[i]].membersSet.erase(px);
    }

    return;
}

void Suku::remove_placement(const int px, const int vx) {
    std::cout << "remove plcmnt: " << px << ", " << vx << std::endl;

    // int rx = posn[px].rcb[0];
    // int cx = posn[px].rcb[1];
    // int bx = posn[px].rcb[2];

    int rcbx[3];
    for (int i = 0; i < 3; i++) {
        rcbx[i] = posn[px].rcb[i];
    }

    // erase from level 0
    spot[0][rcbx[0]][rcbx[1]] = 0;


    // for (int py : grp[0][rx].membersSet) {
    //     // Adjust spot, popen and vopen values
    //     adjust_for_remove(py, vx);
    // }
    // for (int py : grp[1][cx].membersSet) {
    //     // Adjust spot, popen and vopen values
    //     adjust_for_remove(py, vx);
    // }
    // for (int py : grp[2][bx].membersSet) {
    //     // Adjust spot, popen and vopen values
    //     adjust_for_remove(py, vx);
    // }

    for (int i = 0; i < 3; i++) {
        for (int py : grp[i][rcbx[i]].membersSet) {
            // Adjust spot, popen and vopen values
            adjust_for_remove(py, vx);
        }
    }

    // for (int vv = 1; vv < 10; vv++) {
    //     spot[vv][posn[px].rcb[0]][posn[px].rcb[1]] -= 1;
    //     if (spot[vv][posn[px].rcb[0]][posn[px].rcb[1]] == 0) {
    //         posn[px].vopen.reset(vv);
    //     }
    // }

    for (int vv = 1; vv < 10; vv++) {
        spot[vv][rcbx[0]][rcbx[1]] -= 1;
        if (spot[vv][rcbx[0]][rcbx[1]] == 0) {
            posn[px].vopen.set(vv);
        }
    }

    // grp[0][rx].membersSet.insert(px);
    // grp[1][cx].membersSet.insert(px);
    // grp[2][bx].membersSet.insert(px);

    for (int i = 0; i < 3; i++) {
        grp[i][rcbx[i]].membersSet.insert(px);
    }

    return;
}
