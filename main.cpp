#include <iostream>
#include <chrono>
#include <string.h>
#include <stdlib.h>
#include <regex>

// g++ -std=c++0x -O3 -march=native -o trench_hero run_game.cpp

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCSimplifyInspection"
using namespace std;

#define RUN_TEST 0
#define RUN_KILLER_HEURISTIC 0
#define RUN_ALPHA_BETA_OPTIMIZATION 1
#define RUN_WITH_MOVE_VALIDATION 1
#define RUN_WITH_ADDITIONAL_OUTPUT 1
#define RUN_WITH_SHOWING_MOVES 1

#define EMPTY  0
#define C_WINS 5000
#define H_WINS -5000
#define GAME_NOT_OVER -1

#define INT_ROW 0
#define EXT_ROW 1
#define INT_COL 1
#define EXT_COL 0

#define ROW 2
#define COL 3

#define BRD_LENGTH  7
#define C_T 1
#define C_W 2
#define C_D 3
#define C_X 4

#define H_T 5
#define H_W 6
#define H_D 7
#define H_X 8

#define DATABASE_DEPTH 200
#define DATABASE_WIDTH 7

struct KillerMove{
    int moveTo[2][2];
    bool empty[2];
};
KillerMove killerMove[50];

enum  move{
    MOVE_FROM_ROW,
    MOVE_FROM_COL,

    MOVE_TO_ROW,
    MOVE_TO_COL,

    MOVE_PIECE_TYPE,
    MOVE_WHOSE_MOVE,
    VER_HOR,
};

#define HORIZONTAL -5
#define VERTICAL-10

enum TURN{
    COM_TURN,
    HUM_TURN,
};
enum GENERAL{
    COM,
    HUM,
    BOTH,
};

struct payload{
    int BEST_SCORE = 0;
    int NUM_OF_NODES = 0;

    int NUM_OF_HUM_MOVES = 0;
    int NUM_OF_COM_MOVES = 0;
    int MAX_MILS_ONE_ITER = 5000;

    int COLLAPSE_RECURSION = false;
    chrono::high_resolution_clock::time_point last_time;
    int DEPTH_FOR_OUTPUT = 0;
    int board[7][7];
    char const board_labels[9] = {'-', 'T', '~', '*', 'X', 't', '+', '@', 'x'};
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

    int move[7];

    bool HUM_T_FIGHTER_HOR_MOVE = false;
    bool COM_T_FIGHTER_HOR_MOVE = false;
};

int min(int depth, int previousBest, struct payload *payload);

int max(int depth, int previousBest, struct payload *payload);

int findKillerMoveInAllCompMoves(KillerMove *killerMove);

int* makeComMove(struct payload *payload);

int checkForWinner(int depth, struct payload *payload);

void setupBoard(struct payload *payload);

void printBoard(struct payload *payload);

void generateAllMoves(struct payload *payload);

void showAllMoves(int adversary_to_show, struct payload *payload);

void getAMove(struct payload *payload);

void makeHumMove(struct payload *payload);

void checkGameOver(struct payload *payload);

void makeCompMoveOnBoard(struct payload *payload);

void findAllMovesForHumX(struct payload *payload);

void findAllMovesForHumT(struct payload *payload);

bool humanMovesFirst(){
    int tempInputBuffer = 0;
    cout << "Please, specify who's making the firstpayload->move:\n";
    cout << "1 - Computer\n";
    cout << "2 - Human\n";
    cin >> tempInputBuffer;
    if((tempInputBuffer != 1) && (tempInputBuffer != 2)){
        cout << "Please, specify the right choice\n";
        humanMovesFirst();
    }
    if(tempInputBuffer == 2)
        return true;
    return false;
}

int main() {
    struct payload *one_thread_payload = new struct payload;
    bool humansMoveFirst = humanMovesFirst();
    setupBoard(one_thread_payload);
    printBoard(one_thread_payload);
    generateAllMoves(one_thread_payload);
    showAllMoves(BOTH, one_thread_payload);
    for(;;){
        if(humansMoveFirst) {
            getAMove(one_thread_payload);
            makeHumMove(one_thread_payload);
            checkGameOver(one_thread_payload);
            printBoard(one_thread_payload);
            generateAllMoves(one_thread_payload);
            showAllMoves(BOTH, one_thread_payload);
            makeCompMoveOnBoard(one_thread_payload);
            checkGameOver(one_thread_payload);
        } else {
            generateAllMoves(one_thread_payload);
            makeCompMoveOnBoard(one_thread_payload);
            checkGameOver(one_thread_payload);
            getAMove(one_thread_payload);
            makeHumMove(one_thread_payload);
            checkGameOver(one_thread_payload);
            printBoard(one_thread_payload);
        }
    }
#if RUN_TEST
    convertMoveToInternalRep("A1B4");
    convertMoveExternalRep();
    cout << "A1 : " << moveFromI[0] << moveFromI[1] << " : " << moveFromExternal[0] <<
    moveFromExternal[1] << endl;
    cout << "B4 : " << moveToI[0] << moveToI[1] << " : " << moveToExternal[0]
    << moveToExternal[1] << endl;
    cout << (isMoveLegal() ? "Thepayload->move is legal" : "Thepayload->move is illegal");
#endif
}

void setupBoard(struct payload *payload){
    for (int i = 0; i < BRD_LENGTH; i++){
        for (int j = 0; j < BRD_LENGTH; j++){
            /* Computer half */
            if(i == 0 && (j == 1 || j == 2 || j == 4 || j == 5 )){
                payload->board[i][j] = C_T;
            }
            else if(i == 1 && (j == 2 || j == 4)){
                payload->board[i][j] = C_W;
            }
            else if(i == 1 && j == 3){
                payload->board[i][j] = C_D;
            }
            else if(i == 2 && (j == 0 || j == 1 || j == 5 || j == 6)){
                payload->board[i][j] = C_X;
            }

            /* Human Half */
            else if(i == 6 && ( j==1 ||  j == 2 || j == 4 || j == 5 )){
                payload->board[i][j] = H_T;
            }
            else if(i == 5 && (j == 2 || j == 4)){
                payload->board[i][j] = H_W;
            }
            else if(i == 5 && j == 3){
                payload->board[i][j] = H_D;
            }
            else if(i == 4 && (j == 0 || j == 1 || j == 5 || j == 6)){
                payload->board[i][j] = H_X;
            } else
            payload->board[i][j] = EMPTY;
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

void printBoard(struct payload *payload){
    cout << endl;
    for(int i = 0; i < BRD_LENGTH; i++){
        for (int j = 0; j < BRD_LENGTH; j++){
            decorateRows(i,j);

            cout << payload->board_labels[payload->board[i][j]] << " ";
            labelAdversariesBoards(i, j);
        }
        cout << endl;
    }
    decorateColumns();
}

bool moveLegal(struct payload *payload){
    if (payload->moveFromI[0] == -1 ||
        payload->moveFromI[1] == -1 ||
        payload->moveToI[0] == -1 ||
        payload->moveToI[1] == -1)
        return false;

    for(int i = 0; i < payload->NUM_OF_HUM_MOVES; i++){
       if (payload->moveFromI[0] == payload->allHumanMoves[i][MOVE_FROM_ROW] &&
           payload->moveFromI[1] == payload->allHumanMoves[i][MOVE_FROM_COL] &&
           payload->moveToI[0] == payload->allHumanMoves[i][MOVE_TO_ROW] &&
           payload->moveToI[1] == payload->allHumanMoves[i][MOVE_TO_COL])
                return true;
    }
    return false;
}

/*
        7   -  T  T  -  T  -  T
        6   X  -  ~  *  ~  -  -
        5   X  -  -  -  -  X  X
        4   -  -  x  -  -  -  -
        3   x  -  -  -  -  x  x
        2   -  -  +  @  +  -  -
        1   t  -  t  -  t  t  -
            A  B  C  D  E  F  G
 */

void convertMoveToInternalRep(struct payload *payload, char *move){
    regex letters("[A-Ga-g]{1}");
    char placeholder[2];
    placeholder[0] = move[0];
    placeholder[1] = '\0';

    bool found = regex_match(placeholder, letters);
    if(found){
        payload->moveFromI[1] = tolower(move[0]) - 97;
    } else {
        payload->moveFromI[1] = -1;
    }

    regex digits("[1-7]{1}");
    placeholder[0] = move[1];
    found = regex_match(placeholder, digits);
    if(found){
        payload->moveFromI[0] = 55 - int(move[1]);
    } else {
        payload->moveFromI[0] = -1;
    }

    placeholder[0] = move[2];
    found = regex_match(placeholder, letters);
    if(found){
        payload->moveToI[1] = tolower(move[2]) - 97;
    } else {
        payload->moveToI[1] = -1;
    }

    placeholder[0] = move[3];
    found = regex_match(placeholder, digits);
    if(found){
        payload->moveToI[0] = 55 - int(move[3]);
    } else {
        payload->moveToI[0] = -1;
    }
}

char* convertMoveToExternalRep(int *move){
    char *returnValue = new char[5];
    for(int i = 0; i < 2; i++){
        int  *convFrom;
        char *convTo;
        if(i == 0){
            convFrom =move;
            convTo = returnValue;
        } else {
            convFrom =move+2;
            convTo = returnValue+2;
        }

        if(convFrom[INT_ROW] >= 0 && convFrom[INT_ROW] <= 6){
            convTo[EXT_ROW] = char(7 - convFrom[INT_ROW] + int('0'));
        }

        if(convFrom[INT_COL] >= 0 && convFrom[INT_COL] <= 6) {
            convTo[EXT_COL] = char(int('A') + convFrom[INT_COL]);
        }
    }
    returnValue[4] ='\0';
    return returnValue;
}

void convertMoveExternalRep(struct payload *payload){
    for(int i = 0; i < 2; i++){
        int const *convFrom;
        char *convTo;
        if(i == 0){
            convFrom = payload->moveFromI;
            convTo = payload->moveFromExternal;
        } else {
            convFrom = payload->moveToI;
            convTo = payload->moveToExternal;
        }

        if(convFrom[INT_ROW] >= 0 && convFrom[INT_ROW] <= 6){
            convTo[EXT_ROW] = char(7 - convFrom[INT_ROW] + int('0'));
        }

        if(convFrom[INT_COL] >= 0 && convFrom[INT_COL] <= 6) {
            convTo[EXT_COL] = char(int('A') + convFrom[INT_COL]);
        }
    }
}

bool isMoveBeyondBoundaries(int move[]) {
    return move[ROW] > (BRD_LENGTH - 1) ||
           move[ROW] < 0 ||
           move[COL] > (BRD_LENGTH - 1) ||
           move[COL] < 0;
}

bool isMoveBeyondBoundaries(struct payload *payload) {
    return payload->moveFromI[INT_ROW] > (BRD_LENGTH - 1) ||
           payload->moveFromI[INT_ROW] < 0 ||
           payload->moveFromI[INT_COL] > (BRD_LENGTH - 1) ||
           payload->moveFromI[INT_COL] < 0 ||
           payload->moveToI[INT_ROW] > (BRD_LENGTH - 1) ||
           payload->moveToI[INT_ROW] < 0 ||
           payload->moveToI[INT_COL] > (BRD_LENGTH - 1) ||
           payload->moveToI[INT_COL] < 0;
}

bool isMoveLegal(struct payload *payload){
    return !isMoveBeyondBoundaries(payload);
}

bool collidesWithComFigures(struct payload *payload){
    return payload->board[payload->move[ROW]][payload->move[COL]] == C_T ||
           payload->board[payload->move[ROW]][payload->move[COL]] == C_W ||
           payload->board[payload->move[ROW]][payload->move[COL]] == C_X ||
           payload->board[payload->move[ROW]][payload->move[COL]] == C_D ;
}

bool collidesWithHumFigures(struct payload *payload){
    return payload->board[payload->move[ROW]][payload->move[COL]] == H_T ||
           payload->board[payload->move[ROW]][payload->move[COL]] == H_W ||
           payload->board[payload->move[ROW]][payload->move[COL]] == H_X ||
           payload->board[payload->move[ROW]][payload->move[COL]] == H_D ;
}

bool collidesWithBeatableHumFig(struct payload *payload){
    return payload->board[payload->move[ROW]][payload->move[COL]] == H_T ||
           payload->board[payload->move[ROW]][payload->move[COL]] == H_X;
}

bool collidesWithBeatableComFig(struct payload *payload){
    return payload->board[payload->move[ROW]][payload->move[COL]] == C_T ||
           payload->board[payload->move[ROW]][payload->move[COL]] == C_X;
}

bool collidesWithHumDeath(struct payload *payload){
    return payload->board[payload->move[ROW]][payload->move[COL]] == H_D;
}

bool collidesWithComDeath(struct payload *payload){
    return payload->board[payload->move[ROW]][payload->move[COL]] == C_D;
}

bool emptySpot(struct payload *payload){
    return payload->board[payload->move[ROW]][payload->move[COL]] == EMPTY;
}


bool endOfMoveList(int *list){
    return (list[0] == -1 && list[1] == -1 &&
       list[2] == -1 && list[3] == -1);
}

bool compWins(struct payload *payload) {
    if (payload->NUM_OF_COM_MOVES == 0)
        return false;
    if (payload->NUM_OF_HUM_MOVES == 0)
        return true;
    if(payload->board[5][3] != H_D)
        return true;
    return false;
}

bool humWins(struct payload *payload){
   if(payload->NUM_OF_HUM_MOVES == 0)
       return false;
    if(payload->NUM_OF_COM_MOVES == 0)
        return true;
    if(payload->board[1][3] != C_D)
        return true;
    return false;
}


int evaluateMoveability(struct payload *payload){
    return payload->NUM_OF_COM_MOVES - payload->NUM_OF_HUM_MOVES;
}

int findTheNumberOfPieces(int side, struct payload *payload){
    int num_of_comp_figures = 0;
    int num_of_hum_figures = 0;
    for(int i = 0; i < BRD_LENGTH; i++){
        for(int j = 0; j < BRD_LENGTH; j++){
            if(payload->board[i][j] == H_T || payload->board[i][j] == H_X)
                num_of_hum_figures++;
            if(payload->board[i][j] == C_T || payload->board[i][j] == C_X)
                num_of_comp_figures++;
        }
    }
    if(side == COM)
        return num_of_comp_figures;
    if(side == HUM)
        return num_of_hum_figures;
}


int howFarFromTheOppositeSide(struct payload *payload){
    int humAggressiveness = 0;
    for(int i = 0; i < payload->NUM_OF_HUM_MOVES; i++){
        humAggressiveness += payload->allHumanMoves[i][MOVE_FROM_ROW];
    }

    int comAggressiveness = 0;
    for(int i = 0; i < payload->NUM_OF_COM_MOVES; i++){
        comAggressiveness += (BRD_LENGTH - 1 - payload->allCompMoves[i][MOVE_FROM_ROW]);
    }

    return comAggressiveness - humAggressiveness;
}

int evaluate(struct payload *payload){
    int moveability = 0;
    moveability = evaluateMoveability(payload);
    int aggressiveness = 0;
    aggressiveness = howFarFromTheOppositeSide(payload);

    int points_for_capturing_death_star = 0;
    if(payload->board[5][3] != H_D)
        points_for_capturing_death_star = 700;

    if(payload->board[1][3] != C_D)
        points_for_capturing_death_star = -700;

    return  (findTheNumberOfPieces(COM, payload) - findTheNumberOfPieces(HUM, payload))*100 + moveability*1 +
            int(aggressiveness * 1.5) + points_for_capturing_death_star;

//    generateAllMoves();
//    if(side == COM)
//    return  (findTheNumberOfFigures(COM) - findTheNumberOfPieces(HUM)) * 100
//            + (moveability * 2) + (aggressiveness * 5);
//    if(side == HUM)
//    return  (findTheNumberOfFigures(HUM) - findTheNumberOfPieces(COM)) * 100
//                           + (moveability * 2) + (aggressiveness * 5);
}

void showAllMoves(int adversary_to_show, struct payload *payload){
#if RUN_WITH_SHOWING_MOVES
    if(adversary_to_show == BOTH) {
        for (int i = 0; i < payload->NUM_OF_COM_MOVES; i++) {
            if(i%10 == 0){
                cout << endl << "COM MOVES: ";
            }
            cout << convertMoveToExternalRep(payload->allCompMoves[i]);
            cout << "..";

        }
        for (int i = 0; i < payload->NUM_OF_HUM_MOVES; i++) {
            if(i%10 == 0){
                cout << endl << "HUM MOVES: ";
            }
            cout << convertMoveToExternalRep(payload->allHumanMoves[i]);
            cout << "..";
        }
    }
    else if(adversary_to_show == COM){
        for (int i = 0; i < payload->NUM_OF_COM_MOVES; i++) {
            if(i%10 == 0){
                cout << endl << "COM MOVES: ";
            }
            cout << convertMoveToExternalRep(payload->allCompMoves[i]);
            cout << "..";
        }
    } else if(adversary_to_show == HUM){
        for (int i = 0; i < payload->NUM_OF_HUM_MOVES; i++) {
            if(i%10 == 0){
                cout << endl << "HUM MOVES: ";
            }
            cout << convertMoveToExternalRep(payload->allHumanMoves[i]);
            cout << "..";
        }
    }
#endif
}

void addToDatabase(struct payload *payload) {
    // Copy current payload->move into all-moves data-set
    //     dest                  src   count
    memcpy(&payload->allMoves[payload->ALL_INDEX], payload->move, sizeof(int) * 7);

    if (payload->board[payload->allMoves[payload->ALL_INDEX][MOVE_FROM_ROW]][payload->allMoves[payload->ALL_INDEX][MOVE_FROM_COL]] ==  H_T ) {
        payload->allMoves[payload->ALL_INDEX][MOVE_WHOSE_MOVE] = HUM_TURN;
    }
    if (payload->board[payload->allMoves[payload->ALL_INDEX][MOVE_FROM_ROW]][payload->allMoves[payload->ALL_INDEX][MOVE_FROM_COL]] == H_X) {
        payload->allMoves[payload->ALL_INDEX][MOVE_WHOSE_MOVE] = HUM_TURN;
    }

    if (payload->board[payload->allMoves[payload->ALL_INDEX][MOVE_FROM_ROW]][payload->allMoves[payload->ALL_INDEX][MOVE_FROM_COL]] == H_T) {
        payload->allMoves[payload->ALL_INDEX][MOVE_PIECE_TYPE] = H_T;
    }
    if (payload->board[payload->allMoves[payload->ALL_INDEX][MOVE_FROM_ROW]][payload->allMoves[payload->ALL_INDEX][MOVE_FROM_COL]] == H_X) {
        payload->allMoves[payload->ALL_INDEX][MOVE_PIECE_TYPE] = H_X;
    }

    if (payload->board[payload->allMoves[payload->ALL_INDEX][MOVE_FROM_ROW]][payload->allMoves[payload->ALL_INDEX][MOVE_FROM_COL]] == C_T) {
        payload->allMoves[payload->ALL_INDEX][MOVE_WHOSE_MOVE] = COM_TURN;
    }

    if (payload->board[payload->allMoves[payload->ALL_INDEX][MOVE_FROM_ROW]][payload->allMoves[payload->ALL_INDEX][MOVE_FROM_COL]] == C_X) {
            payload->allMoves[payload->ALL_INDEX][MOVE_WHOSE_MOVE] = COM_TURN;
    }

    if (payload->board[payload->allMoves[payload->ALL_INDEX][MOVE_FROM_ROW]][payload->allMoves[payload->ALL_INDEX][MOVE_FROM_COL]] == C_T) {
        payload->allMoves[payload->ALL_INDEX][MOVE_PIECE_TYPE] = C_T;
    }
    if (payload->board[payload->allMoves[payload->ALL_INDEX][MOVE_FROM_ROW]][payload->allMoves[payload->ALL_INDEX][MOVE_FROM_COL]] == C_X) {
        payload->allMoves[payload->ALL_INDEX][MOVE_PIECE_TYPE] = C_X;
    }

    // Copy currentpayload->move into computer/human moves data-set
    if(payload->allMoves[payload->ALL_INDEX][MOVE_WHOSE_MOVE] == COM_TURN){
        memcpy(&payload->allCompMoves[payload->NUM_OF_COM_MOVES++], &payload->allMoves[payload->ALL_INDEX], sizeof(int) * 7);
    }
    if(payload->allMoves[payload->ALL_INDEX][MOVE_WHOSE_MOVE] == HUM_TURN){
        memcpy(&payload->allHumanMoves[payload->NUM_OF_HUM_MOVES++], &payload->allMoves[payload->ALL_INDEX], sizeof(int) * 7);
    }
    payload->ALL_INDEX++;
}

/*      Find computer's moves       */

void moveCompTLeft(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) || collidesWithComFigures( payload))
        return;
    if(collidesWithBeatableHumFig(payload)){
        payload->move[VER_HOR] = HORIZONTAL;
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)) {
        payload->move[VER_HOR] = HORIZONTAL;
        addToDatabase(payload);
        payload->move[4] = -1;
        payload->move[5] = -1;
        payload->move[6] = -1;
        payload->move[COL]--; moveCompTLeft(payload); payload->move[COL]++;
    }
    return; //it's run into unbeatable hum figures
}
void moveCompTRight(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) || collidesWithComFigures(payload))
        return;
    if(collidesWithBeatableHumFig(payload)){
        payload->move[VER_HOR] = HORIZONTAL;
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)) {
        payload->move[VER_HOR] = HORIZONTAL;
        addToDatabase(payload);
        payload->move[4] = -1;
        payload->move[5] = -1;
        payload->move[6] = -1;

        payload->move[COL]++; moveCompTRight(payload); payload->move[COL]--;
    }
    return; //it's run into unbeatable hum figures
}
void moveCompTUp(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) || collidesWithComFigures(payload))
        return;

    if(collidesWithHumDeath(payload) || collidesWithBeatableHumFig(payload)){
        addToDatabase(payload);
        return;
    }
    payload->move[ROW]--; moveCompTUp(payload); payload->move[ROW]++;
    return;
}
void moveCompTDown(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) || collidesWithComFigures(payload))
        return;
    if(collidesWithBeatableHumFig(payload)){
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)) {
        addToDatabase(payload);
        payload->move[4] = -1;
        payload->move[5] = -1;
        payload->move[6] = -1;
        payload->move[ROW]++; moveCompTDown(payload); payload->move[ROW]--;
    }
    return; //return if it's run into a DEATH STAR
}

void findAllMovesForCompT(struct payload *payload){
    if(payload->COM_T_FIGHTER_HOR_MOVE == false) {
        payload->move[COL]--; moveCompTLeft(payload); payload->move[COL]++;
        payload->move[COL]++; moveCompTRight(payload); payload->move[COL]--;
    }
    payload->move[ROW]--; moveCompTUp( payload); payload->move[ROW]++;
    payload->move[ROW]++; moveCompTDown(payload); payload->move[ROW]--;
}

void moveCompXDownLeft(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) )
        return;
    if(collidesWithBeatableHumFig(payload)) {
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)){
        addToDatabase(payload);
        payload->move[ROW]++; payload->move[COL]--; moveCompXDownLeft(payload);  payload->move[ROW]--; payload->move[COL]++;
        return;
    }
}

void moveCompXDownRight(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) ){
        return;
    }
    if(collidesWithBeatableHumFig(payload)){
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)){
        addToDatabase(payload);
       payload->move[ROW]++;payload->move[COL]++; moveCompXDownRight(payload); payload->move[ROW]--;payload->move[COL]--;
        return;
    }
}

void moveCompXUpLeft(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) ) {
        return;
    }
    if(collidesWithBeatableHumFig(payload)) {
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)){
       payload->move[ROW]--;payload->move[COL]--; moveCompXUpLeft(payload);payload->move[ROW]++;payload->move[COL]++;
        return;
    }
    if(collidesWithHumDeath(payload)){
        addToDatabase(payload);
        return;
    }
    return;
}

void moveCompXUpRight(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move)) {
        return;
    }
    if(collidesWithBeatableHumFig(payload)) {
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)){
       payload->move[ROW]--;payload->move[COL]++; moveCompXUpRight(payload);payload->move[ROW]++;payload->move[COL]--;
        return;
    }
    if(collidesWithHumDeath(payload)){
        addToDatabase(payload);
        return;
    }
    return;
}

void findAllMovesForCompX(struct payload *payload){

   payload->move[COL]--;payload->move[ROW]++; moveCompXDownLeft(payload); payload->move[COL]++;payload->move[ROW]--;
   payload->move[COL]++;payload->move[ROW]++; moveCompXDownRight(payload); payload->move[COL]--;payload->move[ROW]--;
   payload->move[COL]--;payload->move[ROW]--; moveCompXUpLeft(payload); payload->move[COL]++;payload->move[ROW]++;
   payload->move[COL]++;payload->move[ROW]--; moveCompXUpRight(payload); payload->move[COL]--;payload->move[ROW]++;
}

void generateAllMoves(struct payload *payload){
    memset(payload->allMoves, -1, sizeof(payload->allMoves[0][0]) * 200 * 7);
    memset(payload->allCompMoves, -1, sizeof(payload->allMoves[0][0]) * 200 * 7);
    memset(payload->allHumanMoves, -1, sizeof(payload->allMoves[0][0]) * 200 * 7);
    payload->NUM_OF_HUM_MOVES = 0;
    payload->NUM_OF_COM_MOVES = 0;
    payload->ALL_INDEX = payload->HUM_INDEX = payload->COM_INDEX = 0; // index at which thepayload->move is being processed.

    memset(payload->move, -1, sizeof(int)*7);
    for(int i = 0; i < BRD_LENGTH; i++){
        for(int j = 0; j < BRD_LENGTH; j++){
            //Only X-WINGs and TIE FIGHTERs canpayload->move. WALLs and DEATH STAR cannotpayload->move.
            if(payload->board[i][j] != C_W &&
               payload->board[i][j] != C_D &&
               payload->board[i][j] != H_W &&
               payload->board[i][j] != H_D){
                    payload->move[0] = i;
                    payload->move[1] = j;
                    payload->move[2] = i;
                    payload->move[3] = j;
                    payload->move[4] = -1;   // To be assigned down the function stack.
                    payload->move[5] = -1;   // To be assigned down the function stack.
                    payload->move[6] = -1;   // To be assigned down the function stack.

                    if(payload->board[i][j] == C_T){
                        findAllMovesForCompT(payload);
                    }  if(payload->board[i][j] == C_X) {
                        findAllMovesForCompX(payload);
                    }  if(payload->board[i][j] == H_X){
                        findAllMovesForHumX(payload);
                    }  if(payload->board[i][j] == H_T){
                        findAllMovesForHumT(payload);
                    }
            }
        }
    }
}


void moveHumXUpLeft(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) || collidesWithHumFigures(payload))
        return;
    if(collidesWithBeatableComFig(payload)) {
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)){
        addToDatabase(payload);
       payload->move[ROW]--;payload->move[COL]--; moveHumXUpLeft(payload); payload->move[ROW]++;payload->move[COL]++;
        return;
    }
    return;
}


void moveHumXUpRight(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) || collidesWithHumFigures(payload)){
        return;
    }
    if(collidesWithBeatableComFig(payload)){
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)){
        addToDatabase(payload);
       payload->move[ROW]--;payload->move[COL]++; moveHumXUpRight(payload); payload->move[ROW]++;payload->move[COL]--;
        return;
    }
    return;
}

void moveHumXDownLeft(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move)) {
        return;
    }
    if(collidesWithBeatableComFig(payload)) {
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)){
       payload->move[ROW]++;payload->move[COL]--; moveHumXDownLeft(payload);payload->move[ROW]--;payload->move[COL]++;
        return;
    }
    if(collidesWithComDeath(payload)){
        addToDatabase(payload);
        return;
    }
    return;
}

void moveHumXDownRight(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move)) {
        return;
    }
    if(collidesWithBeatableComFig(payload)) {
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)){
       payload->move[ROW]++;payload->move[COL]++; moveHumXDownRight(payload);payload->move[ROW]--;payload->move[COL]--;
        return;
    }
    if(collidesWithComDeath(payload)){
        addToDatabase(payload);
        return;
    }
    return;
}

void findAllMovesForHumX(struct payload *payload){
   payload->move[ROW]++;payload->move[COL]--;  moveHumXDownLeft(payload); payload->move[ROW]--;payload->move[COL]++;
   payload->move[ROW]++;payload->move[COL]++;  moveHumXDownRight(payload);payload->move[ROW]--;payload->move[COL]--;
   payload->move[ROW]--;payload->move[COL]--;  moveHumXUpLeft(payload); payload->move[ROW]++;payload->move[COL]++;
   payload->move[ROW]--;payload->move[COL]++;  moveHumXUpRight(payload); payload->move[ROW]++;payload->move[COL]--;
}

void moveHumTLeft(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) || collidesWithHumFigures(payload))
        return;
    if(collidesWithBeatableComFig(payload)){
       payload->move[VER_HOR] = HORIZONTAL;
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)) {
       payload->move[VER_HOR] = HORIZONTAL;
        addToDatabase(payload);
       payload->move[4] = -1;
       payload->move[5] = -1;
       payload->move[6] = -1;
       payload->move[COL]--; moveHumTLeft(payload);payload->move[COL]++;
    }
    return; //it's run into unbeatable hum figures
}
void moveHumTRight(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) || collidesWithHumFigures(payload))
        return;
    if(collidesWithBeatableComFig(payload)){
       payload->move[VER_HOR] = HORIZONTAL;
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)) {
       payload->move[VER_HOR] = HORIZONTAL;
        addToDatabase(payload);
       payload->move[4] = -1;
       payload->move[5] = -1;
       payload->move[6] = -1;
       payload->move[COL]++; moveHumTRight(payload);payload->move[COL]--;
    }
    return; //it's run into unbeatable hum figures
}
void moveHumTDown(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) || collidesWithHumFigures(payload))
        return;

    if(collidesWithComDeath(payload) || collidesWithBeatableComFig(payload)){
        addToDatabase(payload);
        return;
    }
   payload->move[ROW]++; moveHumTDown(payload);payload->move[ROW]--;
    return;
}
void moveHumTUp(struct payload *payload){
    if(isMoveBeyondBoundaries(payload->move) || collidesWithHumFigures(payload))
        return;
    if(collidesWithBeatableComFig(payload)){
        addToDatabase(payload);
        return;
    }
    if(emptySpot(payload)) {
        addToDatabase(payload);
       payload->move[4] = -1;
       payload->move[5] = -1;
       payload->move[6] = -1;
       payload->move[ROW]--; moveHumTUp(payload);payload->move[ROW]++;
    }
    return; //return if it's run into a DEATH STAR
}

void findAllMovesForHumT(struct payload *payload){
    if(payload->HUM_T_FIGHTER_HOR_MOVE == false) {
       payload->move[COL]--; moveHumTLeft(payload);payload->move[COL]++;
       payload->move[COL]++; moveHumTRight(payload);payload->move[COL]--;
    }
   payload->move[ROW]--; moveHumTUp(payload);payload->move[ROW]++;
   payload->move[ROW]++; moveHumTDown(payload);payload->move[ROW]--;
}

void getAMove(struct payload *payload){
    char move[5];
    cout << endl << "Enter your move: " ;
    cin >> move[0];
    cin >> move[1];
    cin >> move[2];
    cin >> move[3];
    move[4] = '\0';
    cout << endl;
    convertMoveToInternalRep(payload, move);
#if RUN_WITH_MOVE_VALIDATION
    if(!moveLegal(payload)) {
        cout << "The move is illegal. Only legal moves are acceptable!" << endl;
        cout << "The illegal move is: " << payload->moveFromI[0] <<", " <<  payload->moveFromI[1]  << ", " << payload->moveToI[0] << ", " << payload->moveToI[1];
        printBoard(payload);
        getAMove(payload);
        return;
    }
#endif
    payload->HUM_MOVE[0] = payload->moveFromI[0];
    payload->HUM_MOVE[1] = payload->moveFromI[1];
    payload->HUM_MOVE[2] = payload->moveToI[0];
    payload->HUM_MOVE[3] = payload->moveToI[1];

#if RUN_TEST
    cout << endl << "H_M_0: " << HUM_MOVE[0] << endl;
    cout << "H_M_1: " << HUM_MOVE[1] << endl;
    cout << "H_M_2: " << HUM_MOVE[2] << endl;
    cout << "H_M_3: " << HUM_MOVE[3] << endl;
#endif

    if(payload->HUM_MOVE[0] == payload->HUM_MOVE[2] && payload->HUM_MOVE[1] != payload->HUM_MOVE[3]){
        payload->HUM_T_FIGHTER_HOR_MOVE = true;
#if RUN_TEST
        cout << "HUMAN GLOBAL HORpayload->move TRUE:  " << HUM_T_FIGHTER_HOR_MOVE << endl;
#endif
    } else {
        payload->HUM_T_FIGHTER_HOR_MOVE = false;
#if RUN_TEST
        cout << "HUMAN GLOBAL HORpayload->move: FALSE: " << HUM_T_FIGHTER_HOR_MOVE << endl;
#endif
    }
}

void makeHumMove(struct payload *payload){
    int piece_to_move_to_new_loc =  payload->board[payload->HUM_MOVE[MOVE_FROM_ROW]][payload->HUM_MOVE[MOVE_FROM_COL]];
    payload->board[payload->HUM_MOVE[MOVE_FROM_ROW]][payload->HUM_MOVE[MOVE_FROM_COL]] = EMPTY;
    payload->board[payload->HUM_MOVE[MOVE_TO_ROW]][payload->HUM_MOVE[MOVE_TO_COL]] = piece_to_move_to_new_loc;

    generateAllMoves(payload);
}

void checkGameOver(struct payload *payload){
    if(checkForWinner(0, payload) == H_WINS) { // -5000
        printBoard(payload);
        cout << "Human win" << endl;
        exit(0);
    }
    if(checkForWinner(0, payload) == C_WINS){  // 5000
        printBoard(payload);
        cout << "Computer win " << endl;
        exit(0);
    }
}

void resetAllMovesAfterChangeOnBoard(struct payload *payload){
    generateAllMoves(payload);
}

char* convertMoveToAnotherArtOpponent(char *moveToConvert){
    /* how not to love c++ hacks... */
    char *convertedMove = new char[4];
    int offsetToA = int('A') - 1;
    convertedMove[0] = char(offsetToA + (int('H')) - moveToConvert[0]);
    convertedMove[2] = char(offsetToA + (int('H')) - moveToConvert[2]);
    convertedMove[1] = (char)(int('0') + int('8') - int(moveToConvert[1]));
    convertedMove[3] = (char)(int('0') + int('8') - int(moveToConvert[3]));
    convertedMove[4] = '\0';

    return convertedMove;
}
void makeCompMoveOnBoard(struct payload *payload){

    for(int i = 0; i < 50; i++){
        killerMove[i].empty[0] = true;
        killerMove[i].empty[1] = true;
    }

    payload->last_time = std::chrono::high_resolution_clock::now();
    payload->COLLAPSE_RECURSION = false;
    payload->DEPTH_FOR_OUTPUT = 0;
    int number_of_nodes = 0;
    int depth = 2;
    int *best_move;
    int *last_move;
    int bestScore;
    while(true){
        payload->MAXDEPTH = depth;
        payload->NUM_OF_NODES = 0;
        last_move = makeComMove(payload);
        if(payload->COLLAPSE_RECURSION == false){
            best_move = last_move;
            number_of_nodes = payload->NUM_OF_NODES;
            bestScore = payload->BEST_SCORE;
        }else {
            break;
        }
        depth++;
    }

    char *compMoveExtRep = convertMoveToExternalRep(best_move);
    cout << endl << "Computer hor/ver is: " << best_move[VER_HOR] << endl;
    cout << endl << endl << "Opponent's move is: " << compMoveExtRep << "(" << convertMoveToAnotherArtOpponent(compMoveExtRep) << ")" << endl;
#if RUN_WITH_ADDITIONAL_OUTPUT
    cout << endl << "Best: " << bestScore << endl;
    cout << endl << "The algorithm went through " << number_of_nodes << " calls. Last iteration stopped at " << payload->NUM_OF_NODES;
    cout << endl << "The maximum depth was: " << payload->DEPTH_FOR_OUTPUT << endl;
#endif
    int prevMove = payload->board[best_move[MOVE_FROM_ROW]][best_move[MOVE_FROM_COL]];
    payload->board[best_move[MOVE_FROM_ROW]][best_move[MOVE_FROM_COL]] = EMPTY;
    payload->board[best_move[MOVE_TO_ROW]][best_move[MOVE_TO_COL]] = prevMove;

    resetAllMovesAfterChangeOnBoard(payload);

    if(prevMove == C_T && best_move[VER_HOR] == HORIZONTAL) {
        payload->COM_T_FIGHTER_HOR_MOVE = true;
    }
    else {
        payload->COM_T_FIGHTER_HOR_MOVE = false;
    }

    resetAllMovesAfterChangeOnBoard(payload);
    printBoard(payload);
    showAllMoves(BOTH, payload);
}

int* makeComMove(struct payload *payload){
    auto currentTime = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - payload->last_time).count() > payload->MAX_MILS_ONE_ITER) {
        payload->COLLAPSE_RECURSION = true;
        return new int[0];
    }
    payload->NUM_OF_NODES++;
    int best =-20000, depth = 0, score;
    int BEST_MOVE[7], LOCAL_MOVE[4];
    int counter = 0;
    bool LOC_HUM_T_FIGHTER_HOR_MOVE;
    bool LOC_COM_T_FIGHTER_HOR_MOVE;

    int maxMoves = payload->NUM_OF_COM_MOVES;
    for(;;){
        generateAllMoves(payload);

        LOC_COM_T_FIGHTER_HOR_MOVE = payload->COM_T_FIGHTER_HOR_MOVE;
        LOC_HUM_T_FIGHTER_HOR_MOVE = payload->HUM_T_FIGHTER_HOR_MOVE;

        LOCAL_MOVE[0] = payload->allCompMoves[counter][0];
        LOCAL_MOVE[1] = payload->allCompMoves[counter][1];
        LOCAL_MOVE[2] = payload->allCompMoves[counter][2];
        LOCAL_MOVE[3] = payload->allCompMoves[counter][3];

        int prev_piece = payload->board[LOCAL_MOVE[MOVE_TO_ROW]][LOCAL_MOVE[MOVE_TO_COL]];

        int prev_loc_of_piece = payload->board[LOCAL_MOVE[MOVE_FROM_ROW]][LOCAL_MOVE[MOVE_FROM_COL]];
        payload->board[LOCAL_MOVE[MOVE_FROM_ROW]][LOCAL_MOVE[MOVE_FROM_COL]] = EMPTY;
        payload->board[LOCAL_MOVE[MOVE_TO_ROW]][LOCAL_MOVE[MOVE_TO_COL]] = prev_loc_of_piece;

        if(prev_loc_of_piece == C_T && payload->allCompMoves[counter][VER_HOR] == HORIZONTAL)
            payload->COM_T_FIGHTER_HOR_MOVE = true;
        else
            payload->COM_T_FIGHTER_HOR_MOVE = false;

        /* Reset all calls for the next max iteration.
         * If COM_T_FIGHTER_HOR_MOVE is true, then Horizontal T Fighter moves
         * will not be generated at all*/
        resetAllMovesAfterChangeOnBoard(payload);

        score = min(depth+1, best, payload);

        /*payload->move back the piece */
        payload->board[LOCAL_MOVE[MOVE_TO_ROW]][LOCAL_MOVE[MOVE_TO_COL]] = prev_piece;
        payload->board[LOCAL_MOVE[MOVE_FROM_ROW]][LOCAL_MOVE[MOVE_FROM_COL]] = prev_loc_of_piece;

        payload->HUM_T_FIGHTER_HOR_MOVE  = LOC_HUM_T_FIGHTER_HOR_MOVE;
        payload->COM_T_FIGHTER_HOR_MOVE = LOC_COM_T_FIGHTER_HOR_MOVE;

        resetAllMovesAfterChangeOnBoard(payload);

        if(score > best){
            //TODO: replace with memcpy
            BEST_MOVE[0] = payload->allCompMoves[counter][0];
            BEST_MOVE[1] = payload->allCompMoves[counter][1];
            BEST_MOVE[2] = payload->allCompMoves[counter][2];
            BEST_MOVE[3] = payload->allCompMoves[counter][3];
            BEST_MOVE[4] = payload->allCompMoves[counter][4];
            BEST_MOVE[5] = payload->allCompMoves[counter][5];
            BEST_MOVE[6] = payload->allCompMoves[counter][6];
            best = score;
        }

        counter++;
        if(counter >= maxMoves){
            break;
        }
    }
    payload->BEST_SCORE = best;
    int *returnValue = (int*)calloc(7, sizeof(int));
    memcpy(returnValue, BEST_MOVE, sizeof(int) * 7);
    return returnValue;
}

int findKillerMoveInAllHumMoves(KillerMove *killerMove, struct payload *payload) {
    for (int i = 0; i < payload->NUM_OF_HUM_MOVES; i++) {
        if (killerMove->empty[0] == false) {
            if ((payload->allHumanMoves[i][MOVE_TO_ROW] == killerMove->moveTo[0][0]) &&
                (payload->allHumanMoves[i][MOVE_TO_COL] == killerMove->moveTo[0][1])) {
                return i;
            }
        }
        if (killerMove->empty[1] == false) {
            if ((payload->allHumanMoves[i][MOVE_TO_ROW] == killerMove->moveTo[1][0]) &&
                (payload->allHumanMoves[i][MOVE_TO_COL] == killerMove->moveTo[1][1])) {
                return i;
            }
        }
    }
    return -100000;
}

int min(int depth, int previousBest, struct payload *payload){
    auto currentTime = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - payload->last_time).count() > payload->MAX_MILS_ONE_ITER) {
        payload->COLLAPSE_RECURSION = true;
        return false;
    }
    payload->NUM_OF_NODES++;
    int best = 20000;
    int score;
    int  LOCAL_HUM_MOVE[4];
    int counter = 0;

    bool LOC_HUM_T_FIGHTER_HOR_MOVE;
    bool LOC_COM_T_FIGHTER_HOR_MOVE;

//    if(checkForWinner(depth) != GAME_NOT_OVER  && depth >= MAXDEPTH - 1) {
//        return checkForWinner(depth);
//    }
    if(depth == payload->MAXDEPTH) {
        payload->DEPTH_FOR_OUTPUT = payload->MAXDEPTH;
        return evaluate(payload);
    }

    int maxMoves = payload->NUM_OF_HUM_MOVES;

    for(;;){
        generateAllMoves(payload);

        LOC_COM_T_FIGHTER_HOR_MOVE = payload->COM_T_FIGHTER_HOR_MOVE;
        LOC_HUM_T_FIGHTER_HOR_MOVE = payload->HUM_T_FIGHTER_HOR_MOVE;

        LOCAL_HUM_MOVE[0] = payload->allHumanMoves[counter][0];
        LOCAL_HUM_MOVE[1] = payload->allHumanMoves[counter][1];
        LOCAL_HUM_MOVE[2] = payload->allHumanMoves[counter][2];
        LOCAL_HUM_MOVE[3] = payload->allHumanMoves[counter][3];

        int prev_piece =  payload->board[LOCAL_HUM_MOVE[MOVE_TO_ROW]][LOCAL_HUM_MOVE[MOVE_TO_COL]];

        int prev_loc_of_piece = payload->board[LOCAL_HUM_MOVE[MOVE_FROM_ROW]][LOCAL_HUM_MOVE[MOVE_FROM_COL]];
        payload->board[LOCAL_HUM_MOVE[MOVE_FROM_ROW]][LOCAL_HUM_MOVE[MOVE_FROM_COL]] = EMPTY;
        payload->board[LOCAL_HUM_MOVE[MOVE_TO_ROW]][LOCAL_HUM_MOVE[MOVE_TO_COL]] = prev_loc_of_piece;


        if(prev_loc_of_piece == H_T && payload->allHumanMoves[counter][VER_HOR] == HORIZONTAL)
            payload->HUM_T_FIGHTER_HOR_MOVE = true;
        else
            payload->HUM_T_FIGHTER_HOR_MOVE = false;

        resetAllMovesAfterChangeOnBoard(payload);

        score = max(depth+1, best, payload);

        //undopayload->move
        payload->board[LOCAL_HUM_MOVE[MOVE_TO_ROW]][LOCAL_HUM_MOVE[MOVE_TO_COL]] = prev_piece;
        payload->board[LOCAL_HUM_MOVE[MOVE_FROM_ROW]][LOCAL_HUM_MOVE[MOVE_FROM_COL]] = prev_loc_of_piece;


        payload->HUM_T_FIGHTER_HOR_MOVE = LOC_COM_T_FIGHTER_HOR_MOVE;
        payload->COM_T_FIGHTER_HOR_MOVE = LOC_HUM_T_FIGHTER_HOR_MOVE;

        resetAllMovesAfterChangeOnBoard(payload);


        if(score < best){
            best = score;
        }

        if(previousBest >= best){

#if RUN_ALPHA_BETA_OPTIMIZATION
            return best;
#endif
        }
        counter++;
        if(counter >= maxMoves){
            break;
        }
    }
    return best;
}

int findKillerMoveInAllCompMoves(KillerMove *killerMove, struct payload *payload) {
    for (int i = 0; i < payload->NUM_OF_COM_MOVES; i++) {
        if (killerMove->empty[0] == false) {
            if ((payload->allCompMoves[i][MOVE_TO_ROW] == killerMove->moveTo[0][0])
                && (payload->allCompMoves[i][MOVE_TO_COL] == killerMove->moveTo[0][1])) {
                return i;
            }
        }
        if (killerMove->empty[1] == false) {
            if ((payload->allCompMoves[i][MOVE_TO_ROW] == killerMove->moveTo[1][0]) &&
                (payload->allCompMoves[i][MOVE_TO_COL] == killerMove->moveTo[1][1])) {
                return i;
            }
        }
    }
    return -100000;
}



int checkForWinner(int depth, struct payload *payload){
    if (compWins(payload)){
        //cout << "\nDepth is : "  << depth << endl;
        return C_WINS - depth; //computer wins 5000
    }

    if (humWins(payload))
        return H_WINS + depth; //human wins -5000

    return GAME_NOT_OVER;
}

int max(int depth, int previousBest, struct payload *payload) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - payload->last_time).count() > payload->MAX_MILS_ONE_ITER) {
        payload->COLLAPSE_RECURSION = true;
        return false;
    }
    payload->NUM_OF_NODES++;
    int best = -20000;
    int score;
    int LOCAL_COM_MOVE[4];
    int counter = 0;

    bool LOC_HUM_T_FIGHTER_HOR_MOVE;
    bool LOC_COM_T_FIGHTER_HOR_MOVE;

//    if(checkForWinner(depth) != GAME_NOT_OVER && depth >= MAXDEPTH - 1) {
//        return checkForWinner(depth);
//    }
    if(depth == payload->MAXDEPTH){
        payload->DEPTH_FOR_OUTPUT = payload->MAXDEPTH;
        return (evaluate(payload));
    }

    int maxMoves = payload->NUM_OF_COM_MOVES;

    for(;;){


        LOC_COM_T_FIGHTER_HOR_MOVE = payload->COM_T_FIGHTER_HOR_MOVE;
        LOC_HUM_T_FIGHTER_HOR_MOVE = payload->HUM_T_FIGHTER_HOR_MOVE;

        LOCAL_COM_MOVE[0] = payload->allCompMoves[counter][0];
        LOCAL_COM_MOVE[1] = payload->allCompMoves[counter][1];
        LOCAL_COM_MOVE[2] = payload->allCompMoves[counter][2];
        LOCAL_COM_MOVE[3] = payload->allCompMoves[counter][3];

        int prev_piece =   payload->board[LOCAL_COM_MOVE[MOVE_TO_ROW]][LOCAL_COM_MOVE[MOVE_TO_COL]];

        int prev_loc_of_piece = payload->board[LOCAL_COM_MOVE[MOVE_FROM_ROW]][LOCAL_COM_MOVE[MOVE_FROM_COL]];
        payload->board[LOCAL_COM_MOVE[MOVE_FROM_ROW]][LOCAL_COM_MOVE[MOVE_FROM_COL]] = EMPTY;
        payload->board[LOCAL_COM_MOVE[MOVE_TO_ROW]][LOCAL_COM_MOVE[MOVE_TO_COL]] = prev_loc_of_piece;

        if(prev_loc_of_piece == C_T && payload->allCompMoves[counter][VER_HOR] == HORIZONTAL)
            payload->COM_T_FIGHTER_HOR_MOVE = true;
        else
            payload->COM_T_FIGHTER_HOR_MOVE = false;

        resetAllMovesAfterChangeOnBoard(payload);

        score = min(depth+1, best, payload);

        //undopayload->move
        payload->board[LOCAL_COM_MOVE[MOVE_TO_ROW]][LOCAL_COM_MOVE[MOVE_TO_COL]] = prev_piece;
        payload->board[LOCAL_COM_MOVE[MOVE_FROM_ROW]][LOCAL_COM_MOVE[MOVE_FROM_COL]] = prev_loc_of_piece;

        payload->HUM_T_FIGHTER_HOR_MOVE = LOC_HUM_T_FIGHTER_HOR_MOVE;
        payload->COM_T_FIGHTER_HOR_MOVE = LOC_COM_T_FIGHTER_HOR_MOVE;

        resetAllMovesAfterChangeOnBoard(payload);

        if(score >  best){
            best = score;
        }

        if(previousBest <= best){

#if RUN_ALPHA_BETA_OPTIMIZATION
            return best;
#endif
        }

        counter++;
        if(counter >= maxMoves){
            break;
        }
    }
    return best;
}


#pragma clang diagnostic pop