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
        int r;
        int ri;
        int c;
        int ci;
        int b;
        int bi;
        std::bitset<10> vopen;
    };
    struct Grp {
        std::vector<int> members;
        std::unordered_set<int> membersSet;
        std::bitset<10> popen[9];
    };
    
    struct Placement {
        uint16_t p;
        std::bitset<10> alts;
    };

    Posn posn[81];
    Grp row[9];
    Grp col[9];
    Grp blk[9];
    int spot[10][9][9] = {};
    std::vector<Placement> stk = {};
    std::vector<int> branchStk;

    Suku() {
        for (int i = 0; i < 81; i++) {
            int r = i / 9;
            int c = i % 9;
            int b = (r / 3) * 3 + (c / 3);
            posn[i].r = r;
            posn[i].c = c;
            posn[i].b = b;
            row[r].members.push_back(i);
            row[r].membersSet.insert(i);
            int ri = row[r].members.size() - 1;
            posn[i].ri = ri;
            col[c].members.push_back(i);
            col[c].membersSet.insert(i);
            int ci = col[c].members.size() - 1;
            posn[i].ci = ci;
            blk[b].members.push_back(i);
            blk[b].membersSet.insert(i);
            int bi = blk[b].members.size() - 1;
            posn[i].bi = bi;
        }
    }
    
    int valArray[81];

    bool readBoard(std::string filename);
    void writeBoard();
    int check_placement(const int p, const int v);
    void adjust_for_add(int py, int vx);
    void adjust_for_remove(int py, int vx);
    int add_placement(const int p, const int v);
    void remove_placement(const int p, const int v);
};

#endif

