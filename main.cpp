#include <iostream>
using namespace std;

#define RUN_TEST 0

#define EMPTY 0
#define C_WINS 5000
#define H_WINS  -5000
#define DRAW 0
#define GAME_NOT_OVER -1

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

#define DATABASE_DEPTH 200
#define DATABASE_WIDTH 6

struct KillerMove{
    int moveTo[2];
    bool empty;
};
KillerMove killerMove[50];



enum MOVE{
    MOVE_FROM_ROW,
    MOVE_FROM_COL,
    MOVE_TO_ROW,
    MOVE_TO_COL,
    MOVE_FIG_TYPE,
    MOVE_WHOSE_MOVE,
};

enum TURN{
    COM_TURN,
    HUM_TURN,
};
enum GENERAL{
    COM,
    HUM,
    BOTH,
};

int BEST_SCORE = 0;
int NUM_OF_LEAVES = 0;

int NUM_OF_HUM_MOVES = 0;
int NUM_OF_COM_MOVES = 0;
int MAX_MILS_ONE_ITER = 5000;

int COLLAPSE_RECURSION = false;
chrono::high_resolution_clock::time_point lastTime;
int DEPTH_FOR_OUTPUT = 0;
int board[7][7];
char const board_labels[] = {'-', 'T', '~', '*', 'X', 't', '+', '@', 'x'};
int moveFromI[2];
int moveToI[2];
char moveFromExternal[2];
char moveToExternal[2];
int allMoves[DATABASE_DEPTH][DATABASE_WIDTH];
int allCompMoves[DATABASE_DEPTH][DATABASE_WIDTH];
int allHumanMoves[DATABASE_DEPTH][DATABASE_WIDTH];

int HUM_MOVE[4];
int COM_MOVE[4];

int MAXDEPTH = 5;

int ALL_INDEX = 0;
int COM_INDEX = 0;
int HUM_INDEX = 0;

int* makeComMove();
int min(int depth,int previousBest);
int max(int depth, int previousBest);
int evaluate();
int checkForWinner(int);
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
/* BOTH | COM | HUM */
void showAllMoves(int);
bool endOfMoveList(const int *list);
void makeHumMove();
void makeCompMoveOnBoard();

int main() {
    setupBoard();
    printBoard();
    generateAllMoves();
    showAllMoves(BOTH);
    for(;;){
        getAMove();
        makeHumMove();
        checkGameOver();
        printBoard();
        generateAllMoves();
        showAllMoves(BOTH);
        makeCompMoveOnBoard();
        checkGameOver();
    }
#if RUN_TEST
    convertMoveToInternalRep("A1B4");
    convertMoveExternalRep();
    cout << "A1 : " << moveFromI[0] << moveFromI[1] << " : " << moveFromExternal[0] <<
    moveFromExternal[1] << endl;
    cout << "B4 : " << moveToI[0] << moveToI[1] << " : " << moveToExternal[0]
    << moveToExternal[1] << endl;
    cout << (isMoveLegal() ? "The move is legal" : "The move is illegal");
#endif
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
    cout << endl;
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

bool justEnteredMoveLegal(){
    return !(moveFromI[0] == -1 ||
        moveFromI[1] == -1 ||
        moveToI[0] == -1 ||
        moveToI[1] == -1);
}

void convertMoveToInternalRep(char *move){
    if(move[0] == 'A' || move[0] == 'a'){
        moveFromI[1] = 0;
    } else if(move[0] == 'B' || move[0] == 'b'){
        moveFromI[1] = 1;
    } else if(move[0] == 'C' || move[0] == 'c'){
        moveFromI[1] = 2;
    } else if(move[0] == 'D' || move[0] == 'd'){
        moveFromI[1] = 3;
    } else if(move[0] == 'E' || move[0] == 'e'){
        moveFromI[1] = 4;
    } else if(move[0] == 'F' || move[0] == 'f'){
        moveFromI[1] = 5;
    } else if(move[0] == 'G' || move[0] == 'g'){
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

    if(move[2] == 'A' || move[2] == 'a'){
        moveToI[1] = 0;
    } else if(move[2] == 'B' || move[2] == 'b'){
        moveToI[1] = 1;
    } else if(move[2] == 'C' || move[2] == 'c'){
        moveToI[1] = 2;
    } else if(move[2] == 'D' || move[2] == 'd'){
        moveToI[1] = 3;
    } else if(move[2] == 'E' || move[2] == 'e'){
        moveToI[1] = 4;
    } else if(move[2] == 'F' || move[2] == 'f'){
        moveToI[1] = 5;
    } else if(move[2] == 'G' || move[2] == 'g'){
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
           board[move[ROW]][move[COL]] == C_X;
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

bool endOfMoveList(int *list){
    return (list[0] == -1 && list[1] == -1 &&
       list[2] == -1 && list[3] == -1);
}

bool compWins() {
    if (NUM_OF_COM_MOVES == 0)
        return false;
    if (NUM_OF_HUM_MOVES == 0)
        return true;
    if(board[5][3] != H_D)
        return true;
    return false;
}

bool humWins(){
   if(NUM_OF_HUM_MOVES == 0)
       return false;
    if(NUM_OF_COM_MOVES == 0)
        return true;
    if(board[1][3] != C_D)
        return true;
    return false;
}


int evaluateMoveability(){
    return NUM_OF_COM_MOVES - NUM_OF_HUM_MOVES;
}

int findTheNumberOfFigures(int side){
    int num_of_comp_figures = 0;
    int num_of_hum_figures = 0;
    for(int i = 0; i < BRD_LENGTH; i++){
        for(int j = 0; j < BRD_LENGTH; j++){
            if(board[i][j] == H_T || board[i][j] == H_X)
                num_of_hum_figures++;
            if(board[i][j] == C_T || board[i][j] == C_X)
                num_of_comp_figures++;
        }
    }
    if(side == COM){
        return num_of_comp_figures;
    }
    if(side == HUM){
        return num_of_hum_figures;
    }
}


int howFarFromTheOppositeSide(){
    int humAggressiveness = 0;
    for(int i = 0; i < NUM_OF_HUM_MOVES; i++){
        humAggressiveness += allHumanMoves[i][MOVE_FROM_ROW];
    }

    int comAggressiveness = 0;
    for(int i = 0; i < NUM_OF_COM_MOVES; i++){
        comAggressiveness += (BRD_LENGTH - 1 - allCompMoves[i][MOVE_FROM_ROW]);
    }

    return comAggressiveness - humAggressiveness;
}

int evaluate(){ //STUB  //HEURISTIC
    int moveability = 0;
    moveability = evaluateMoveability()/2;
    int aggressiveness = 0;
    aggressiveness = howFarFromTheOppositeSide()/2;
    generateAllMoves();
    return  (findTheNumberOfFigures(COM) - findTheNumberOfFigures(HUM)) * 100
            + (moveability * 10) + (aggressiveness * 30);
}

void showAllMoves(int advToShow){
    if(advToShow == BOTH) {
        for (int i = 0; i < NUM_OF_COM_MOVES; i++) {
            if(i%10 == 0){
                cout << endl << "COM MOVES: ";
            }
            cout << convertMoveExternalRep(allCompMoves[i]);
            cout << "..";

        }
        for (int i = 0; i < NUM_OF_HUM_MOVES; i++) {
            if(i%10 == 0){
                cout << endl << "HUM MOVES: ";
            }
            cout << convertMoveExternalRep(allHumanMoves[i]);
            cout << "..";
        }
    }
    else if(advToShow == COM){
        for (int i = 0; i < NUM_OF_COM_MOVES; i++) {
            if(i%10 == 0){
                cout << endl << "COM MOVES: ";
            }
            cout << convertMoveExternalRep(allCompMoves[i]);
            cout << "..";
        }
    } else if(advToShow == HUM){
        for (int i = 0; i < NUM_OF_HUM_MOVES; i++) {
            if(i%10 == 0){
                cout << endl << "HUM MOVES: ";
            }
            cout << convertMoveExternalRep(allHumanMoves[i]);
            cout << "..";
        }
    }
}

void addToDatabase(int *move) {

    memcpy(&allMoves[ALL_INDEX], move, sizeof(int) * 4);

    if (board[allMoves[ALL_INDEX][MOVE_FROM_ROW]][allMoves[ALL_INDEX][MOVE_FROM_COL]] ==  H_T ) {
        allMoves[ALL_INDEX][MOVE_WHOSE_MOVE] = HUM_TURN;
    }
    if (board[allMoves[ALL_INDEX][MOVE_FROM_ROW]][allMoves[ALL_INDEX][MOVE_FROM_COL]] == H_X) {
        allMoves[ALL_INDEX][MOVE_WHOSE_MOVE] = HUM_TURN;
    }

    if (board[allMoves[ALL_INDEX][MOVE_FROM_ROW]][allMoves[ALL_INDEX][MOVE_FROM_COL]] == H_T) {
        allMoves[ALL_INDEX][MOVE_FIG_TYPE] = H_T;
    }
    if (board[allMoves[ALL_INDEX][MOVE_FROM_ROW]][allMoves[ALL_INDEX][MOVE_FROM_COL]] == H_X) {
        allMoves[ALL_INDEX][MOVE_FIG_TYPE] = H_X;
    }

    if (board[allMoves[ALL_INDEX][MOVE_FROM_ROW]][allMoves[ALL_INDEX][MOVE_FROM_COL]] == C_T) {
        allMoves[ALL_INDEX][MOVE_WHOSE_MOVE] = COM_TURN;
    }

    if (board[allMoves[ALL_INDEX][MOVE_FROM_ROW]][allMoves[ALL_INDEX][MOVE_FROM_COL]] == C_X) {
            allMoves[ALL_INDEX][MOVE_WHOSE_MOVE] = COM_TURN;
    }

    if (board[allMoves[ALL_INDEX][MOVE_FROM_ROW]][allMoves[ALL_INDEX][MOVE_FROM_COL]] == C_T) {
        allMoves[ALL_INDEX][MOVE_FIG_TYPE] = C_T;
    }
    if (board[allMoves[ALL_INDEX][MOVE_FROM_ROW]][allMoves[ALL_INDEX][MOVE_FROM_COL]] == C_X) {
        allMoves[ALL_INDEX][MOVE_FIG_TYPE] = C_X;
    }

    if(allMoves[ALL_INDEX][MOVE_WHOSE_MOVE] == COM_TURN){
        memcpy(&allCompMoves[NUM_OF_COM_MOVES++], move, sizeof(int) * 6);
    }
    if(allMoves[ALL_INDEX][MOVE_WHOSE_MOVE] == HUM_TURN){
        memcpy(&allHumanMoves[NUM_OF_HUM_MOVES++], move, sizeof(int) * 6);
    }
    ALL_INDEX++;
}

/************************************/
/*      Find computer's moves       */
/************************************/

void moveCompTLeft(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move))
        return;
    if(collidesWithBeatableHumFig(move)){
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)) {
        addToDatabase(move);
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
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)) {
        addToDatabase(move);
        move[COL]++; moveCompTRight(move); move[COL]--;
    }
    return; //it's run into unbeatable hum figures
}
void moveCompTUp(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move))
        return;

    if(collidesWithHumDeath(move) || collidesWithBeatableHumFig(move)){
        addToDatabase(move);
        return;
    }
    move[ROW]--; moveCompTUp(move); move[ROW]++;
    return;
}
void moveCompTDown(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithComFigures(move))
        return;
    if(collidesWithBeatableHumFig(move)){
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)) {
        addToDatabase(move);
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
    if(isMoveBeyondBoundaries(move) )
        return;
    if(collidesWithBeatableHumFig(move)) {
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)){
        addToDatabase(move);
        move[ROW]++; move[COL]--; moveCompXDownLeft(move);  move[ROW]--; move[COL]++;
        return;
    }
}

void moveCompXDownRight(int move[]){
    if(isMoveBeyondBoundaries(move) ){
        return;
    }
    if(collidesWithBeatableHumFig(move)){
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)){
        addToDatabase(move);
        move[ROW]++; move[COL]++; moveCompXDownRight(move);  move[ROW]--; move[COL]--;
        return;
    }
}

void moveCompXUpLeft(int move[]){
    if(isMoveBeyondBoundaries(move) ) {
        return;
    }
    if(collidesWithBeatableHumFig(move)) {
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)){
        move[ROW]--; move[COL]--; moveCompXUpLeft(move); move[ROW]++; move[COL]++;
        return;
    }
    if(collidesWithHumDeath(move)){
        addToDatabase(move);
        return;
    }
    return;
}

void moveCompXUpRight(int move[]){
    if(isMoveBeyondBoundaries(move)) {
        return;
    }
    if(collidesWithBeatableHumFig(move)) {
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)){
        move[ROW]--; move[COL]++; moveCompXUpRight(move); move[ROW]++; move[COL]--;
        return;
    }
    if(collidesWithHumDeath(move)){
        addToDatabase(move);
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
    memset(allMoves, -1, sizeof(allMoves[0][0]) * 200 * 4);
    memset(allCompMoves, -1, sizeof(allMoves[0][0]) * 200 * 4);
    memset(allHumanMoves, -1, sizeof(allMoves[0][0]) * 200 * 4);
    NUM_OF_HUM_MOVES = 0;
    NUM_OF_COM_MOVES = 0;
    ALL_INDEX = HUM_INDEX = COM_INDEX = 0;

    int move[4];
    for(int i = 0; i < BRD_LENGTH; i++){
        for(int j = 0; j < BRD_LENGTH; j++){
            if(board[i][j] != C_W &&
               board[i][j] != C_D &&
               board[i][j] != H_W &&
               board[i][j] != H_D){
                    move[0] = i;
                    move[1] = j;
                    move[2] = i;
                    move[3] = j;
                    if(board[i][j] == C_T){
                        findAllMovesForCompT(move);
                    }  if(board[i][j] == C_X) {
                        findAllMovesForCompX(move);
                    }  if(board[i][j] == H_X){
                        findAllMovesForHumX(move);
                    }  if(board[i][j] == H_T){
                        findAllMovesForHumT(move);
                    }
            }
        }
    }
}


void moveHumXUpLeft(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithHumFigures(move))
        return;
    if(collidesWithBeatableComFig(move)) {
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)){
        addToDatabase(move);
        move[ROW]--; move[COL]--; moveHumXUpLeft(move);  move[ROW]++; move[COL]++;
        return;
    }
    return;
}


void moveHumXUpRight(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithHumFigures(move)){
        return;
    }
    if(collidesWithBeatableComFig(move)){
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)){
        addToDatabase(move);
        move[ROW]--; move[COL]++; moveHumXUpRight(move);  move[ROW]++; move[COL]--;
        return;
    }
    return;
}

void moveHumXDownLeft(int move[]){
    if(isMoveBeyondBoundaries(move)) {
        return;
    }
    if(collidesWithBeatableComFig(move)) {
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)){
        move[ROW]++; move[COL]--; moveHumXDownLeft(move); move[ROW]--; move[COL]++;
        return;
    }
    if(collidesWithComDeath(move)){
        addToDatabase(move);
        return;
    }
    return;
}

void moveHumXDownRight(int move[]){
    if(isMoveBeyondBoundaries(move)) {
        return;
    }
    if(collidesWithBeatableComFig(move)) {
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)){
        move[ROW]++; move[COL]++; moveHumXDownRight(move); move[ROW]--; move[COL]--;
        return;
    }
    if(collidesWithComDeath(move)){
        addToDatabase(move);
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
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)) {
        addToDatabase(move);
        move[COL]--; moveHumTLeft(move); move[COL]++;
    }
    return; //it's run into unbeatable hum figures
}
void moveHumTRight(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithHumFigures(move))
        return;
    if(collidesWithBeatableComFig(move)){
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)) {
        addToDatabase(move);
        move[COL]++; moveHumTRight(move); move[COL]--;
    }
    return; //it's run into unbeatable hum figures
}
void moveHumTDown(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithHumFigures(move))
        return;

    if(collidesWithComDeath(move) || collidesWithBeatableComFig(move)){
        addToDatabase(move);
        return;
    }
    move[ROW]--; moveHumTDown(move); move[ROW]++;
    return;
}
void moveHumTUp(int move[]){
    if(isMoveBeyondBoundaries(move) || collidesWithHumFigures(move))
        return;
    if(collidesWithBeatableComFig(move)){
        addToDatabase(move);
        return;
    }
    if(emptySpot(move)) {
        addToDatabase(move);
        move[ROW]--; moveHumTUp(move); move[ROW]++;
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
    char move[5];
    cout << endl << "Enter your move: " ;
    cin >> move[0];
    cin >> move[1];
    cin >> move[2];
    cin >> move[3];
    move[4] = '\0';
    cout << endl;
    convertMoveToInternalRep(move);
//    if(!justEnteredMoveLegal()) {
//        cout << "Entered move is illegal. Please, enter the right move" << endl;
//        getAMove();
//    } else {
//        //TODO: add logic so it goes through all of the moves and checks if it's legal
//    }
    HUM_MOVE[0] = moveFromI[0];
    HUM_MOVE[1] = moveFromI[1];
    HUM_MOVE[2] = moveToI[0];
    HUM_MOVE[3] = moveToI[1];
}

void makeHumMove(){
    int prevMove =  board[HUM_MOVE[MOVE_FROM_ROW]][HUM_MOVE[MOVE_FROM_COL]];
    board[HUM_MOVE[MOVE_FROM_ROW]][HUM_MOVE[MOVE_FROM_COL]] = EMPTY;
    board[HUM_MOVE[MOVE_TO_ROW]][HUM_MOVE[MOVE_TO_COL]] = prevMove;
    generateAllMoves();
}

void checkGameOver(){
    if(checkForWinner(0) == H_WINS) { // -5000
        printBoard();
        cout << "Human win" << endl;
        exit(0);
    }
    if(checkForWinner(0) == C_WINS){  // 5000
        printBoard();
        cout << "Computer win " << endl;
        exit(0);
    }
}

void resetAllMovesAfterChangeOnBoard(){
    generateAllMoves();
}

void makeCompMoveOnBoard(){
    lastTime = std::chrono::high_resolution_clock::now();
    COLLAPSE_RECURSION = false;
    DEPTH_FOR_OUTPUT = 0;
    int numberOfLeaves = 0;
    int i = 2;
    int *bestMove;
    int *lastMove;
    while(true){
        MAXDEPTH = i;
        NUM_OF_LEAVES = 0;
        lastMove = makeComMove();
        if(COLLAPSE_RECURSION == false){
            bestMove = lastMove;
            numberOfLeaves = NUM_OF_LEAVES;
        }else {
            break;
        }
        i++;
    }

    cout << endl << "Opponent's move is: " << convertMoveExternalRep(bestMove) << endl << "Best: " << BEST_SCORE << endl;
    cout << endl << "The algorithm went through " << numberOfLeaves << " calls";
    cout << endl << "The maximum depth was: " << DEPTH_FOR_OUTPUT << endl;

    int prevMove = board[bestMove[MOVE_FROM_ROW]][bestMove[MOVE_FROM_COL]];
    board[bestMove[MOVE_FROM_ROW]][bestMove[MOVE_FROM_COL]] = EMPTY;
    board[bestMove[MOVE_TO_ROW]][bestMove[MOVE_TO_COL]] = prevMove;

    resetAllMovesAfterChangeOnBoard();
    printBoard();
    showAllMoves(BOTH);
}

int* makeComMove(){
    auto currentTime = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() > MAX_MILS_ONE_ITER)
    {
        COLLAPSE_RECURSION = true;
        return new int[0];
    }
    NUM_OF_LEAVES++;
    int best =-20000, depth = 0, score;
    int BEST_MOVE[4], LOCAL_MOVE[4];
    int counter = 0;

    int maxMoves = NUM_OF_COM_MOVES;
    for(;;){
        LOCAL_MOVE[0] = allCompMoves[counter][0];
        LOCAL_MOVE[1] = allCompMoves[counter][1];
        LOCAL_MOVE[2] = allCompMoves[counter][2];
        LOCAL_MOVE[3] = allCompMoves[counter][3];

        int prevFigure = board[LOCAL_MOVE[MOVE_TO_ROW]][LOCAL_MOVE[MOVE_TO_COL]];
        int prevMove = board[LOCAL_MOVE[MOVE_FROM_ROW]][LOCAL_MOVE[MOVE_FROM_COL]];
        board[LOCAL_MOVE[MOVE_FROM_ROW]][LOCAL_MOVE[MOVE_FROM_COL]] = EMPTY;
        board[LOCAL_MOVE[MOVE_TO_ROW]][LOCAL_MOVE[MOVE_TO_COL]] = prevMove;
        resetAllMovesAfterChangeOnBoard();

        score = min(depth+1, best);

        board[LOCAL_MOVE[MOVE_TO_ROW]][LOCAL_MOVE[MOVE_TO_COL]] = prevFigure;
        board[LOCAL_MOVE[MOVE_FROM_ROW]][LOCAL_MOVE[MOVE_FROM_COL]] = prevMove;
        resetAllMovesAfterChangeOnBoard();

        if(score > best){
            BEST_MOVE[0] = allCompMoves[counter][0];
            BEST_MOVE[1] = allCompMoves[counter][1];
            BEST_MOVE[2] = allCompMoves[counter][2];
            BEST_MOVE[3] = allCompMoves[counter][3];
            best = score;
        }
        counter++;
        if(counter >= maxMoves){
            break;
        }
    }
    BEST_SCORE = best;
    int *returnValue = (int*)calloc(4, sizeof(int));
    memcpy(returnValue, BEST_MOVE, sizeof(int) * 4);
    return returnValue;
}

int min(int depth, int previousBest){
    auto currentTime = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() > MAX_MILS_ONE_ITER)
    {
        COLLAPSE_RECURSION = true;
        return false;
    }
    NUM_OF_LEAVES++;
    int best = 20000;
    int score;
    int  LOCAL_HUM_MOVE[4];
    int counter = 0;
    if(checkForWinner(depth) != GAME_NOT_OVER) {
        return checkForWinner(depth);
    }
    if(depth == MAXDEPTH) {
        DEPTH_FOR_OUTPUT = MAXDEPTH;
        return (evaluate());
    }

    int maxMoves = NUM_OF_HUM_MOVES;
    for(;;){
        generateAllMoves();
        LOCAL_HUM_MOVE[0] = allHumanMoves[counter][0];
        LOCAL_HUM_MOVE[1] = allHumanMoves[counter][1];
        LOCAL_HUM_MOVE[2] = allHumanMoves[counter][2];
        LOCAL_HUM_MOVE[3] = allHumanMoves[counter][3];

        int prevFigure =  board[LOCAL_HUM_MOVE[MOVE_TO_ROW]][LOCAL_HUM_MOVE[MOVE_TO_COL]];

        int prevMove = board[LOCAL_HUM_MOVE[MOVE_FROM_ROW]][LOCAL_HUM_MOVE[MOVE_FROM_COL]];
        board[LOCAL_HUM_MOVE[MOVE_FROM_ROW]][LOCAL_HUM_MOVE[MOVE_FROM_COL]] = EMPTY;
        board[LOCAL_HUM_MOVE[MOVE_TO_ROW]][LOCAL_HUM_MOVE[MOVE_TO_COL]] = prevMove;
        resetAllMovesAfterChangeOnBoard();

        score = max(depth+1, best);

        //undo move
        board[LOCAL_HUM_MOVE[MOVE_TO_ROW]][LOCAL_HUM_MOVE[MOVE_TO_COL]] = prevFigure;
        board[LOCAL_HUM_MOVE[MOVE_FROM_ROW]][LOCAL_HUM_MOVE[MOVE_FROM_COL]] = prevMove;
        resetAllMovesAfterChangeOnBoard();

        if(score < best){
            best = score;
        }

        if(previousBest >= best){
            return best;
        }
        counter++;
        if(counter >= maxMoves){
            break;
        }
    }
    return best;
}

int max(int depth, int previousBest){
    auto currentTime = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() > MAX_MILS_ONE_ITER)
    {
        COLLAPSE_RECURSION = true;
        return false;
    }
    NUM_OF_LEAVES++;
    int best = -20000;
    int score;
    int LOCAL_COM_MOVE[4];
    int counter = 0;

    if(checkForWinner(depth) != GAME_NOT_OVER) {
        return checkForWinner(depth);
    }
    if(depth == MAXDEPTH){
        DEPTH_FOR_OUTPUT = MAXDEPTH;
        return (evaluate());
    }

    int maxMoves = NUM_OF_COM_MOVES;

    for(;;){
        LOCAL_COM_MOVE[0] = allCompMoves[counter][0];
        LOCAL_COM_MOVE[1] = allCompMoves[counter][1];
        LOCAL_COM_MOVE[2] = allCompMoves[counter][2];
        LOCAL_COM_MOVE[3] = allCompMoves[counter][3];

        int prevFigure =   board[LOCAL_COM_MOVE[MOVE_TO_ROW]][LOCAL_COM_MOVE[MOVE_TO_COL]];
        int prevMove = board[LOCAL_COM_MOVE[MOVE_FROM_ROW]][LOCAL_COM_MOVE[MOVE_FROM_COL]];
        board[LOCAL_COM_MOVE[MOVE_FROM_ROW]][LOCAL_COM_MOVE[MOVE_FROM_COL]] = EMPTY;
        board[LOCAL_COM_MOVE[MOVE_TO_ROW]][LOCAL_COM_MOVE[MOVE_TO_COL]] = prevMove;
        resetAllMovesAfterChangeOnBoard();

        score = min(depth+1, best);

        //undo move
        board[LOCAL_COM_MOVE[MOVE_TO_ROW]][LOCAL_COM_MOVE[MOVE_TO_COL]] = prevFigure;
        board[LOCAL_COM_MOVE[MOVE_FROM_ROW]][LOCAL_COM_MOVE[MOVE_FROM_COL]] = prevMove;
        resetAllMovesAfterChangeOnBoard();

        if(score >  best){
            best = score;
        }

        if(previousBest <= best){
            return best;
        }

        counter++;
        if(counter >= maxMoves){
            break;
        }
    }
    return best;
}

int checkForWinner(int depth){
    if (compWins()){
        //cout << "\nDepth is : "  << depth << endl;
        return C_WINS - depth; //computer wins 5000
    }

    if (humWins())
        return H_WINS + depth; //human wins -5000

    return GAME_NOT_OVER;
}





























