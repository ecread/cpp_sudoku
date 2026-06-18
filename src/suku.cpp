#include "suku.h"
#include <fstream>
#include <iomanip>
#include <iostream>

#include <vector>

int Suku::rcb(int pi,int gi) {
    return posn[pi].rcb[gi];
}

int Suku::rcbi(int pi, int gi) {
    return posn[pi].rcbIdx[gi];
}

std::bitset<9>& Suku::pgopenBits(int v, int pi, int gi) {
    return grp[gi][rcb(pi, gi)].popen[v];
}

bool Suku::readBoard(const std::string filename) {
    bool checkit = true;
    int valArray[81] = {};
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Failed to open file" << std::endl;
        return false;
    }
    for (int pi = 0; pi < 81; ++pi) {
        if (!(file >> valArray[pi])) {
            std::cout << "Too few values in starting board" << std::endl;
            return false;
        }
    }

    for (int pi = 0; pi < 81; pi++) {
        int v = valArray[pi];
        if (v) {
            if ( !add_placement( {(uint16_t)pi, 0b0000000000 | 1 << v}, checkit ) ) {
                std::cout << "Board has rule violation at position: "
                    << pi << ", terminating" << std::endl;
                return false;
            }
        }
    }

    std::cout << "\nInitial count of placements: "
              << stkCtr << std::endl;

    return true;
}

void Suku::writeBoard() {
    for (int pi = 0; pi < 81; ++pi) {
        int r = pi / 9;
        int c = pi % 9;

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

bool Suku::check_placement(int px, int vx) {
    /*
    Check the placement.
    If it is a rule violation,
    return false, else return true
    */

    for (int i = 0; i < 3; i++) {  // HERE
        if ( !pgopenBits(vx, px, i).test(rcbi(px, i)) ) {
            return false; // violation: would duplicate value in row, col or block
        }
    }
    return true;
}

void Suku::adjust_for_add(int py, int vx) {
    spot[vx][posn[py].rcb[0]][posn[py].rcb[1]] += 1;
    if (spot[vx][posn[py].rcb[0]][posn[py].rcb[1]] == 1) {
        for (int gi = 0; gi < 3; gi++) {
            pgopenBits(vx, py, gi).reset(rcbi(py, gi));
        }
        posn[py].vopen.reset(vx);
    }
}

void Suku::adjust_for_remove(int py, int vx) {
    spot[vx][posn[py].rcb[0]][posn[py].rcb[1]] -= 1;
    if (spot[vx][posn[py].rcb[0]][posn[py].rcb[1]] == 0) {
        for (int gi = 0; gi < 3; gi++) {
            pgopenBits(vx, py, gi).set(rcbi(py, gi));
        }
        posn[py].vopen.set(vx);
    }
}

bool Suku::add_placement(Placement plcmntx, bool check) {
    static int pcount = 0;
    int px = plcmntx.p;
    std::bitset<10> altsx = plcmntx.alts;
    int vx = altsx._Find_first();
    int countx = altsx.count();
    
    if (check) {
        if (!check_placement(px, vx)) {
            return false;
        }
    }

    for (int gi = 0; gi < 3; gi++) {
        for (int j = 0; j < 9; j++) {
            int py = grp[gi][rcb(px, gi)].members[j]; // HERE
            adjust_for_add(py, vx); 
        } 
    }
    for (int v = 1; v < 10; v++) {
         adjust_for_add(px, v);
    }
    for (int gi = 0; gi < 3; gi++) {
        grp[gi][rcb(px, gi)].membersSet.erase(px);
    }

    // Add to stack
    stk[stkCtr++] = plcmntx;
    if (countx > 1) {
        branchStk.push_back(stkCtr);
    }

    // add to level 0 for board
    spot[0][rcb(px,0)][rcb(px,1)] = vx;

    if (countx > 1) {
        // placement with alternatives
        std::cout << "\n\t\tadvance, alternate placements count: "
            << branchStk.size() << ", "
            << "[" << std::setw(2) << plcmntx.p << "," 
            << plcmntx.alts << "]" << std::endl;
        pcount = 0;
    }
    std::cout << "[" << std::setw(2) << plcmntx.p << "," << vx << "]" <<  "  ";
    if (++pcount == 10) {
        std::cout << std::endl;
        pcount = 0;
    }

    return true;
}

void Suku::remove_placement() {
    Placement plcmntx = stk[--stkCtr];
    int px = plcmntx.p;
    std::bitset<10> altsx = plcmntx.alts;
    int vx = altsx._Find_first();

    // erase from level 0
    spot[0][rcb(px,0)][rcb(px,1)] = 0;

    for (int gi = 0; gi < 3; gi++) {
        for (int j = 0; j < 9; j++) {
            int py = grp[gi][rcb(px, gi)].members[j]; // HERE
            adjust_for_remove(py, vx); 
        } 
    }
    for (int v = 1; v < 10; v++) {
        adjust_for_remove(px, v);
    }

    for (int gi = 0; gi < 3; gi++) {
        grp[gi][rcb(px, gi)].membersSet.insert(px);
    }

    return;
}

bool Suku::find_placements() {
    std::bitset<9> bits9;
    std::bitset<10> bits10;
    bool found_forced = false;
    Placement plcmnt;

    int altsCnt = 9;
    int cnt;
    int pix = -1;

    if (stkCtr == 81) {
        std::cout << std::endl;
        return false;
    }
    
    for (int gi = 0; gi < 3; gi++) {
        for (int j = 0; j < 9; j++) {
            for (int v = 1; v < 10; v++) {
                bits9 = grp[gi][j].popen[v];
                if (bits9.count() == 1) {
                    // find the only open position
                    int pix = bits9._Find_first();
                    int py = grp[gi][j].members[pix];
                                        
                    plcmnt = {(uint16_t)py, 0b0000000000 | 1 << v};
                    add_placement(plcmnt);
                    found_forced = true;
                    continue;
                }
            }
        }
    }

    for (int pi = 0; pi < 81; pi++) {
        if (spot[0][pi/9][pi%9] != 0) {
            continue;
        }

        bits10 = posn[pi].vopen;
        cnt = bits10.count();

        if (found_forced) {
            if (cnt == 1) {
                plcmnt = {(uint16_t)pi, bits10};
                add_placement(plcmnt);
            }
        } else {
            if (cnt < altsCnt && cnt > 0) {
                altsCnt = cnt;
                pix = pi;
            }
        }
    }

    if (!found_forced) {
        if (pix >= 0) {
            bits10 = posn[pix].vopen;
        } else {
            return false;
        }
        plcmnt = {(uint16_t)pix, bits10};
        add_placement( plcmnt );
    }
    
    return true;
}

bool Suku::find_alt_placement() {
    if (branchStk.size() == 0) {
        return false;
    }
    int stkCntWhenAlts = branchStk[branchStk.size()-1];

    while (stkCtr >= stkCntWhenAlts) {
        remove_placement();
    }

    Placement plcmntx = stk[stkCtr];

    int px = plcmntx.p;
    std::bitset<10> altsx = plcmntx.alts;
    int vx = altsx._Find_first();
    altsx.reset(vx);
    plcmntx = {(uint16_t)px, altsx};

    int countx = altsx.count();
    if (countx == 1) {
        // placement no longer has alternates
        branchStk.pop_back();
    }

    std::cout  << "\n\t\tretreat, alternate placements count: "
        << branchStk.size() << ", "
        << plcmntx.p << "," << plcmntx.alts << std::endl;

    if ( add_placement({(uint16_t)px, altsx}) ) {
        return true;
    } else {
        return false;
    }
}

int Suku::solve() {
    /*
    "advance" action:
    Repeatedly call find_placements() while it returns true
    and the count of placments on the board is less than 81.
    If 81 placements can be installed, return 0 if there are
    no alternatives to try, else return 1.
    If find_placements() returns false call find_alt_placement().

    "retreat" (backtrack) action:
    If find_altPlacement() returns true, resume the "advance" action.
    Otherwise, no alternative placement may be found; return false.

    Note:
    Entry into this function when there have been 81 placements
    signals that another solution is being sought, so backtrack.
    */

   if (stkCtr == 81) {
        goto retreat;
   }

advance:
    while ( find_placements() ) {
        if (stkCtr == 81) {
            if (branchStk.size() == 0) {
                return 0;
            } else {
                return 1;
            }
        } else {
            continue;
        }
    }
retreat:
    // find_placements() encountered rule violation
    if ( find_alt_placement() ) {
        goto advance;
    } else {
        return -1;
    }
}
