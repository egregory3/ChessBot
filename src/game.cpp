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

int* get_true_pos(bitset<64> board){
    // Find the true values in a bitboard
    int *posns = (int*)malloc(sizeof(int)*17);
    int k=0;
    for(int i=0;i<64;i++){
        if(board[63-i])
            posns[k++] = i;
    }
    // Set last element to -1 to indicate arr has ended
    posns[k] = -1;
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

bitset<64> move_generator(string board, int pos, Updation updation[4], int valid_updations){
    int pos_temp;
    bitset<64> area (0);
    for(int i=0, k=0; i < valid_updations; i++){
        pos_temp = pos + updation[i].updation;
        for(k=0;pos_temp >= 0 && pos_temp <= 63; pos_temp += updation[i].updation, k++){
            if(updation[i].times == k)
                break;
            area[63-pos_temp] = 1;
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

bitset<64> get_valid_moves(string board, int pos, string player, map<string, bool[2]> castle){
    bitset<64> area (0);
    if(tolower(board[pos]) == 'p'){
        int dirn;
        bool initial_move, en_passant_pos;
        if(player == WHITE){
            dirn = -1;
            en_passant_pos = pos <= 31 && pos >= 24;
            initial_move = pos <= 55 && pos >= 48;
        }
        else{
            dirn = 1;
            initial_move = pos <= 15 && pos >= 8;
            en_passant_pos = pos <= 47 && pos >= 40;
        }
        area[63 - (pos + 8*dirn)] = 1;
        if(board[pos + 8*dirn] == 'f' && initial_move)
            // Initial Double Step
            area[63 - (pos + 16*dirn)] = 1;
        if(can_go_left(pos))
            // Left Side Capture
            area[63 - (pos + 8*dirn - 1)] = 1;
        if(can_go_right(pos))
            // Right Side Capture
            area[63 - (pos + 8*dirn + 1)] = 1;
        // ENPASSANT GOES HERE
        if(can_go_left(pos) && board[pos - 1] == 'f' && get_player(board[pos - 1]) == reverse_player(player) && en_passant_pos)
            area[63 - (pos + 8*dirn - 1)] = 1;
        if(can_go_right(pos) && board[pos + 1] == 'f' && get_player(board[pos + 1]) == reverse_player(player) && en_passant_pos)
            area[63 - (pos + 8*dirn + 1)] = 1;

    }
    else if(tolower(board[pos]) == 'n'){
        int posns[8],k=0;
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
                area[63 - (pos + posns[i])] = 1;
    }
    else if(tolower(board[pos]) == 'k'){
        int posns[10] = {-8, 8, 0, 0, 0, 0, 0, 0, 0, 0}, k = 2;
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
        if(castle[player][0])
            // Queen Side Castling
            if(board[pos - 1] == 'f' && board[pos - 2] == 'f' && board[pos - 3] == 'f')
                posns[k++] = -2;
        if(castle[player][1])
            // King side castling
            if(board[pos + 1] == 'f' && board[pos + 2] == 'f')
                posns[k++] = 2;
        for(int i=0; i < k; i++)
            if( pos + posns[i] >= 0 && pos + posns[i] < 64 )
                area[63 - (pos + posns[i])] = 1;
    }
    else{
        // Q, B, R
        Updation rook_updation[4];
        Updation bishop_updation[4];
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


game::game(){
    //constructor
    this->game_board = "rnbqkbnrppppppppffffffffffffffffffffffffffffffffPPPPPPPPRNBQKBNR";
    this->castle[WHITE][0] = true;
    this->castle[WHITE][1] = true;
    this->castle[BLACK][0] = true;
    this->castle[BLACK][1] = true;
    this->en_passant = -1;
    this->checked = false;
    this->pawn_promotion = -1;
    this->game_over = false;
    this->winner = "";
    for(int i=0; i < 64; i++){
        if(this->game_board[i] != 'f')
            this->set_occupied(get_player(this->game_board[i]), i, 1);
    }
    int *white_pieces = get_true_pos(white_occupied);
    int *black_pieces = get_true_pos(black_occupied);
    int *ptr;
    for(ptr = white_pieces; *ptr != -1; ptr++){
        this->target_area[*ptr] = get_valid_moves(this->game_board, *ptr, WHITE, this->castle);
    }
    for(ptr = black_pieces; *ptr != -1; ptr++)
        this->target_area[*ptr] = get_valid_moves(this->game_board, *ptr, BLACK, this->castle);
}

game::game(const game &old){
    // Copy Constructor

    this->game_board = old.game_board;
    this->castle.insert(old.castle.begin(), old.castle.end());
    this->en_passant = old.en_passant;
    this->game_over = old.game_over;
    this->checked = old.checked;
    this->winner = old.winner;
    this->black_occupied = old.black_occupied;
    this->white_occupied = old.white_occupied;
    this->target_area.insert(old.target_area.begin(), old.target_area.end());
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
    return this->target_area.at(pos);
}

bitset<64> game::get_pseudo_target_area(string player){
    // Get pseudo target area of a player
    bitset<64> area (0);
    int *posns = get_true_pos(this->get_occupied(player));
    for(;*posns != -1; posns++)
        area |= this->target_area.at(*posns);
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
            area[63-(pos + 8*dirn)] = 1;
        if(this->game_board[pos + 8*dirn] == 'f' && this->game_board[pos + 16*dirn] == 'f' && initial_move)
            // Initial Double Step
            area[63-(pos + 16*dirn)] = 1;
        if(get_player(this->game_board[pos + 8*dirn - 1]) == reverse_player(player))
            // Left Capture
            area[63-(pos + 8*dirn - 1)] = 1;
        if(get_player(this->game_board[pos + 8*dirn + 1]) == reverse_player(player))
            // Right Capture
            area[63-(pos + 8*dirn + 1)] = 1;
        // EN PASSANT
        if(tolower(this->game_board[pos - 1]) == 'p' && get_player(this->game_board[pos - 1]) == reverse_player(player) && can_go_left(pos) && this->en_passant != -1)
            area[63 - (pos - 1 + 8 * dirn)] = 1;
        if(tolower(this->game_board[pos + 1]) == 'p' && get_player(this->game_board[pos + 1]) == reverse_player(player) && can_go_right(pos) && this->en_passant != -1)
            area[63 - (pos + 1 + 8 * dirn)] = 1;
        return area;
    }
    return this->target_area.at(pos) & ~this->get_occupied(player);
}

bitset<64> game::get_true_target_area(string player){
    // Get True target area of player
    bitset<64> area (0);
    for(int i = 0; i < 64; i++)
        if(get_player(this->game_board[i]) == player)
            area |= this->get_true_target_area(i, player);
    return area;
}

int* game::get_checked_pieces(string player){
    int king_pos = find_king(this->game_board, player);
    int arr[16], k=0;
    for(int i=0;i<64;i++)
        if (get_player(this->game_board[i]) == reverse_player(player) && target_area[i][63-king_pos])
           arr[k++] = i;
    arr[k] = -1; // To indicate end of arr
    int* pieces = arr;
    return pieces;
}

bool game::is_check(string player){
    int king_pos = find_king(this->game_board, player);
    if (king_pos == -1)
        // King is 'captured' by AI
        return true;
    return this->get_true_target_area(reverse_player(player))[63-king_pos];
}

void game::update_status(string player){
    // Check For CheckMate Or Draw
    int status = 0;//this->check_game_over(reverse_player(player));
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
            this->target_area[current] = get_valid_moves(this->game_board, current, player, this->castle);
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
        this->white_occupied[63- index] = val;
    else if(player == BLACK)
        this->black_occupied[63- index] = val;
}

int game::check_game_over(string player){
    // Check if current player is checkmated or game is drawn(If player has no legal move and not in check)
    // 1 -> Game Over => player has lost
    // 0 -> Game Not Over
    // -1 -> Game is drawn

    /*
     'moves' contain only legal moves
     If it is null, then the player has no legal moves
     Implying the game is either lost or drawn
    */

    Move* moves = this->get_all_moves(player, true);

    if(moves == nullptr){
        if(this->is_check(player))
            return 1;
        else
            return -1;
    }
    // 'player' has a legal move, implying not a draw or checkmate
    return 0;

}

Move* game::get_all_moves(string player, bool legal){
    Move *moves = nullptr;
    int *temp = nullptr;
    for(int i=0; i < 64; i++){
        if(get_player(this->game_board[i]) == player){
            temp = get_true_pos(this->get_true_target_area(i, player));
            for(; *temp != -1; temp++){
                if(legal)
                    if(!this->make_move(i, *temp, player, true, false))
                        continue;
                Move *temp_move = new Move;
                temp_move->current = i;
                temp_move->target = *temp;
                temp_move->next = moves;
                moves = temp_move;
            }
        }
    }
    return moves;
}

bool game::make_move(int current, int target, string player, bool _reverse, bool ai){
    if(!ai){
        if(get_player(this->game_board[current]) != player)
            return false;
        if(!this->get_true_target_area(current, player)[63 - target])
            return false;
    }

    bitset<64> move_board(0);

    move_board[63-current] = 1;
    move_board[63-target] = 1;
    // These are the steps that should be reversed to restore game to previous state
    char old_piece = this->game_board[target];
    this->game_board[target] = this->game_board[current];
    this->game_board[current] = 'f';

    this->set_occupied(player, target, 1);
    this->set_occupied(player, current, 0);
    // Essential For Captures
    this->set_occupied(reverse_player(player), target, 0);
    map<int, bitset<64>,Comparer> old_target_area;
    old_target_area.insert(this->target_area.begin(), this->target_area.end());
    this->target_area[target] = get_valid_moves(this->game_board, target, player, this->castle);
    this->target_area.erase(current);
    map<int, bitset<64>,Comparer>::iterator itr;
    for(itr = this->target_area.begin(); itr != this->target_area.end(); itr++)
        if((itr->second & move_board).any())
            //update target_area
            this->target_area[itr->first] = get_valid_moves(this->game_board, itr->first, get_player(this->game_board[itr->first]), this->castle);
    bool checked_;
    if (!ai)
        checked_ = this->is_check(player);
    else
        checked_ = false;
    if(_reverse || checked_){
        this -> target_area.clear();
        this -> target_area.insert(old_target_area.begin(), old_target_area.end());
        this -> game_board[current] = this->game_board[target];
        this -> game_board[target] = old_piece;
        this -> set_occupied(player, current, 1);
        this -> set_occupied(player, target, 0);
        if(old_piece != 'f')
            this -> set_occupied(reverse_player(player), current, 1);
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
        if(current == left)
            this->castle[player][0] = false;
        else if(current == right)
            this->castle[player][1] = false;
    }
    else if(tolower(this->game_board[target]) == 'k'){
        char rook;
        if(player == WHITE)
            rook = 'R';
        else
            rook = 'r';
        if(abs(target - current) == 2){
            if(target == current - 2 && this->castle[player][0]){
                // Queen Side
                this->game_board[target + 1] = rook;
                this->game_board[target - 2] = 'f';
                this->target_area[target + 1] = get_valid_moves(this->game_board, target + 1,  player, this->castle);
                this->set_occupied(player, target - 2, 0);
                this->set_occupied(player, target + 1, 1);
                this->target_area.erase(target - 2);
            }
            else{
                // King Side
                this->game_board[target - 1] = rook;
                this->game_board[target + 1] = 'f';
                this->target_area[target - 1] = get_valid_moves(this->game_board, target - 1,  player, this->castle);
                this->set_occupied(player, target - 1, 1);
                this->set_occupied(player, target + 1, 0);
                this->target_area.erase(target + 1);
            }
        }
        this->castle[player][0] = false;
        this->castle[player][1] = false;

    }
    else if(tolower(this->game_board[target]) == 'p'){
        // Setting Up Pawn Promotion
        bool last_rank;
        int dirn;
        if(player == WHITE){
            last_rank = target < 8 && target >= 0;
            dirn = -1;
        }
        else{
            last_rank = target > 55 && target < 64;
            dirn = 1;
        }
        if(last_rank){
            this -> pawn_promotion = target;
            return true;
        }
        // Implementing En Passant
        // En Passant doesn't affect legality of the move, so checking for check b4 implementing it is not a problem
        if(this->en_passant != -1)
            if(target == this->en_passant){
                this->game_board[target + 8 * -dirn] = 'f';
                this->set_occupied(reverse_player(player), target + 8 * -dirn, 0);
                this->target_area.erase(target + 8 * -dirn);
            }


        // Setting Up En Passant
        bool initial_move;
        if(player == BLACK)
            initial_move = current < 16 && current > 7 && target > 23 && target < 32;
        else
            initial_move = current < 56 && current > 47 && target < 40 && target > 31;
        if(initial_move)
            this->en_passant = target + 8 * -dirn;
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

signed int heuristic(game GameObj, string player, string max_player, int& i){
    map<char, int> piece_vals;
    int score = 0;
    set_piece_vals(piece_vals, max_player);
    for(int i = 0; i < 64; i++)
        score += piece_vals[GameObj.game_board[i]];
    i++;
    return score;
}

int minimax(game GameObj, string max_player, string player, bool is_max, short int depth, int alpha, int beta, int &i){
    if(depth == 0)
        return heuristic(GameObj, player, max_player, i);

    int val;
    Move *temp = GameObj.get_all_moves(player, false);
    if(is_max){
        for(;temp != nullptr; temp=temp->next){
            GameObj.make_move(temp->current, temp->target, player, false, true);
            val = minimax(GameObj, max_player,reverse_player(player), false, depth - 1, alpha, beta, i);
            alpha = val > alpha ? val : alpha;
            if(beta <= alpha)
                break;
        }
        if(alpha == -1*pow(10, 5)){
            // No moves
        }
        return alpha;
    }
    else{
        for(;temp != nullptr; temp=temp->next){
            GameObj.make_move(temp->current, temp->target, player, false, true);
            val = minimax(GameObj, max_player, reverse_player(player), true, depth - 1, alpha, beta, i);
            beta = beta > val ? val : beta;
            if(beta <= alpha)
                break;
        }
        if(beta == pow(10, 5)){
            // No moves
        }
        return beta;
    }
}

Move call_ai(game GameObj, string player, short int depth){
    int beta = pow(10, 5);
    int alpha = -1 * pow(10, 5);
    int temp_val;
    Move move_pos(-1, -1);
    Move *temp = GameObj.get_all_moves(player, false);
    int i=0;
    for(;temp != nullptr; temp=temp->next){
        i++;
        GameObj.make_move(temp->current, temp->target, player, false, true);
        temp_val = minimax(GameObj, player, reverse_player(player), false, depth - 1, alpha, beta, i);
        if(temp_val > alpha){
            move_pos.current = temp->current;
            move_pos.target = temp->target;
            alpha = temp_val;
        }
    }
    cout<<"Nodes "<<i;
    return move_pos;
}

