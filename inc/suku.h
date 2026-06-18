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
        std::bitset<10> vopen = 0b1111111110;
    };
    struct Grp {
        std::vector<int> members;
        std::unordered_set<int> membersSet;
        std::bitset<9> popen[10] = {
            0b111111111, 0b111111111, 0b111111111, 0b111111111, 0b111111111, 
            0b111111111, 0b111111111, 0b111111111, 0b111111111, 0b111111111 
        };
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
    // For backtracking, push the value of stkCtr when
    // a placement with alternatives has been added to stk
    std::vector<int> branchStk;

    Suku() {
        for (int pi = 0; pi < 81; pi++) {
            int r = pi / 9;
            int c = pi % 9;
            int b = (r / 3) * 3 + (c / 3);

            posn[pi].rcb[0] = r;  // row of posn[pi]
            posn[pi].rcb[1] = c;  // column of posn[pi]
            posn[pi].rcb[2] = b;  // sub-block of posn[pi]

            for (int gi = 0; gi < 3; gi++) {
                grp[gi][posn[pi].rcb[gi]].members.push_back(pi); // vector of members in r, c, b
                // index of posn pi in r, c, b member list
                posn[pi].rcbIdx[gi] = grp[gi][posn[pi].rcb[gi]].members.size() - 1;
                grp[gi][posn[pi].rcb[gi]].membersSet.insert(pi); // unordered set of members in r, c, b
            }
        }
    }

    bool readBoard(std::string filename);
    void writeBoard();
    int rcb(int pi, int vi);
    int rcbi(int pi, int vi);
    std::bitset<9>& pgopenBits(int v, int pi, int gi);
    bool check_placement(const int p, const int v);
    void adjust_for_add(int py, int vx);
    void adjust_for_remove(int py, int vx);
    bool find_placements();
    bool find_alt_placement();
    bool add_placement(Placement plcmntx, bool check=false);
    void remove_placement();
    int solve();
};

#endif

