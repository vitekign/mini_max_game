#include <iostream>

using namespace std;


#define RUN_TEST 0

#define H 2
#define C 1
#define EMPTY 0
#define FULL 1
#define C_WINS 5000
#define H_WINS -5000
#define DRAW 0
#define NOT_OVER -1

#define INT_RAW 0
#define EXT_ROW 1
#define INT_COL 1
#define EXT_COL 0


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
char labels[] = {'-', 'T', '~', '*', 'X', 't', '+', '@', 'x'};
int moveFromInternal[2];
int moveToInternal[2];
char moveFromExternal[2];
char moveToExternal[2];

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

void decorateRows(int i, int j);
void decorateColumns();

void convertMoveToInternalRep(char *move);

void convertMoveExternalRep();

int main() {
    setupBoard();
    printBoard();



#if RUN_TEST
    convertMoveToInternalRep("A1B4");
    convertMoveExternalRep();
    cout << "A1 : " << moveFromInternal[0] << moveFromInternal[1] << " : " << moveFromExternal[0] <<
    moveFromExternal[1]<< endl;
    cout << "B4 : " << moveToInternal[0] << moveToInternal[1] << " : " << moveToExternal[0]
    << moveToExternal[1]<< endl;
#endif
//    for(;;){
//        getAMove();
//        checkGameOver();
//        makeMove();
//        checkGameOver();
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
void decorateRows(int i, int j){
    if(j == 0) {
        cout << (7 - i) << "  ";
    }
}
void decorateColumns(){
    cout << "   A B C D E F G";
    cout << endl << endl;
}

void labelAdversariesBoards(int i, int j){
    if(i == 0 && j == 6){
        cout << "  COMPUTER ";
    } else if(i == 6 && j == 6){
        cout << "  HUMAN";
    }
}

void printBoard(){
    cout << endl;
    for(int i = 0; i < BRD_LENGTH; i++){
        for (int j = 0; j < BRD_LENGTH; j++){
            decorateRows(i,j);
            cout << labels[board[i][j]] << " ";
            labelAdversariesBoards(i, j);
        }
        cout << endl;
    }
    decorateColumns();
}

void convertMoveToInternalRep(char *move){
    if(move[0] == 'A'){
        moveFromInternal[1] = 0;
    } else if(move[0] == 'B'){
        moveFromInternal[1] = 1;
    } else if(move[0] == 'C'){
        moveFromInternal[1] = 2;
    } else if(move[0] == 'D'){
        moveFromInternal[1] = 3;
    } else if(move[0] == 'E'){
        moveFromInternal[1] = 4;
    } else if(move[0] == 'F'){
        moveFromInternal[1] = 5;
    } else if(move[0] == 'G'){
        moveFromInternal[1] = 6;
    }

    if(move[1] == '1'){
        moveFromInternal[0] = 6;
    } else if(move[1] == '2'){
        moveFromInternal[0] = 5;
    } else if(move[1] == '3'){
        moveFromInternal[0] = 4;
    } else if(move[1] == '4'){
        moveFromInternal[0] = 3;
    } else if(move[1] == '5'){
        moveFromInternal[0] = 2;
    } else if(move[1] == '6'){
        moveFromInternal[0] = 1;
    } else if(move[1] == '7'){
        moveFromInternal[0] = 0;
    }

    if(move[2] == 'A'){
        moveToInternal[1] = 0;
    } else if(move[2] == 'B'){
        moveToInternal[1] = 1;
    } else if(move[2] == 'C'){
        moveToInternal[1] = 2;
    } else if(move[2] == 'D'){
        moveToInternal[1] = 3;
    } else if(move[2] == 'E'){
        moveToInternal[1] = 4;
    } else if(move[2] == 'F'){
        moveToInternal[1] = 5;
    } else if(move[2] == 'G'){
        moveToInternal[1] = 6;
    }

    if(move[3] == '1'){
        moveToInternal[0] = 6;
    } else if(move[3] == '2'){
        moveToInternal[0] = 5;
    } else if(move[3] == '3'){
        moveToInternal[0] = 4;
    } else if(move[3] == '4'){
        moveToInternal[0] = 3;
    } else if(move[3] == '5'){
        moveToInternal[0] = 2;
    } else if(move[3] == '6'){
        moveToInternal[0] = 1;
    } else if(move[3] == '7'){
        moveToInternal[0] = 0;
    }
}

void convertMoveExternalRep(){

    if(moveFromInternal[INT_RAW] == 0){
        moveFromExternal[EXT_ROW] = '7';
    } else if(moveFromInternal[INT_RAW] == 1){
        moveFromExternal[EXT_ROW] = '6';
    } else if(moveFromInternal[INT_RAW] == 2){
        moveFromExternal[EXT_ROW] = '5';
    } else if(moveFromInternal[INT_RAW] == 3){
        moveFromExternal[EXT_ROW] = '4';
    } else if(moveFromInternal[INT_RAW] == 4){
        moveFromExternal[EXT_ROW] = '3';
    } else if(moveFromInternal[INT_RAW] == 5){
        moveFromExternal[EXT_ROW] = '2';
    } else if(moveFromInternal[INT_RAW] == 6){
        moveFromExternal[EXT_ROW] = '1';
    }

    if(moveFromInternal[INT_COL] == 0){
        moveFromExternal[EXT_COL] = 'A';
    } else if(moveFromInternal[INT_COL] == 1){
        moveFromExternal[EXT_COL] = 'B';
    } else if(moveFromInternal[INT_COL] == 2){
        moveFromExternal[EXT_COL] = 'C';
    } else if(moveFromInternal[INT_COL] == 3){
        moveFromExternal[EXT_COL] = 'D';
    } else if(moveFromInternal[INT_COL] == 4){
        moveFromExternal[EXT_COL] = 'E';
    } else if(moveFromInternal[INT_COL] == 5){
        moveFromExternal[EXT_COL] = 'F';
    } else if(moveFromInternal[INT_COL] == 6){
        moveFromExternal[EXT_COL] = 'G';
    }

    if(moveToInternal[INT_RAW] == 0){
        moveToExternal[EXT_ROW] = '7';
    } else if(moveFromInternal[INT_RAW] == 1){
        moveToExternal[EXT_ROW] = '6';
    } else if(moveToInternal[INT_RAW] == 2){
        moveToExternal[EXT_ROW] = '5';
    } else if(moveToInternal[INT_RAW] == 3){
        moveToExternal[EXT_ROW] = '4';
    } else if(moveToInternal[INT_RAW] == 4){
        moveToExternal[EXT_ROW] = '3';
    } else if(moveToInternal[INT_RAW] == 5){
        moveToExternal[EXT_ROW] = '2';
    } else if(moveToInternal[INT_RAW] == 6){
        moveFromExternal[EXT_ROW] = '1';
    }

    if(moveToInternal[INT_COL] == 0){
        moveToExternal[EXT_COL] = 'A';
    } else if(moveToInternal[INT_COL] == 1){
        moveToExternal[EXT_COL] = 'B';
    } else if(moveToInternal[INT_COL] == 2){
        moveToExternal[EXT_COL] = 'C';
    } else if(moveToInternal[INT_COL] == 3){
        moveToExternal[EXT_COL] = 'D';
    } else if(moveToInternal[INT_COL] == 4){
        moveToExternal[EXT_COL] = 'E';
    } else if(moveToInternal[INT_COL] == 5){
        moveToExternal[EXT_COL] = 'F';
    } else if(moveToInternal[INT_COL] == 6){
        moveToExternal[EXT_COL] = 'G';
    }
}

bool checkIfMoveLegal(){
    /* 1. If the move goes beyond BRD_LENGTH - 1
     *      IT'S ILLEGAL
     * 2. If the move has negatives
     *      IT'S ILLEGAL
     *
     * a bunch of recursive functions:
     *      up
     *      down
     *      left
     *      right
     *
     *      up-left
     *      up-right
     *      down-left
     *      down-left
     */
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



























