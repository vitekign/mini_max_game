#include <iostream>
#include <vector>

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

#define INT_ROW 0
#define EXT_ROW 1
#define INT_COL 1
#define EXT_COL 0

#define ROW 2
#define COL 3

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

#define HUM_T_FIGHTER_HOR_MOVE 0
#define COM_T_FIGHTER_HOR_MOVE 0

int board[7][7];
char board_labels[] = {'-', 'T', '~', '*', 'X', 't', '+', '@', 'x'};
int moveFromI[2];
int moveToI[2];
char moveFromExternal[2];
char moveToExternal[2];

int MAXDEPTH = 49;

vector<int> allHumanMoves;
vector<int> allComMoves;


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

bool isMoveLegal();

void generateAllMoves();

void findAllMovesForHumX(int move[]);

void findAllMovesForHumT(int move[]);

int main() {
    setupBoard();
    printBoard();
    generateAllMoves();




#if RUN_TEST
    convertMoveToInternalRep("A1B4");
    convertMoveExternalRep();
    cout << "A1 : " << moveFromI[0] << moveFromI[1] << " : " << moveFromExternal[0] <<
    moveFromExternal[1] << endl;
    cout << "B4 : " << moveToI[0] << moveToI[1] << " : " << moveToExternal[0]
    << moveToExternal[1] << endl;
    cout << (isMoveLegal() ? "The move is legal" : "The move is illegal");
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
            cout << board_labels[board[i][j]] << " ";
            labelAdversariesBoards(i, j);
        }
        cout << endl;
    }
    decorateColumns();
}

void convertMoveToInternalRep(char *move){
    if(move[0] == 'A'){
        moveFromI[1] = 0;
    } else if(move[0] == 'B'){
        moveFromI[1] = 1;
    } else if(move[0] == 'C'){
        moveFromI[1] = 2;
    } else if(move[0] == 'D'){
        moveFromI[1] = 3;
    } else if(move[0] == 'E'){
        moveFromI[1] = 4;
    } else if(move[0] == 'F'){
        moveFromI[1] = 5;
    } else if(move[0] == 'G'){
        moveFromI[1] = 6;
    } else {
        moveFromI[1] = -1;
    }

    if(move[1] == '1'){
        moveFromI[0] = 6;
    } else if(move[1] == '2'){
        moveFromI[0] = 5;
    } else if(move[1] == '3'){
        moveFromI[0] = 4;
    } else if(move[1] == '4'){
        moveFromI[0] = 3;
    } else if(move[1] == '5'){
        moveFromI[0] = 2;
    } else if(move[1] == '6'){
        moveFromI[0] = 1;
    } else if(move[1] == '7'){
        moveFromI[0] = 0;
    } else {
        moveFromI[0] = -1;
    }

    if(move[2] == 'A'){
        moveToI[1] = 0;
    } else if(move[2] == 'B'){
        moveToI[1] = 1;
    } else if(move[2] == 'C'){
        moveToI[1] = 2;
    } else if(move[2] == 'D'){
        moveToI[1] = 3;
    } else if(move[2] == 'E'){
        moveToI[1] = 4;
    } else if(move[2] == 'F'){
        moveToI[1] = 5;
    } else if(move[2] == 'G'){
        moveToI[1] = 6;
    } else {
        moveToI[1] = -1;
    }

    if(move[3] == '1'){
        moveToI[0] = 6;
    } else if(move[3] == '2'){
        moveToI[0] = 5;
    } else if(move[3] == '3'){
        moveToI[0] = 4;
    } else if(move[3] == '4'){
        moveToI[0] = 3;
    } else if(move[3] == '5'){
        moveToI[0] = 2;
    } else if(move[3] == '6'){
        moveToI[0] = 1;
    } else if(move[3] == '7'){
        moveToI[0] = 0;
    } else {
        moveToI[0] = -1;
    }
}

char* convertMoveExternalRep(int *move){
    char *returnValue = new char[5];
    for(int i = 0; i < 2; i++){
        int  *convFrom;
        char *convTo;
        if(i == 0){
            convFrom = move;
            convTo = returnValue;
        } else {
            convFrom = move+2;
            convTo = returnValue+2;
        }
        if(convFrom[INT_ROW] == 0){
            convTo[EXT_ROW] = '7';
        } else if(convFrom[INT_ROW] == 1){
            convTo[EXT_ROW] = '6';
        } else if(convFrom[INT_ROW] == 2){
            convTo[EXT_ROW] = '5';
        } else if(convFrom[INT_ROW] == 3){
            convTo[EXT_ROW] = '4';
        } else if(convFrom[INT_ROW] == 4){
            convTo[EXT_ROW] = '3';
        } else if(convFrom[INT_ROW] == 5){
            convTo[EXT_ROW] = '2';
        } else if(convFrom[INT_ROW] == 6){
            convTo[EXT_ROW] = '1';
        }


        if(convFrom[INT_COL] == 0){
            convTo[EXT_COL] = 'A';
        } else if(convFrom[INT_COL] == 1){
            convTo[EXT_COL] = 'B';
        } else if(convFrom[INT_COL] == 2){
            convTo[EXT_COL] = 'C';
        } else if(convFrom[INT_COL] == 3){
            convTo[EXT_COL] = 'D';
        } else if(convFrom[INT_COL] == 4){
            convTo[EXT_COL] = 'E';
        } else if(convFrom[INT_COL] == 5){
            convTo[EXT_COL] = 'F';
        } else if(convFrom[INT_COL] == 6){
            convTo[EXT_COL] = 'G';
        }
    }
    returnValue[4] ='\0';
    return returnValue;
}

void convertMoveExternalRep(){
    for(int i = 0; i < 2; i++){
        int const *convFrom;
        char *convTo;
        if(i == 0){
            convFrom = moveFromI;
            convTo = moveFromExternal;
        } else {
            convFrom = moveToI;
            convTo = moveToExternal;
        }
        if(convFrom[INT_ROW] == 0){
            convTo[EXT_ROW] = '7';
        } else if(convFrom[INT_ROW] == 1){
            convTo[EXT_ROW] = '6';
        } else if(convFrom[INT_ROW] == 2){
            convTo[EXT_ROW] = '5';
        } else if(convFrom[INT_ROW] == 3){
            convTo[EXT_ROW] = '4';
        } else if(convFrom[INT_ROW] == 4){
            convTo[EXT_ROW] = '3';
        } else if(convFrom[INT_ROW] == 5){
            convTo[EXT_ROW] = '2';
        } else if(convFrom[INT_ROW] == 6){
            convTo[EXT_ROW] = '1';
        }


        if(convFrom[INT_COL] == 0){
            convTo[EXT_COL] = 'A';
        } else if(convFrom[INT_COL] == 1){
            convTo[EXT_COL] = 'B';
        } else if(convFrom[INT_COL] == 2){
            convTo[EXT_COL] = 'C';
        } else if(convFrom[INT_COL] == 3){
            convTo[EXT_COL] = 'D';
        } else if(convFrom[INT_COL] == 4){
            convTo[EXT_COL] = 'E';
        } else if(convFrom[INT_COL] == 5){
            convTo[EXT_COL] = 'F';
        } else if(convFrom[INT_COL] == 6){
            convTo[EXT_COL] = 'G';
        }
    }
}

bool isMoveBeyondBoundaries(int move[]) {
    return move[ROW] > (BRD_LENGTH - 1) ||
           move[ROW] < 0 ||
           move[COL] > (BRD_LENGTH - 1) ||
           move[COL] < 0;
}

bool isMoveBeyondBoundaries() {
    return moveFromI[INT_ROW] > (BRD_LENGTH - 1) ||
           moveFromI[INT_ROW] < 0 ||
           moveFromI[INT_COL] > (BRD_LENGTH - 1) ||
           moveFromI[INT_COL] < 0 ||
           moveToI[INT_ROW] > (BRD_LENGTH - 1) ||
           moveToI[INT_ROW] < 0 ||
           moveToI[INT_COL] > (BRD_LENGTH - 1) ||
           moveToI[INT_COL] < 0;
}

bool isMoveLegal(){
    !isMoveBeyondBoundaries();

}

bool collidesWithComFigures(int move[]){
    return board[move[ROW]][move[COL]] == C_T ||
           board[move[ROW]][move[COL]] == C_W ||
           board[move[ROW]][move[COL]] == C_X ||
           board[move[ROW]][move[COL]] == C_D ;
}

bool collidesWithHumFigures(int move[]){
    return board[move[ROW]][move[COL]] == H_T ||
           board[move[ROW]][move[COL]] == H_W ||
           board[move[ROW]][move[COL]] == H_X ||
           board[move[ROW]][move[COL]] == H_D ;
}

bool collidesWithBeatableHumFig(int move[]){
    return board[move[ROW]][move[COL]] == H_T ||
           board[move[ROW]][move[COL]] == H_X;
}

bool collidesWithBeatableComFig(int *move){
    return board[move[ROW]][move[COL]] == C_T ||
           board[move[ROW]][move[COL]] == H_X;
}

bool collidesWithHumDeath(int move[]){
    return board[move[ROW]][move[COL]] == H_D;
}

bool collidesWithComDeath(int *move){
    return board[move[ROW]][move[COL]] == C_D;
}

bool emptySpot(int move[]){
    return board[move[ROW]][move[COL]] == EMPTY;
}

bool lastHumMoveWasTHor(void){
    return HUM_T_FIGHTER_HOR_MOVE;
}

bool lastComMoveWasTHor(void){
    return COM_T_FIGHTER_HOR_MOVE;
}


void addToTheDatabase(int *move){

   // cout << "[" << move[0] << move[1] <<"]:[" << move[2] << move[3] <<"]" << " ";
    cout << convertMoveExternalRep(move) << "..";
}

/*      Find computer's moves       */

void moveCompTLeft(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move))
        return;
    if(collidesWithBeatableHumFig(move)){
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)) {
        addToTheDatabase(move);
        move[COL]--;
        moveCompTLeft(move);
        move[COL]++;
    }
    return; //it's run into unbeatable hum figures
}
void moveCompTRight(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move))
        return;
    if(collidesWithBeatableHumFig(move)){
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)) {
        addToTheDatabase(move);
        move[COL]++; moveCompTRight(move); move[COL]--;
    }
    return; //it's run into unbeatable hum figures
}
void moveCompTUp(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move))
        return;

    if(collidesWithHumDeath(move) || collidesWithBeatableHumFig(move)){
        addToTheDatabase(move);
        return;
    }
    move[ROW]--; moveCompTUp(move); move[ROW]++;
    return;
}
void moveCompTDown(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move))
        return;
    if(collidesWithBeatableHumFig(move)){
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)) {
        addToTheDatabase(move);
        move[ROW]++; moveCompTDown(move); move[ROW]--;
    }
    return; //return if it's run into a DEATH STAR
}

void findAllMovesForCompT(int move[]){
    if(!lastComMoveWasTHor()) {
        move[COL]--; moveCompTLeft(move); move[COL]++;
        move[COL]++; moveCompTRight(move); move[COL]--;
    }
    move[ROW]--; moveCompTUp(move); move[ROW]++;
    move[ROW]++; moveCompTDown(move); move[ROW]--;
}

void moveCompXDownLeft(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move))
        return;
    if(collidesWithBeatableHumFig(move)) {
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)){
        addToTheDatabase(move);
        move[ROW]++; move[COL]--; moveCompXDownLeft(move);  move[ROW]--; move[COL]++;
        return;
    }
}

void moveCompXDownRight(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move)){
        return;
    }
    if(collidesWithBeatableHumFig(move)){
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)){
        addToTheDatabase(move);
        move[ROW]++; move[COL]++; moveCompXDownRight(move);  move[ROW]--; move[COL]--;
        return;
    }
}

void moveCompXUpLeft(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move)) {
        return;
    }
    if(collidesWithBeatableHumFig(move)) {
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)){
        move[ROW]--; move[COL]--; moveCompXUpLeft(move); move[ROW]++; move[COL]++;
        return;
    }
    if(collidesWithHumDeath(move)){
       addToTheDatabase(move);
        return;
    }
    return;
}

void moveCompXUpRight(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move)) {
        return;
    }
    if(collidesWithBeatableHumFig(move)) {
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)){
        move[ROW]--; move[COL]++; moveCompXUpRight(move); move[ROW]++; move[COL]--;
        return;
    }
    if(collidesWithHumDeath(move)){
        addToTheDatabase(move);
        return;
    }
    return;
}

void findAllMovesForCompX(int move[]){
    move[COL]--; move[ROW]++; moveCompXDownLeft(move);  move[COL]++; move[ROW]--;
    move[COL]++; move[ROW]++; moveCompXDownRight(move);  move[COL]--; move[ROW]--;
    move[COL]--; move[ROW]--; moveCompXUpLeft(move);  move[COL]++; move[ROW]++;
    move[COL]++; move[ROW]--; moveCompXUpRight(move);  move[COL]--; move[ROW]++;
}

void generateAllMoves(){
    cout << "Start to generate all moves..." << endl;
    int move[4];
    for(int i = 0; i < BRD_LENGTH; i++){
        (i == 0 || i == 2 || i == 4 || i == 5) ? (cout << endl) : (cout << "");
        for(int j = 0; j < BRD_LENGTH; j++){
            if(board[i][j] != C_W &&
               board[i][j] != C_D &&
               board[i][j] != H_W &&
               board[i][j] != H_D){
                    move[0] = i;
                    move[1] = j;
                    if(board[i][j] == C_T){

                        move[2] = i;
                        move[3] = j;
                        findAllMovesForCompT(move);

                    } else if(board[i][j] == C_X) {
                        move[2] = i;
                        move[3] = j;

                        findAllMovesForCompX(move);
                    } else if(board[i][j] == H_X){
                        move[2] = i;
                        move[3] = j;

                        findAllMovesForHumX(move);
                    } else if(board[i][j] == H_T){
                        move[2] = i;
                        move[3] = j;

                        findAllMovesForHumT(move);
                    }
            }
        }
    }
}

void moveHumXUpLeft(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move))
        return;
    if(collidesWithBeatableComFig(move)) {
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)){
        addToTheDatabase(move);
        move[ROW]--; move[COL]--; moveHumXUpLeft(move);  move[ROW]++; move[COL]++;
        return;
    }
}

void moveHumXUpRight(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move)){
        return;
    }
    if(collidesWithBeatableComFig(move)){
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)){
        addToTheDatabase(move);
        move[ROW]--; move[COL]++; moveHumXUpRight(move);  move[ROW]++; move[COL]--;
        return;
    }
}

void moveHumXDownLeft(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move)) {
        return;
    }
    if(collidesWithBeatableComFig(move)) {
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)){
        move[ROW]++; move[COL]--; moveHumXDownLeft(move); move[ROW]--; move[COL]++;
        return;
    }
    if(collidesWithComDeath(move)){
        addToTheDatabase(move);
        return;
    }
    return;
}

void moveHumXDownRight(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithHumFigures(move)) {
        return;
    }
    if(collidesWithBeatableHumFig(move)) {
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)){
        move[ROW]++; move[COL]++; moveHumXDownRight(move); move[ROW]--; move[COL]--;
        return;
    }
    if(collidesWithComDeath(move)){
        addToTheDatabase(move);
        return;
    }
    return;
}

void findAllMovesForHumX(int move[]){
    move[ROW]++; move[COL]--;  moveHumXDownLeft(move);  move[ROW]--; move[COL]++;
    move[ROW]++; move[COL]++;  moveHumXDownRight(move); move[ROW]--; move[COL]--;
    move[ROW]--; move[COL]--;  moveHumXUpLeft(move);  move[ROW]++; move[COL]++;
    move[ROW]--; move[COL]++;  moveHumXUpRight(move);  move[ROW]++; move[COL]--;


}

void moveHumTLeft(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithHumFigures(move))
        return;
    if(collidesWithBeatableComFig(move)){
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)) {
        addToTheDatabase(move);
        move[COL]--; moveHumTLeft(move); move[COL]++;
    }
    return; //it's run into unbeatable hum figures
}
void moveHumTRight(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithHumFigures(move))
        return;
    if(collidesWithBeatableComFig(move)){
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)) {
        addToTheDatabase(move);
        move[COL]++; moveHumTRight(move); move[COL]--;
    }
    return; //it's run into unbeatable hum figures
}
void moveHumTDown(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithHumFigures(move))
        return;

    if(collidesWithComDeath(move) || collidesWithBeatableComFig(move)){
        addToTheDatabase(move);
        return;
    }
    move[ROW]--; moveHumTDown(move); move[ROW]++;
    return;
}
void moveHumTUp(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithHumFigures(move))
        return;
    if(collidesWithBeatableComFig(move)){
        addToTheDatabase(move);
        return;
    }
    if(emptySpot(move)) {
        addToTheDatabase(move);
        move[ROW]++; moveHumTUp(move); move[ROW]--;
    }
    return; //return if it's run into a DEATH STAR
}

void findAllMovesForHumT(int move[]){
    if(!lastHumMoveWasTHor()) {
        move[COL]--; moveHumTLeft(move); move[COL]++;
        move[COL]++; moveHumTRight(move); move[COL]--;
    }
    move[ROW]--; moveHumTUp(move); move[ROW]++;
    move[ROW]++; moveHumTDown(move); move[ROW]--;
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



























