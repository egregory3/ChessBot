#include "game.h"

using namespace std;

string file_name = "moves.txt";


short convert_to_pos(char notation[3]){
    // Receives chess notation(eg A2) and converts to 'pos'
    short pos=0;
    pos += toupper(notation[0]) - 65;
    // 48 is the corresponding ASCII value of '0'
    pos += 8 * (8 - ((short)notation[1] - 48));
    return pos;
}

long compute_average(){
    ifstream move_file(file_name);
    short current, target;
    long time;
    long total_time=0;
    short moves=0;
    Move previous;
    string player;
    while(!move_file.eof()){
        moves++;
        move_file >> player;
        move_file >> current;
        move_file >> target;
        move_file >> time;
        if(previous != Move(current, target))
            total_time += time;
        previous = Move(current, target);
    }
    move_file.close();
    return total_time/moves;
}

void process_move_file(game &GameObj){
    ifstream move_file(file_name);
    short current, target;
    long time;
    string player;
    while(true){
        move_file >> player;
        move_file >> current;
        move_file >> target;
        move_file >> time;

        if(!GameObj.make_move(Move(current, target), player, false, true)){
                cout<<endl<<Move(current, target)<<' '<<player;
                disp_board(GameObj.game_board);

        }
        if(move_file.eof())
            break;
    }
    move_file.close();
}

void dump_move(string player, Move m, long duration){
    ofstream move_file(file_name, ios::app);
    move_file << player << ' ' << m.current << ' ' << m.target << ' ' << duration << '\n';
}

void get_status(game GameObj, string player){
    cout<<"Player: "<<player<<'\n';
    cout<<"Target Area(White):"<<'\n';
    disp_board(GameObj.get_true_target_area(WHITE));
    cout<<"\nTarget Area(Black):"<<'\n';
    disp_board(GameObj.get_true_target_area(BLACK));
    cout<<"\nCheck: "<<GameObj.checked;
    cout<<"\nGameOver: "<<GameObj.game_over;
    cout<<"\nWinner: "<<GameObj.winner<<endl;
}

int main()
{
    /// Initialize PRN
    bool LogMove = true;
    if(LogMove){
        ofstream move_file(file_name, std::ofstream::out | std::ofstream::trunc);
        move_file.close();
    }
    short hash_conflicts = 0;
    initialize_prn();
    game GameObj;
    string player = WHITE;
    const string human = WHITE;
    const string ai_player = WHITE;
    char notation_target[3], notation_current[3];
    Move a;
    int target, current;
    bool status;
    bool ai_only = true;
    bool play_game = true;
    bool multiplayer = true;

    GameObj.initial_zobrist_hash(player);

    //process_move_file(GameObj);

    while(!GameObj.game_over && play_game){
        disp_board(GameObj.game_board);
        cout<<"\n"<<player<<"\'s Turn";
        if((player == human || multiplayer )&& !ai_only){
            cout<<"\nEnter Current & Target: ";
            cin>>notation_current;
            cin>>notation_target;
            current = convert_to_pos(notation_current);
            target = convert_to_pos(notation_target);
            cout<<'\n'<<current<<' '<<target<<'\n';
            status = GameObj.make_move(Move(current, target), player, false, false);
        }
        else{
            auto start = std::chrono::high_resolution_clock::now();
            a = call_ai(GameObj, player, 7, hash_conflicts);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            cout<<'\n'<<a.current<<' '<<a.target<<' '<<duration.count()<<endl;
            if(LogMove)
                /**
                MOVE LOGGING
                */
                dump_move(player, a, duration.count());

            if(a == Move(-1, -1)){
                // no moves
                cout << "Stopped Because "<<player<<" returned -1,-1 as move";
                break;
            }
            status = GameObj.make_move(a, player, false, true);
        }
        if(status){
            if(GameObj.pawn_promotion != -1){
                char piece;
                bool result = false;
                while(!result){
                    cout<<"\nEnter Piece For Promotion: ";
                    cin>>piece;
                    result = GameObj.promote_pawn(target, piece, player, false);
                }

            }

            player = reverse_player(player);

            short GameStatus = GameObj.check_game_over(player);
            if(GameStatus == 1){
                /// Game is Lost
                GameObj.game_over = true;
                GameObj.winner = reverse_player(player);
            }
            else if(GameStatus == -1)
                /// Game is Drawn
                GameObj.game_over = true;
        }
        else
            cout<<"\n\nERROR\n\n";
    }
    disp_board(GameObj.game_board);
    cout<<endl;
    cout<<"Hash Conflicts: "<<hash_conflicts<<endl;
    if(GameObj.winner == "")
        cout<<"Drawn";
    else
        cout<<GameObj.winner<<" has won";
    return 0;
}
