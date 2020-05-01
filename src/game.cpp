#include "game.h"
/*
game::game(char board[64])
{
    //constructor
    game_board = board
    for(int i=0; i < 64; i++){
        if(board[i] != 'f' &&
            (board[i]) == WHITE)
            white_occupied[i] = 1;
        else if(board[i] != 'f' &&
                    (board[i]) == BLACK)
            black_occupied[i] = 1;
    }

}
*/


struct Updation{
    int updation;
    int times;
};

void disp_board(bitset<64> board){
    for(int i=0;i<64;i++){
        if(i % 8 == 0)
            cout<<'\n';
        cout<<board[63-i];

    }
}

void disp_board(string board){
    for(int i=0;i<64;i++){
        if(i % 8 == 0)
            cout<<"\n\n";
        cout<<board[i]<<' ';

    }
}

void set_bit(bitset<64> &bit_board, short pos, short val=1){
    bit_board.set(63 - pos, val);
}

bool test_bit(bitset<64> bit_board, short pos){
    return bit_board.test(63 - pos);
}


vector<int> get_true_pos(bitset<64> board){
    // Find the true values in a bitboard
    vector<int> posns;
    for(int i=0;i<64;i++){
        if(test_bit(board, i))
            posns.push_back(i);
    }

    return posns;
}


string get_player(char piece){
    if(piece == 'f')
            return "";
    if(piece > 96)
        return BLACK;
    return WHITE;
}

bool can_go_left(int pos){
    return pos % 8 != 0;
}

bool can_go_right(int pos){
    return pos % 8 != 7;
}

int how_many_times_can_go_left(int pos){
    return pos % 8;
}

int how_many_times_can_go_right(int pos){
    return 7 - pos % 8;
}

string reverse_player(string player){
    if(player == WHITE)
        return BLACK;
    return WHITE;
}

bitset<64> move_generator(string board, int pos, array<Updation, 4> updation, int valid_updations){
    int pos_temp;
    bitset<64> area (0);
    for(int i=0, k=0; i < valid_updations; i++){
        pos_temp = pos + updation[i].updation;
        for(k=0;pos_temp >= 0 && pos_temp <= 63; pos_temp += updation[i].updation, k++){
            if(updation[i].times == k)
                break;
            set_bit(area, pos_temp);
            if(board[pos_temp] != 'f')
                // Current col is occupied
                break;
        }
    }
    return area;
}



int find_king(string board, string player){
    char target;
    if(player == WHITE)
        target = 'K';
    else
        target = 'k';
    for(int i=0;i<64;i++)
        if(board[i] == target)
            return i;
    return -1;
}

bitset<64> get_valid_moves(string board, int pos, string player, array<bool, 2> white_castle, array<bool, 2> black_castle){
    bitset<64> area (0);
    if(tolower(board[pos]) == 'p'){
        int dirn;
        bool initial_move, en_passant_pos;
        if(get_player(board[pos]) == WHITE){
            dirn = -1;
            en_passant_pos = pos <= 31 && pos >= 24;
            initial_move = pos <= 55 && pos >= 48;
        }
        else{
            dirn = 1;
            initial_move = pos <= 15 && pos >= 8;
            en_passant_pos = pos <= 47 && pos >= 40;
        }
        set_bit(area, pos + 8*dirn);
        if(board[pos + 8*dirn] == 'f' && initial_move)
            // Initial Double Step
            set_bit(area, pos + 16*dirn);
        if(can_go_left(pos))
            // Left Side Capture
            set_bit(area, pos + 8*dirn - 1);
        if(can_go_right(pos))
            // Right Side Capture
            set_bit(area, pos + 8*dirn + 1);
        // ENPASSANT GOES HERE
        if(can_go_left(pos) && tolower(board[pos - 1]) == 'p' && get_player(board[pos - 1]) == reverse_player(player) && en_passant_pos)
            set_bit(area, pos + 8*dirn - 1);
        if(can_go_right(pos) && tolower(board[pos + 1]) == 'p' && get_player(board[pos + 1]) == reverse_player(player) && en_passant_pos)
            set_bit(area, pos + 8*dirn + 1);

    }
    else if(tolower(board[pos]) == 'n'){
        array<int, 8> posns;
        short k=0;
        if(can_go_left(pos)){
            posns[k++] = -17; // up
            posns[k++] = 15; // down
            if(can_go_left(pos - 1)){
                posns[k++] = 6; // down
                posns[k++] = -10; // up
            }
        }
        if(can_go_right(pos)){
            posns[k++] = -15; // up
            posns[k++] = 17; // down
            if(can_go_right(pos + 1)){
                posns[k++] = -6; // up
                posns[k++] = 10; // down
            }
        }
        for(int i=0; i < k; i++)
            if( pos + posns[i] >= 0 && pos + posns[i] < 64)
                set_bit(area, pos + posns[i]);
    }
    else if(tolower(board[pos]) == 'k'){
        array<int, 10> posns = {-8, 8, 0, 0, 0, 0, 0, 0, 0, 0};
        short k = 2;
        if(can_go_left(pos)){
            posns[k++] = -1;
            posns[k++] = -9;
            posns[k++] = 7;
        }
        if(can_go_right(pos)){
            posns[k++] = 1;
            posns[k++] = -7;
            posns[k++] = 9;
        }
        if((white_castle[0] && player == WHITE) || (black_castle[0] && player == BLACK))
            // Queen Side Castling
            if(board[pos - 1] == 'f' && board[pos - 2] == 'f' && board[pos - 3] == 'f')
                posns[k++] = -2;
        if((white_castle[1] && player == WHITE) || (black_castle[1] && player == BLACK))
            // King side castling
            if(board[pos + 1] == 'f' && board[pos + 2] == 'f')
                posns[k++] = 2;
        for(int i=0; i < k; i++)
            if( pos + posns[i] >= 0 && pos + posns[i] < 64 )
                set_bit(area, pos + posns[i]);
    }
    else{
        // Q, B, R
        array<Updation, 4> rook_updation;
        array<Updation, 4> bishop_updation;
        rook_updation[0] = {-8, 7};
        rook_updation[1] = {8, 7};
        int r = 2, b = 0;
        // how_many_times_can_go_left and how_many_times_can_go_right returns the maximum
        // number of times the piece can go in that particular dirn if the board is empty
        // Since it isn't constrained to vertical bounds it may not always be true
        if(can_go_left(pos)){
            rook_updation[r++] = {-1, how_many_times_can_go_left(pos)};
            bishop_updation[b++] = {-9, how_many_times_can_go_left(pos)};
            bishop_updation[b++] = {7, how_many_times_can_go_left(pos)};
        }
        if(can_go_right(pos)){
           rook_updation[r++] = {1, how_many_times_can_go_right(pos)};
            bishop_updation[b++] = {-7, how_many_times_can_go_right(pos)};
            bishop_updation[b++] = {9, how_many_times_can_go_right(pos)};
        }
        if(tolower(board[pos]) == 'b')
            return move_generator(board, pos, bishop_updation, b);
        else if(tolower(board[pos]) == 'r')
            return move_generator(board, pos, rook_updation, r);
        else{
            // Queen = Rook + Bishop
            area = move_generator(board, pos, bishop_updation, b);
            area |= move_generator(board, pos, rook_updation, r);
        }
    }
    return area;
}

void set_piece_vals(map<char,int> &piece_vals, string max_player){
    int multiplier = 1; // By default max_player is black
    if(max_player == WHITE)
        multiplier = -1;
    // BLACK
    piece_vals['p'] = multiplier * 50;
    piece_vals['n'] = multiplier * 200;
    piece_vals['b'] = multiplier * 300;
    piece_vals['r'] = multiplier * 500;
    piece_vals['q'] = multiplier * 1000;
    piece_vals['k'] = multiplier * pow(10, 7);
    // WHITE
    piece_vals['P'] = multiplier * -50;
    piece_vals['N'] = multiplier * -200;
    piece_vals['B'] = multiplier * -300;
    piece_vals['R'] = multiplier * -500;
    piece_vals['Q'] = multiplier * -1000;
    piece_vals['K'] = multiplier * -1 * pow(10, 7);
}

game::game(){
    //constructor
    this->game_board = "rnbqkbnrppppppppffffffffffffffffffffffffffffffffPPPPPPPPRNBQKBNR";
    this->white_castle = {true, true};
    this->black_castle = {true, true};
    this->en_passant = -1;
    this->checked = false;
    this->pawn_promotion = -1;
    this->game_over = false;
    this->winner = "";
    for(int i=0; i < 64; i++){
        if(this->game_board[i] != 'f')
            this->set_occupied(get_player(this->game_board[i]), i, 1);
    }
    vector <int> white_pieces = get_true_pos(white_occupied);
    vector <int> black_pieces = get_true_pos(black_occupied);

    for(auto ptr = white_pieces.begin(); ptr != white_pieces.end(); ptr++){
        this->target_areas[*ptr] = get_valid_moves(this->game_board, *ptr, WHITE, this->white_castle, this->black_castle);
    }
    for(auto ptr = black_pieces.begin(); ptr != black_pieces.end(); ptr++)
        this->target_areas[*ptr] = get_valid_moves(this->game_board, *ptr, BLACK, this->white_castle, this->black_castle);
}

game::game(const game &old){
    // Copy Constructor
    this->game_board = old.game_board;
    this->white_castle = old.white_castle;
    this->black_castle = old.black_castle;
    this->en_passant = old.en_passant;
    this->game_over = old.game_over;
    this->checked = old.checked;
    this->winner = old.winner;
    this->black_occupied = old.black_occupied;
    this->white_occupied = old.white_occupied;
    this->target_areas = old.target_areas;
    this->pawn_promotion = old.pawn_promotion;
}

bitset<64> game::get_occupied(string player){
    // occupied by a specific player
    if(player == WHITE)
        return this->white_occupied;
    return this->black_occupied;
}

bitset<64> game::get_occupied(){
    // All occupied cols
    return this->black_occupied | this->white_occupied;
}

bitset<64> game::get_pseudo_target_area(int pos){
    // Get pseudo target area of a particular piece
    return this->target_areas[pos];
}

bitset<64> game::get_pseudo_target_area(string player){
    // Get pseudo target area of a player
    bitset<64> area (0);
    vector <int> posns = get_true_pos(this->get_occupied(player));
    for(auto posn=posns.begin();posn != posns.end(); posn++)
        area |= this->target_areas[*posn];
    return area;
}

bitset<64> game::get_true_target_area(int pos, string player){
    // Get true target area of piece
    if(tolower(this->game_board[pos]) == 'p'){
        // Fucking pawn with all the "special" moves
        int dirn;
        bitset<64> area (0);
        bool initial_move;
        if(player == WHITE){
            dirn = -1;
            initial_move = pos < 56 && pos > 47;
        }
        else{
            dirn = 1;
            initial_move = pos < 16 && pos > 7;
        }
        if(this->game_board[pos + 8*dirn] == 'f')
            // Normal Move
            set_bit(area, pos + 8*dirn);
        if(this->game_board[pos + 8*dirn] == 'f' && this->game_board[pos + 16*dirn] == 'f' && initial_move)
            // Initial Double Step
            set_bit(area, pos + 16*dirn);
        if(get_player(this->game_board[pos + 8*dirn - 1]) == reverse_player(player) && can_go_left(pos))
            // Left Capture
            set_bit(area, pos + 8*dirn - 1);
        if(get_player(this->game_board[pos + 8*dirn + 1]) == reverse_player(player) && can_go_right(pos))
            // Right Capture
            set_bit(area, pos + 8*dirn + 1);
        // EN PASSANT
        if(tolower(this->game_board[pos - 1]) == 'p' && get_player(this->game_board[pos - 1]) == reverse_player(player) && can_go_left(pos) && this->en_passant == pos - 1 + 8 * dirn && this->game_board[pos + 8 * dirn] == 'f')
            set_bit(area, pos + 8*dirn - 1);
        if(tolower(this->game_board[pos + 1]) == 'p' && get_player(this->game_board[pos + 1]) == reverse_player(player) && can_go_right(pos) && this->en_passant == pos + 1 + 8 * dirn&& this->game_board[pos + 8 * dirn] == 'f')
            set_bit(area, pos + 8*dirn + 1);
        return area;
    }
    return this->target_areas[pos] & ~this->get_occupied(player);
}

bitset<64> game::get_true_target_area(string player){
    // Get True target area of player
    bitset<64> area (0);
    for(int i = 0; i < 64; i++)
        if(get_player(this->game_board[i]) == player)
            area |= this->get_true_target_area(i, player);
    return area;
}

bool game::is_check(string player){
    int king_pos = find_king(this->game_board, player);
    if (king_pos == -1)
        // King is 'captured' by AI
        return true;
    return test_bit(this->get_true_target_area(reverse_player(player)), king_pos);
}

void game::update_status(string player){
    // Check For CheckMate Or Draw
    int status = this->check_game_over(reverse_player(player));
    if(status == 1){
        this->game_over = true;
        this->winner = player;
    }
    if(status == -1){
        this->game_over = true;
        this->winner = "";
    }
}

bool game::promote_pawn(int current, char piece, string player, bool ai=false){
    if(this->pawn_promotion == current){
        if(tolower(piece) == 'r' || tolower(piece) == 'q' || tolower(piece) == 'q'){
            if(player == WHITE)
                piece = toupper(piece);
            else
                piece = tolower(piece);
            this->game_board[current] = piece;
            this->target_areas[current] = get_valid_moves(this->game_board, current, player, this->white_castle, this->black_castle);
            if(!ai){
                this->update_status(player);
                this->checked = this->is_check(player);
            }
            return true;
        }
    }
    return false;
}

void game::set_occupied(string player, int index, bool val){
    if(player == WHITE)
        set_bit(this->white_occupied, index, val);
    else if(player == BLACK)
        set_bit(this->black_occupied, index, val);
}

int game::check_game_over(string player){
    // Check if current player is checkmated or game is drawn(If player has no legal move and not in check)
    // 1 -> Game Over => player has lost
    // 0 -> Game Not Over
    // -1 -> Game is drawn
    return 0;
    /**
     'moves' contain only legal moves
     If it is null, then the player has no legal moves
     Implying the game is either lost or drawn
    */

    vector<Move> moves = this->get_all_moves(player, true);

    if(moves.empty()){
        if(this->is_check(player))
            return 1;
        else
            return -1;
    }
    // 'player' has a legal move, implying not a draw or checkmate
    return 0;
}

bool game::is_capture(short current, short target){

    if(this->game_board[target] != 'f')
        // Normal capture
        return true;
    if((this->en_passant == target - 1 || this->en_passant == target + 1 ) && tolower(this->game_board[current]) == 'p')
        // En "Fucking" Passant
        return true;
    return false;
}

bool comparer(Move a, Move b){
    return !(a.score < b.score);
}

vector<Move> game::get_all_moves(string player, bool legal){
    vector<Move> non_captures;
    vector<Move> captures;
    vector<int> temp;

    map<char, int> piece_vals;
    set_piece_vals(piece_vals, player);
    for(int i=0; i < 64; i++){
        if(get_player(this->game_board[i]) == player){
            temp = get_true_pos(this->get_true_target_area(i, player));
            for(auto j=temp.begin(); j != temp.end(); j++){
                if(legal)
                    if(!this->make_move(i, *j, player, true, false))
                        continue;
                if(this->is_capture(i, *j))
                    captures.push_back(Move(i, *j, piece_vals[i] - piece_vals[*j]));
                else
                    non_captures.push_back(Move(i, *j, 0));
            }
        }
    }

    vector<Move> all_moves;
    sort(captures.begin(), captures.end(), comparer);
    all_moves.reserve(captures.size() + non_captures.size());
    all_moves.insert(all_moves.end(), captures.begin(), captures.end());
    all_moves.insert(all_moves.end(), non_captures.begin(), non_captures.end());
    return all_moves;
}

bool game::make_move(short current, short target, string player, bool _reverse, bool ai){
    if(!ai){
        if(get_player(this->game_board[current]) != player)
            return false;
        if(!test_bit(this->get_true_target_area(current, player), target))
            return false;
    }

    bitset<64> move_board(0);
    set_bit(move_board, current);
    set_bit(move_board, target);
    // These are the steps that should be reversed to restore game to previous state
    char old_piece = this->game_board[target];
    this->game_board[target] = this->game_board[current];
    this->game_board[current] = 'f';

    this->set_occupied(player, target, 1);
    this->set_occupied(player, current, 0);
    // Essential For Captures
    this->set_occupied(reverse_player(player), target, 0);
    array <bitset<64>, 64> old_target_area;
    old_target_area = this->target_areas;
    bool last_rank = (target < 64 && target > 55 && player == BLACK) || (target < 8 && target >= 0 && player == WHITE);
    if(tolower(this->game_board[target]) == 'p' && last_rank)
        this->target_areas[target] = 0;
    else
        this->target_areas[target] = get_valid_moves(this->game_board, target, player, this->white_castle, this->black_castle);
    this->target_areas[current] = 0;

    for(int i = 0; i < 64; i++)
        if((this->target_areas[i] & move_board).any())
            //update target_area
            this->target_areas[i] = get_valid_moves(this->game_board, i, get_player(this->game_board[i]), this->white_castle, this->black_castle);
    bool checked_;
    if (!ai)
        checked_ = this->is_check(player);
    else
        checked_ = false;
    if(_reverse || checked_){
        this -> target_areas = old_target_area;
        this -> game_board[current] = this->game_board[target];
        this -> game_board[target] = old_piece;
        this -> set_occupied(player, current, 1);
        this -> set_occupied(player, target, 0);
        if(old_piece != 'f')
            this -> set_occupied(reverse_player(player), target, 1);
        return !checked_;
    }
    if(tolower(this->game_board[target]) == 'r'){
        int left, right;
        if(player == WHITE){
            left = 56;
            right = 63;
        }
        else{
            left = 0;
            right = 7;
        }
        if(current == left){
            if(player == WHITE)
                this->white_castle[0] = false;
            else
                this->black_castle[0] = false;
        }
        else if(current == right){
            if(player == WHITE)
                this->white_castle[1] = false;
            else
                this->black_castle[1] = false;
        }
    }
    else if(tolower(this->game_board[target]) == 'k'){
        char rook;
        if(player == WHITE)
            rook = 'R';
        else
            rook = 'r';
        if(abs(target - current) == 2){
            if(target == current - 2){
                // Queen Side
                this->game_board[target + 1] = rook;
                this->game_board[target - 2] = 'f';
                this->target_areas[target + 1] = get_valid_moves(this->game_board, target + 1,  player, {false, false}, {false, false});
                this->set_occupied(player, target - 2, 0);
                this->set_occupied(player, target + 1, 1);
                this->target_areas[target - 2] = 0;
            }
            else{
                // King Side
                this->game_board[target - 1] = rook;
                this->game_board[target + 1] = 'f';
                this->target_areas[target - 1] = get_valid_moves(this->game_board, target - 1,  player, {false, false}, {false, false});
                this->set_occupied(player, target - 1, 1);
                this->set_occupied(player, target + 1, 0);
                this->target_areas[target + 1] = 0;
            }
        }
        if(player == WHITE)
            this->white_castle = {0, 0};
        else
            this->black_castle = {0, 0};
    }
    else if(tolower(this->game_board[target]) == 'p'){
        // Setting Up Pawn Promotion
        int dirn;
        if(player == WHITE)
            dirn = -1;

        else
            dirn = 1;

        char ai_piece = 'q';

        if(last_rank){
            if(!ai)
                this -> pawn_promotion = target;
            else
                this -> promote_pawn(target, ai_piece, player, true);

            return true;
        }
        // Implementing En Passant
        // En Passant doesn't affect legality of the move, so checking for check b4 implementing it is not a problem
        if(this->en_passant != -1)
            if((current + 1 == this->en_passant && can_go_right(current)) || (current - 1 == this->en_passant && can_go_left(current))){
                short enemy_pos;
                if(current + 1 == this->en_passant && can_go_right(target)){
                    enemy_pos = target + 1;
                }
                else if(current - 1 == this->en_passant && can_go_left(target)){
                    enemy_pos = target - 1;
                }
                this->game_board[enemy_pos] = 'f';
                this->set_occupied(reverse_player(player), enemy_pos, 0);
                this->target_areas[enemy_pos] = 0;
            }


        // Setting Up En Passant
        bool initial_move;
        if(player == BLACK)
            initial_move = current < 16 && current > 7 && target > 23 && target < 32;
        else
            initial_move = current < 56 && current > 47 && target < 40 && target > 31;
        if(initial_move)
            this->en_passant = target;
        else
            this->en_passant = -1;
    }
    else
        this->en_passant = -1;

    if(!ai){
        update_status(player);
        this->checked = this->is_check(reverse_player(player));
    }
    return true;
}

game::~game()
{
    //destructor
}


// AI functions

int nodes = 0;

int heuristic(game GameObj, string player, string max_player){
    map<char, int> piece_vals;
    int score = 0, player_score = 0, enemy_score = 0;
    short int multiplier = 1;
    bitset<64> psuedo_target_area = GameObj.get_pseudo_target_area(player);
    bool initial_rank;

    set_piece_vals(piece_vals, max_player);

    if(player != max_player)
        multiplier = -1;

    const short int const_protection = 5 * multiplier;

    for(int i = 0; i < 64; i++){
        initial_rank = (i > 7 && i < 16 && player == BLACK) || (i > 48 && i < 56 && player == WHITE);
        score += piece_vals[GameObj.game_board[i]];
        if(get_player(GameObj.game_board[i]) == player){
            player_score += abs(piece_vals[GameObj.game_board[i]]);
            if(psuedo_target_area[63 - i] && tolower(GameObj.game_board[i]) == 'p'){
                if(!initial_rank)
                    // Pawns at initial rank are protected, so to move them forward, protection score wont be applied at initial rank
                    score += const_protection;
            }
            else if(psuedo_target_area[63 - i] && tolower(GameObj.game_board[i]) != 'p' )
                score += const_protection;
        }
        else
            enemy_score += abs(piece_vals[GameObj.game_board[i]]);
    }

    nodes++;
    return score;
}

const int late_game_cutoff = 600;

bool check_null_move(game GameObj, string player){

    if(GameObj.is_check(player))
        return false;
    map<char, int> piece_vals;
    set_piece_vals(piece_vals, player);
    int score = 0;
    for(int i=0;i<64;i++)
        if(get_player(GameObj.game_board[i]) == player){
            score += piece_vals[GameObj.game_board[i]];
        }
    if(score  - pow(10, 7) <= late_game_cutoff)
        return false;

    return true;
}


const int draw_cutoff = 200;
const short int R = 2;

int minimax(game GameObj, string max_player, string player, bool is_max, short int depth, int alpha, int beta, bool null_move){
    game tempObj;

    if(depth == 0)
        return heuristic(GameObj, player, max_player);
    int val;

    // Null Move
    if(!null_move && depth >= R + 1 && check_null_move(GameObj, player)){
        val = -minimax(GameObj, max_player, reverse_player(player), !is_max, depth - R - 1, -beta, -beta + 1, true);
        if(val >= beta)
            return beta;
    }

    vector<Move> moves = GameObj.get_all_moves(player, false);

    if(is_max){
        for(auto i = moves.begin();i != moves.end(); i++){
            tempObj = game(GameObj);
            tempObj.make_move(i->current, i->target, player, false, true);
            val = minimax(tempObj, max_player, reverse_player(player), false, depth - 1, alpha, beta, null_move);
            alpha = max(alpha, val);
            if(beta <= alpha)
                break;
        }
        if(alpha == -1*pow(10, 5)){
            // No moves
            if(heuristic(GameObj, player, max_player) < -draw_cutoff && !GameObj.is_check(player))
                // Not in check implies draw...
                // Maximiser at a disadvantage, force a draw by providing high cutoff
                return pow(10, 5);
        }
        return alpha;
    }
    else{
        for(auto i = moves.begin();i != moves.end(); i++){
            tempObj = game(GameObj);
            tempObj.make_move(i->current, i->target, player, false, true);
            val = minimax(tempObj, max_player, reverse_player(player), true, depth - 1, alpha, beta, null_move);
            beta = min(val, beta);
            if(beta <= alpha)
                break;
        }
        if(beta == pow(10, 5)){
            if(heuristic(GameObj, player, max_player) > draw_cutoff && !GameObj.is_check(player))
                // Not in check implies draw...
                // Minimiser at a disadvantage, force a draw by providing high cutoff
                return -pow(10, 5);
        }
        return beta;
    }
}

Move call_ai(game GameObj, string player, short int depth){
    int beta = pow(10, 5);
    int alpha = -pow(10, 5);
    int temp_val;
    Move move_pos(-1, -1);
    vector<Move> moves = GameObj.get_all_moves(player, false);
    game tempObj;
    for(auto temp = moves.begin();temp != moves.end(); temp++){
        tempObj = game(GameObj);
        tempObj.make_move(temp->current, temp->target, player, false, true);
        temp_val = minimax(tempObj, player, reverse_player(player), false, depth - 1, alpha, beta, false);
        if(temp_val > alpha){
            move_pos.current = temp->current;
            move_pos.target = temp->target;
            alpha = temp_val;
        }
    }
    cout<<"Nodes "<<nodes<<' '<<alpha;
    return move_pos;
}

