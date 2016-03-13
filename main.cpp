#include <iostream>

using namespace std;


#define H 2
#define C 1
#define EMPTY 0
#define C_WINS 5000
#define H_WINS -5000
#define DRAW 0
#define NOT_OVER -1

int b[3][3];
int MAXDEPTH = 9;


void makeMove();
int min(int depth);
int max(int depth);
int evaluate();
int checkForWinner();
void checkGameOver();
void getAMove();
void setup();
void printBoard();

int main() {
    setup();
    printBoard();
    for(;;){
        getAMove();
        checkGameOver();
        makeMove();
        checkGameOver();
    }
}

void setup(){
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            b[i][j] = EMPTY;
        }
    }
}

void printBoard(){
    cout << endl;
    cout << b[0][0] << " " << b[0][1] << " " << b[0][2] << endl;
    cout << b[1][0] << " " << b[1][1] << " " << b[1][2] << endl;
    cout << b[2][0] << " " << b[2][1] << " " << b[2][2] << endl;
}

void getAMove(){
    int i;
    int j;
    cout << "Enter your move: " ;
    cin >> i >> j;
    b[i][j] = H;
}

void checkGameOver(){
    printBoard();
    if(checkForWinner() == H_WINS) { // -5000
        cout << "you win" << endl;
        exit(0);
    }
    if(checkForWinner() == C_WINS){  // 5000
        cout << "I win " << endl;
        exit(0);
    }
    if(checkForWinner() == DRAW){
        cout << "Draw" << endl ;
        exit(0);
    }
}

void makeMove(){
    int best =-20000, depth = 1, score, mi, mj;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(b[i][j] == EMPTY){
                b[i][j] = C; //make move board
                score = min(depth+1);
                if(score > best){
                    mi = i;
                    mj = j;
                    best = score;
                }
                b[i][j] = EMPTY; //undo move
            }
        }
    }
    cout << "my move is" << mi << " " << mj << endl;
    b[mi][mj] = C;
}

int min(int depth){
    int best = 20000;
    int score;
    if(checkForWinner() != NOT_OVER) return (checkForWinner());
    if(depth == MAXDEPTH) return (evaluate());
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(b[i][j] == EMPTY){
                b[i][j] = H; //make move on board
                score = max(depth+1);
                if(score < best) best = score;
                b[i][j] = EMPTY; // undo move
            }
        }
    }
    return (best);
}

int max(int depth){
    int best =-20000;
    int score;
    if(checkForWinner() != NOT_OVER) return (checkForWinner());
    if(depth == MAXDEPTH) return (evaluate());
    for(int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            if(b[i][j] == EMPTY){
                b[i][j] = C; //make move on board
                score = min(depth + 1);
                if(score > best) best = score;
                b[i][j] = EMPTY; //undo move
            }
        }
    }
    return best;
}

int checkForWinner(){
    if ((b[0][0] == C ) && (b[0][1] == C ) && (b[0][2] == C )
        || (b[1][0] == C ) && (b[1][1] == C ) && (b[1][2] == C )
        || (b[2][0] == C ) && (b[2][1] == C ) && (b[2][2] == C )
        || (b[0][0] == C ) && (b[1][0] == C ) && (b[2][0] == C )
        || (b[0][1] == C ) && (b[1][1] == C ) && (b[2][1] == C )
        || (b[0][2] == C ) && (b[1][2] == C ) && (b[2][2] == C )
        || (b[0][0] == C ) && (b[1][1] == C ) && (b[2][2] == C )
        || (b[0][2] == C ) && (b[1][1] == C ) && (b[2][0] == C ))
        return C_WINS; //computer wins 5000
    if ((b[0][0] == H ) && (b[0][1] == H ) && (b[0][2] == H )
        || (b[1][0] == H ) && (b[1][1] == H ) && (b[1][2] == H )
        || (b[2][0] == H ) && (b[2][1] == H ) && (b[2][2] == H )
        || (b[0][0] == H ) && (b[1][0] == H ) && (b[2][0] == H )
        || (b[0][1] == H ) && (b[1][1] == H ) && (b[2][1] == H )
        || (b[0][2] == H ) && (b[1][2] == H ) && (b[2][2] == H )
        || (b[0][0] == H ) && (b[1][1] == H ) && (b[2][2] == H )
        || (b[0][2] == H ) && (b[1][1] == H ) && (b[2][0] == H ))
        return H_WINS; //human wins -5000

    for(int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            if(b[i][j] == EMPTY) return NOT_OVER; //game not over
        }
    }
    return DRAW; //game over - draw 0
}

int evaluate(){ //STUB  //HEURISTIC
    return 0;
}



























