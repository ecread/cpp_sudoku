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

    for (int pi = 0; pi < 81; pi++) {
        int v = valArray[pi];
        if (v) {
            if ( add_placement({(uint16_t)pi, 0b0000000000 | 1 << v}) == -1) {
                std::cout << "Board has rule violation at position: "
                    << pi << ", terminating" << std::endl;
                return false;
            }
        }
    }

    std::cout << "Initial count of placements: "
              << stkCtr << std::endl;

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

int Suku::check_placement(int px, int vx) {
    /*
    Check the placement. If it:
        is a rule violation, return -1
        is a duplicate, return 1
        is OK at this stage, return 0
    */

    int rcbx[3];
    for (int i = 0; i < 3; i++) {
        rcbx[i] = posn[px].rcb[i];
    }

    if (spot[0][rcbx[0]][rcbx[1]] != 0) {
        if (spot[0][rcbx[0]][rcbx[1]] == vx) {
            return 1; // found duplicate
        } else {
            return -1; // violation: position has placement with another value
        }
    } else {
        for (int i = 0; i < 3; i++) {
            if (grp[i][rcbx[i]].popen[vx].test(posn[px].rcbIdx[i]) != 0) {
                return -1; // violation: would duplicate value in row, col or block
            }
        }
    }

    return 0;
}

void Suku::adjust_for_add(int py, int vx) {
    // py: posn index, vx: value
    spot[vx][posn[py].rcb[0]][posn[py].rcb[1]] += 1;
    if (spot[vx][posn[py].rcb[0]][posn[py].rcb[1]] == 1) {
        for (int i = 0; i < 3; i++) {
            grp[i][posn[py].rcb[i]].popen[vx].set(posn[py].rcbIdx[i]);
        }
        posn[py].vopen.set(vx);
    }
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

//int Suku::add_placement(int px, int vx) {
int Suku::add_placement(Placement plcmntx) {
    // Check: -1 => rule violation, 0 => OK, 1 => simple duplication
    // int indic = check_placement(px, vx);
    // if (indic) { return indic; }

    int px = plcmntx.p;
    std::bitset<10> altsx = plcmntx.alts;
    int vx = altsx._Find_first();

    int checkVal;
    int rcbx[3];
    for (int i = 0; i < 3; i++) {
        rcbx[i] = posn[px].rcb[i];
    }

    // Check candidate placement
    checkVal = check_placement(px, vx);
    if (checkVal != 0) {
        return checkVal;
    }

    std::cout << "add plcmnt: " << px << ", " << vx << std::endl;
    // add to level 0 for board
    spot[0][rcbx[0]][rcbx[1]] = vx;

    for (int i = 0; i < 3; i++) {
        //for (int py : grp[i][rcbx[i]].membersSet) {
        for (int j = 0; j < 9; j++) {
            int py = grp[i][rcbx[i]].members[j];
            // Adjust spot, popen and vopen values
            adjust_for_add(py, vx); 
	        //std::cout << "adjust_for_add: " << py << "," << vx;
        } 
    }
    for (int vv = 1; vv < 10; vv++) {
        adjust_for_add(px, vv);
    }
    for (int i = 0; i < 3; i++) {
        grp[i][rcbx[i]].membersSet.erase(px);
    }

    // Add to stack
    //stk[stkCtr++] = {(uint16_t)px, 0b0000000000 | 1 << vx};
    stk[stkCtr++] = plcmntx;

    return 0;
}

void Suku::remove_placement(Placement plcmntx) {

    int px = plcmntx.p;
    std::bitset<10> altsx = plcmntx.alts;
    int vx = altsx._Find_first();

    std::cout << "remove plcmnt: " << px << ", " << vx << std::endl;

    int rcbx[3];
    for (int i = 0; i < 3; i++) {
        rcbx[i] = posn[px].rcb[i];
    }

    // erase from level 0
    spot[0][rcbx[0]][rcbx[1]] = 0;

    for (int i = 0; i < 3; i++) {
        //for (int py : grp[i][rcbx[i]].membersSet) {
        for (int j = 0; j < 9; j++) {
            int py = grp[i][rcbx[i]].members[j];
            // Adjust spot, popen and vopen values
            adjust_for_remove(py, vx);
        }
    }

    for (int vv = 1; vv < 10; vv++) {
        adjust_for_remove(px, vv);
    }

    for (int i = 0; i < 3; i++) {
        grp[i][rcbx[i]].membersSet.insert(px);
    }

    return;
}

bool Suku::find_placements() {
    std::bitset<9> tmp;
    int rslt = 0;
    bool found_forced = false;
    bool found_violation = false;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 9; j++) {
            for (int v = 1; v < 10; v++) {
                tmp = ~(grp[i][j].popen[v]);
                if (tmp.count() == 1) {
                    found_forced = true;
                    // find the only open position
                    int pix = tmp._Find_first();
                    int py = grp[i][j].members[pix];
                    std::cout << "Found candidate: " << py << ", " << v << std::endl;
                    
                    rslt = add_placement( {(uint16_t)py, 0b0000000000 | 1 << v} );
                    if (rslt == 1) {
                        std::cout << "duplicate" << std::endl;
                        continue;
                    } else if (rslt == -1) {
                        found_violation = true;
                        break;
                    } else {
                        found_forced;
                        continue;
                    }
                }
            }
            if (found_violation) {
                break;
            }
        }
        if (found_violation) {
            break;
        }
    }
    return true;
}

bool Suku::find_alt_placement() {
    return true;
}

int Suku::solve() {
    /*
    Repeatedly call find_placements() while it returns true
    and the count of placments on the board is less than 81.
    If 81 placements can be installed, return 0 if there are
    no alternatives to try, else return 1.
    If find_placements() returns false, it has encountered
    a rule violation; in this case return -1.
    */
    while (true) {
        if ( find_placements() ) {
            if (stkCtr == 81) {
                if (branchStk.size() == 0) {
                    return 0;
                } else {
                    return 1;
                }
            } else {
                continue;
            }
        } else {
            return -1;
        }
    }
}