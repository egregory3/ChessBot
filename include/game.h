#ifndef GAME_H
#define GAME_H
#include <bitset>
#include <string>
#include <cstring>
#include <cstdlib>
#include <map>
#include <iostream>

using namespace std;

const string WHITE = "white";
const string BLACK = "black";

struct Comparer {
    bool operator() (const bitset<64> &b1, const bitset<64> &b2) const {
        return b1.to_ulong() < b2.to_ulong();
    }
};

bitset<64> get_valid_moves(char[65], int, string);

class game
{
    public:
        game();
        //game(char[65])
        virtual ~game();
        bitset<64> get_occupied(string);
        bitset<64> get_occupied();
        bitset<64> get_true_target_area(int, string);
        bitset<64> get_true_target_area(string);
        bitset<64> get_pseudo_target_area(int);
        bitset<64> get_pseudo_target_area(string);
        int* get_player_posns(string);
        bool make_move(int, int, string, bool);
        bool is_check(string);
        /* size = 65 because of \0 */
        char game_board[65];
        int pawn_promotion = -1;
        bitset<64> white_occupied;
        bitset<64> black_occupied;
        map<string, bool[2]> castle;
        map<int, bitset<64>, Comparer> target_area;
};

#endif // GAME_H
