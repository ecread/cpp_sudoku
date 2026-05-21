#ifndef SUKU_H
#define SUKU_H

#include <bit>
#include <bitset>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

class Suku {
public:
    struct Posn {
        // for posn i, the row, col and blk index
        int rcb[3];
        // for posn i, the member index in row,col, blk
        int rcbIdx[3]; 
        std::bitset<10> vopen;
    };
    struct Grp {
        std::vector<int> members;
        std::unordered_set<int> membersSet;

        /*
        popen[5] = 0b00000010100 (for example)
        */
        std::bitset<9> popen[10] = {};

        /*
        valToPosn[3] = 7 (for example) means:
        the value 3 is set on
        the Grp member (a position) indexed by 7
        int valToPosn[10];
        */
    };
    
    struct Placement {
        uint16_t p;
        std::bitset<10> alts;
    };

    Posn posn[81] = {};
    Grp grp[3][9] = {};
    int spot[10][9][9] = {};

    // Each time a placement is added to the board
    // set the stk member indexed by stkCtr to its value
    // and increment stkCtr
    Placement stk[81];
    // Count of placements added to board
    int stkCtr = 0;
    // Store candidate placements
    Placement tmpStk[81];
    // Count of placement candidates
    int tmpStkCtr = 0;
    // For backtracking, push the value of stkCtr when
    // a placement with alternatives has been added to stk
    std::vector<int> branchStk;

    Suku() {
        for (int i = 0; i < 81; i++) {
            int r = i / 9;
            int c = i % 9;
            int b = (r / 3) * 3 + (c / 3);

            posn[i].rcb[0] = r;
            posn[i].rcb[1] = c;
            posn[i].rcb[2] = b;

            grp[0][r].members.push_back(i);
            grp[0][r].membersSet.insert(i);
            int ri = grp[0][r].members.size() - 1;

            grp[1][c].members.push_back(i);
            grp[1][c].membersSet.insert(i);
            int ci = grp[1][c].members.size() - 1;

            grp[2][b].members.push_back(i);
            grp[2][b].membersSet.insert(i);
            int bi = grp[2][b].members.size() - 1;

            posn[i].rcbIdx[0] = ri;
            posn[i].rcbIdx[1] = ci;
            posn[i].rcbIdx[2] = bi;
        }
    }

    bool readBoard(std::string filename);
    void writeBoard();
    int check_placement(const int p, const int v);
    void adjust_for_add(int py, int vx);
    void adjust_for_remove(int py, int vx);
    bool find_placements();
    bool find_alt_placement();
    void add_placement(const int p, const int v);
    void remove_placement(const int p, const int v);
};

#endif

