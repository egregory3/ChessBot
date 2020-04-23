#include "game.h"

using namespace std;
using namespace std::chrono;


void disp_board(string board){
    for(int i=0;i<64;i++){
        if(i % 8 == 0)
            cout<<"\n\n";
        cout<<board[i]<<' ';

    }
}

int convert_to_pos(char notation[2]){
    // Receives chess notation(eg A2) and converts to 'pos'
    int pos=0;
    pos += toupper(notation[0]) - 65;
    // 48 is the corresponding ASCII value of '0'
    pos += 8 * (8 - ((int)notation[1] - 48));
    return pos;
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
    game GameObj;
    string player = WHITE;
    const string human = WHITE;
    const string ai_player = BLACK;
    char notation_target[2], notation_current[2];
    Move a;
    int target, current;
    bool status;
    while(!GameObj.game_over){
        disp_board(GameObj.game_board);
        cout<<"\n"<<player<<"\'s Turn";
        if(player == human){
            cout<<"\nEnter Current & Target: ";
            cin>>notation_current;
            cin>>notation_target;
            current = convert_to_pos(notation_current);
            target = convert_to_pos(notation_target);
            cout<<'\n'<<current<<' '<<target<<'\n';
            status = GameObj.make_move(current, target, player, false, false);
        }
        else{
            auto start = high_resolution_clock::now();
            a = call_ai(GameObj, player, 4);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout<<'\n'<<a.current<<' '<<a.target<<' '<<duration.count()<<endl;
            status = GameObj.make_move(a.current, a.target, player, false, true);
        }
        if(status){
            if(GameObj.pawn_promotion != -1){
                if(player != human){
                    GameObj.promote_pawn(a.target, 'q', player, true);
                }
                else{
                    char piece;
                    bool result = false;
                    while(!result){
                        cout<<"\nEnter Piece For Promotion: ";
                        cin>>piece;
                        result = GameObj.promote_pawn(target, piece, player, false);
                    }
                }
            }
            player = reverse_player(player);
        }
        else
            cout<<"\n\nERROR\n\n";
    }


    return 0;
}