#include <iostream>

using namespace std;


#define H 2
#define C 1
#define EMPTY 0
#define C_WINS 5000
#define H_WINS -5000
#define DRAW 0
#define NOT_OVER -1

/* BOARD */
#define BRD_LENGTH  7
#define C_T 1
#define C_W 2
#define C_D 3
#define C_X 4

#define H_T 5
#define H_W 6
#define H_D 7
#define H_X 8

int board[7][7];
char labels[] = {'-', 'C', '~', '*', 'X', 't', '+', '@', 'x'};

int MAXDEPTH = 49;


void makeMove();
int min(int depth);
int max(int depth);
int evaluate();
int checkForWinner();
void checkGameOver();
void getAMove();
void setupBoard();
void printBoard();

int main() {
    setupBoard();
    printBoard();
//    for(;;){
////        getAMove();
////        checkGameOver();
////        makeMove();
////        checkGameOver();
//    }
}

void setupBoard(){
    for (int i = 0; i < BRD_LENGTH; i++){
        for (int j = 0; j < BRD_LENGTH; j++){
            /* Computer half */
            if(i == 0 && (j == 1 || j == 2 || j == 4 || j == 5 )){
                board[i][j] = C_T;
            }
            else if(i == 1 && (j == 2 || j == 4)){
                board[i][j] = C_W;
            }
            else if(i == 1 && j == 3){
                board[i][j] = C_D;
            }
            else if(i == 2 && (j == 0 || j == 1 || j == 5 || j == 6)){
                board[i][j] = C_X;
            }

            /* Human Half */
            else if(i == 6 && ( j==1 ||  j == 2 || j == 4 || j == 5 )){
                board[i][j] = H_T;
            }
            else if(i == 5 && (j == 2 || j == 4)){
                board[i][j] = H_W;
            }
            else if(i == 5 && j == 3){
                board[i][j] = H_D;
            }
            else if(i == 4 && (j == 0 || j == 1 || j == 5 || j == 6)){
                board[i][j] = H_X;
            } else
            board[i][j] = EMPTY;
        }
    }
}

void printBoard(){
    cout << endl;
    for(int i = 0; i < BRD_LENGTH; i++){
        for (int j = 0; j < BRD_LENGTH; j++){
           cout << labels[board[i][j]] << " ";
        }
        cout << endl;
    }
}

void getAMove(){
    int i;
    int j;
    cout << "Enter your move: " ;
    cin >> i >> j;
    board[i][j] = H;
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
            if(board[i][j] == EMPTY){
                board[i][j] = C; //make move board
                score = min(depth+1);
                if(score > best){
                    mi = i;
                    mj = j;
                    best = score;
                }
                board[i][j] = EMPTY; //undo move
            }
        }
    }
    cout << "my move is" << mi << " " << mj << endl;
    board[mi][mj] = C;
}

int min(int depth){
    int best = 20000;
    int score;
    if(checkForWinner() != NOT_OVER) return (checkForWinner());
    if(depth == MAXDEPTH) return (evaluate());
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(board[i][j] == EMPTY){
                board[i][j] = H; //make move on board
                score = max(depth+1);
                if(score < best) best = score;
                board[i][j] = EMPTY; // undo move
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
            if(board[i][j] == EMPTY){
                board[i][j] = C; //make move on board
                score = min(depth + 1);
                if(score > best) best = score;
                board[i][j] = EMPTY; //undo move
            }
        }
    }
    return best;
}

int checkForWinner(){
    if ((board[0][0] == C ) && (board[0][1] == C ) && (board[0][2] == C )
        || (board[1][0] == C ) && (board[1][1] == C ) && (board[1][2] == C )
        || (board[2][0] == C ) && (board[2][1] == C ) && (board[2][2] == C )
        || (board[0][0] == C ) && (board[1][0] == C ) && (board[2][0] == C )
        || (board[0][1] == C ) && (board[1][1] == C ) && (board[2][1] == C )
        || (board[0][2] == C ) && (board[1][2] == C ) && (board[2][2] == C )
        || (board[0][0] == C ) && (board[1][1] == C ) && (board[2][2] == C )
        || (board[0][2] == C ) && (board[1][1] == C ) && (board[2][0] == C ))
        return C_WINS; //computer wins 5000
    if ((board[0][0] == H ) && (board[0][1] == H ) && (board[0][2] == H )
        || (board[1][0] == H ) && (board[1][1] == H ) && (board[1][2] == H )
        || (board[2][0] == H ) && (board[2][1] == H ) && (board[2][2] == H )
        || (board[0][0] == H ) && (board[1][0] == H ) && (board[2][0] == H )
        || (board[0][1] == H ) && (board[1][1] == H ) && (board[2][1] == H )
        || (board[0][2] == H ) && (board[1][2] == H ) && (board[2][2] == H )
        || (board[0][0] == H ) && (board[1][1] == H ) && (board[2][2] == H )
        || (board[0][2] == H ) && (board[1][1] == H ) && (board[2][0] == H ))
        return H_WINS; //human wins -5000

    for(int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            if(board[i][j] == EMPTY) return NOT_OVER; //game not over
        }
    }
    return DRAW; //game over - draw 0
}

int evaluate(){ //STUB  //HEURISTIC
    return 0;
}



























