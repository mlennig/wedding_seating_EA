//
//  Mim_Lennig_FinalProject_classes.cpp
//  COEN 432
//  Assignment #1 A
//
//  Created by Miriam Lennig on 2016-11-25.
//  Copyright © 2018 Miriam Lennig. All rights reserved.
//

#include "Mim_Lennig_FinalProject_classes.hpp"
#include <vector>
#include <cassert>
#include <fstream>

// Rule
// Each object of the Rule class represents one rule
Rule::Rule(){
    // Generate a random mask

    
}

Rule::Rule(const Rule& r){
    
}

Rule::Rule(Condition inputMask[maskSize * maskSize], Move inputMove){
    for(int i = 0; i < maskSize * maskSize; i++)
        mask[i] = inputMask[i];
    
    action = inputMove;
}

// Strategy
// Each object of the strategy class is a specific strategy for playing the Walls game
Strategy::Strategy(){
    int numberOfRules = (rand() % 46) + 5;
    for(int k = 0; k < numberOfRules; k++){
        Rule* rule = new Rule;
        rules.push_back(rule);
    }
}

// Create a standard player by human designed rules
Strategy::Strategy(int n){

    
}

Strategy::Strategy(char c){
    rules.clear();
}

Strategy::Strategy(string fileName){
    // Construct a strategy from a strategy stored in a file
    ifstream infile;
    
    infile.open(fileName, ios::in | ios::binary);   // Open fileName for read
    
    while(!infile.eof()){
        Rule* r = new Rule; // Create a new rule
        infile.read((char*)&r->mask, sizeof(r->mask));     // Read mask
        infile.read((char*)&r->action, sizeof(r->action));  // Read action
        rules.push_back(r); // Store the rule in the strategy
    }
    
    infile.close();
}

void Strategy::store2file(string fileName){
    // Store a strategy in a file
    ofstream of;
    
    of.open(fileName, ios::out | ios::binary);  // Open new file for write
    
    for(int i = 0; i < rules.size(); i++){
        of.write((char*)&rules[i]->mask, sizeof(rules[i]->mask));   // Write the mask in binary
        of.write((char*)&rules[i]->action, sizeof(rules[i]->action));   // Write the action in binary
    }
    
    of.close();
}


Strategy::Strategy(const Strategy& s){
    // Copy constructor
    for(int k = 0; k < s.rules.size(); k++)
        rules.push_back(new Rule(*s.rules[k]));
}

Strategy* Strategy::mutate(Strategy* parent){
    // Construct a new child strategy via mutation
    Mutation m[4] = {Add, Reorder, Modify, Delete};
    Mutation mutation;
    if(parent->rules.size() < 5)
       mutation = m[rand() % 3];
    else
       mutation = m[rand() % 4];
    
    Strategy* child = new Strategy(*parent);
    vector<Rule*>::iterator it, it1, it2;
    int randomPosition, fromPosition, toPosition;
   
    Rule* rule2move;    // Variable for Reorder
    
    // Variables for Modify
    Rule* rule2beModified;
    int maskPosition;
    Condition conditions[9] = {north, south, east, west, empty, brickA, brickB, dontCare, edge};
    Condition conditionsWithoutHeadings[5] = {empty, brickA, brickB, dontCare, edge};
    int nHeadings = 0;
    Move moves[3] = {Left, Right, Forward};
    
    switch (mutation){
        case Add:
            // randomPosition == 0 means insert the new rule before the existing first rule
            // randomPosition == child->rules.size() means insert the new rule after the last rule
            randomPosition = rand() % (child->rules.size() + 1);
            it = child->rules.begin() + randomPosition;
            child->rules.insert(it, new Rule);
            break;
        case Delete:
            randomPosition = rand() % (child->rules.size());
            it = child->rules.begin() + randomPosition;
            delete child->rules[randomPosition];
            child->rules.erase(it);
            break;
        case Reorder:
            fromPosition = rand() % (child->rules.size());
            toPosition = rand() % (child->rules.size());
            it1 = child->rules.begin() + fromPosition;
            it2 = child->rules.begin() + toPosition;
            rule2move = child->rules[fromPosition];
            child->rules.erase(it1);
            child->rules.insert(it2, rule2move);
            break;
        case Modify:
            randomPosition = rand() % (child->rules.size());
            
            // Flip a coin
            if(rand() % 2 == 0){
                // Modify the mask
                rule2beModified = child->rules[randomPosition];
                maskPosition = rand() % maskSize * maskSize;
                
                if(maskPosition == maskSize * maskSize / 2)
                    rule2beModified->mask[maskPosition] = north;
                else {
                    for (int k = 0; k < maskSize * maskSize; k++)
                        if (rule2beModified->mask[k] <= west)
                            nHeadings++;
                    if(nHeadings >= 2)
                        // Other player in neighborhood mask
                        rule2beModified->mask[maskPosition] = conditionsWithoutHeadings[rand() % 5];
                    else
                        // Other player not in neighborhood mask
                        rule2beModified->mask[maskPosition] = conditions[rand() % 9];
                }
            }
            else
                // Modify the action
                rule2beModified = child->rules[randomPosition];
                rule2beModified->action = moves[rand() % 3];
            break;
        default:
            cout << "Illegal mutation.\n";
    }
    
    return child;
}

Strategy* Strategy::recombine(Strategy* parent1, Strategy* parent2){
    // Construct a new child strategy via recombination
    Strategy* child = new Strategy('c');    // Start with an empty strategy
    unsigned long p1size = parent1->rules.size();
    unsigned long p2size = parent2->rules.size();
    unsigned long minNumRules = min(p1size, p2size);
    unsigned long maxNumRules = max(p1size, p2size);
    unsigned long randNumRules = rand() % (maxNumRules - minNumRules + 1) + minNumRules;
    
    while(child->rules.size() < randNumRules){
        if(rand() % 2 == 0)
            child->rules.push_back(new Rule(*parent1->rules[rand() % parent1->rules.size()]));
        if(rand() % 2 == 0)
            child->rules.push_back(new Rule(*parent2->rules[rand() % parent2->rules.size()]));
    }
    
    return child;
}

int Strategy::m2b(int imask, int ipos){
   return ipos + imask - maskSize / 2;
}

Move Strategy::getMove(int p2m, Condition board[N][N], int xpos[2], int ypos[2], Condition heading[2]){
    Condition neighborhood[maskSize][maskSize];
    int opp = (p2m + 1) % 2;
    
    // Initialize neighborhood
    for(int row = 0; row < maskSize; row++)
        for(int col = 0; col < maskSize; col++){
            int x = m2b(row, xpos[p2m]);
            int y = m2b(col, ypos[p2m]);
            
            if((row == maskSize / 2) && (col == maskSize / 2))
                neighborhood[row][col] = heading[p2m];
            else if( (x < 0) || (y < 0) || (x >= N) || (y >= N) )
                neighborhood[row][col] = edge;
            else if( (x == xpos[opp]) && (y ==  ypos[opp]) )
                neighborhood[row][col] = heading[opp];
            else if(board[x][y] == brickA)
                if(p2m == 0)
                    neighborhood[row][col] = ownBrick;
                else
                    neighborhood[row][col] = oppBrick;
            else if(board[x][y] == brickB)
                if(p2m == 0)
                    neighborhood[row][col] = oppBrick;
                else
                    neighborhood[row][col] = ownBrick;
            else if(board[x][y] == empty)
                neighborhood[row][col] = empty;
        }
    
    // Normalize the neighborhood
    Condition normNeigh[maskSize * maskSize];
    normalize(neighborhood, normNeigh);
    
    for(int i = 0; i < rules.size(); i++)
        if(rules[i]->doesMatch(normNeigh))
            return rules[i]->action;
    
    return Forward;     // Default
}

// Neighborhood is the input and normNeigh is the normalized and linearized output
void Strategy::normalize(Condition neighborhood[maskSize][maskSize], Condition normNeigh[maskSize * maskSize]){
    // This rotates the neighborhood so p2m is heading North and it linearizes the rotated neighborhood
    // This function assumes maskSize = 3
    assert (maskSize == 3);
    
    int r0[maskSize * maskSize] = {0, 1, 2, 3, 4, 5, 6, 7, 8};      // No rotation
    int r1[maskSize * maskSize] = {6, 3, 0, 7, 4, 1, 8, 5, 2};      // Rotate CCW 90 degrees
    int r2[maskSize * maskSize] = {8, 7, 6, 5, 4, 3, 2, 1, 0};      // Rotate CCW 180 degrees
    int r3[maskSize * maskSize] = {2, 5, 8, 1, 4, 7, 0, 3, 6};      // Rotate CCW 270 degrees
    
    Condition heading = neighborhood[maskSize / 2][maskSize / 2];
    int* r;
    int rotation;
    Condition h[4] = {north, west, south, east};
    
    switch(heading){
        case north:
            r = r0;
            rotation = 0;
            break;
        case east:
            r = r1;
            rotation = 1;
            break;
        case south:
            r = r2;
            rotation = 2;
            break;
        case west:
            r = r3;
            rotation = 3;
            break;
        default:
            r = r0;
            rotation = 0;
            cout << "This is messed up.\n";
    }
    
    for(int row = 0; row < maskSize; row++)
        for(int col = 0; col < maskSize; col++)
            normNeigh[ r[maskSize * row + col] ] = neighborhood[row][col];
    
    for(int i = 0; i < maskSize * maskSize; i++)
        switch(normNeigh[i]){
            case north:
                normNeigh[i] = h[(0 + rotation) % 4];
                break;
            case east:
                normNeigh[i] = h[(3 + rotation) % 4];
                break;
            case south:
                normNeigh[i] = h[(2 + rotation) % 4];
                break;
            case west:
                normNeigh[i] = h[(1 + rotation) % 4];
                break;
            default:
                break;
        }
}

bool Rule::doesMatch(Condition neighborhood[maskSize * maskSize]){
    for(int i = 0; i < maskSize * maskSize; i++)
            if((neighborhood[i] != mask[i]) && (mask[i] != dontCare))
                return false;
            else
                continue;
    return true;
}

Strategy::~Strategy(){
    if(rules.size() > 0){
        for(int k = 0; k < rules.size(); k++){
            delete rules[k];
        }
    }
}

// Game
// Represents one game between strategies
Game::Game(Strategy* player0, Strategy* player1){
    strategy[0] = player0;
    strategy[1] = player1;
    
    xpos[0] = N/2;
    ypos[0] = N/2 - 1;
    heading[0] = west;
    
    xpos[1] = N/2;
    ypos[1] = N/2 + 1;
    heading[1] = east;
    
    // player to move = p2m = moveNumber % 2
    moveNumber = 0;
    
    // Clear board
    for(int row = 0; row < N; row++)
        for(int col = 0; col < N; col++)
            board[row][col] = empty;
}

void Game::makeHalfMove(){
    Move m;
    int p2m = moveNumber % 2;
    if(strategy[p2m] == 0){
        // Human player
        m = getHumanMove();
    }
    else{
        // Use AA
        clock_t timeStampBefore = clock();  // Start timer
        m = strategy[p2m]->getMove(p2m, board, xpos, ypos, heading);
        accumulateTiming(clock() - timeStampBefore);    // Accumulate time difference
        if((strategy[0] == 0) || (strategy[1] == 0))
            cout << "player " << p2m << " moves " << move2string(m) << endl;
            
    }
    executeMove(m);
    moveNumber++;
}

string Game::move2string(Move m){
    switch(m){
        case Left:
            return "left";
        case Right:
            return "right";
        case Forward:
            return "forward";
        default:
            cout << "Error in translating move.";
            return "Error in translating move.";
    }
}

Move Game::getHumanMove(){
    char inChar = 0;
    while(true){
        cout << "Human player " << moveNumber % 2 << " to play (L, R, F): ";
        cin >> inChar;
        
        switch(inChar){
            case 'L':
            case 'l':
                return Left;
            case 'R':
            case 'r':
                return Right;
            case 'F':
            case 'f':
                return Forward;
        }
    }
}

void Game::executeMove(Move m){
    // Checks to see if move m is possible and if so, executes it
    int p2m = moveNumber % 2;
  
    if(m == Forward){
        if (canMoveForward()){
            Condition p2mBrick = p2m ? brickB : brickA;
            
            if(board[xpos[p2m]][ypos[p2m]] == p2mBrick)
                noBrickMoves[p2m]++;
            else{
                board[xpos[p2m]][ypos[p2m]] = p2mBrick;    // Lay a brick
                noBrickMoves[p2m] = 0;
            }
            
            switch(heading[p2m]){
                case north:
                    xpos[p2m]--;
                    break;
                case east:
                    ypos[p2m]++;
                    break;
                case south:
                    xpos[p2m]++;
                    break;
                case west:
                    ypos[p2m]--;
                    break;
                default:
                    cout << "Illegal heading for forward motion\n";
                    break;
            }
        }
        else
            noBrickMoves[p2m]++;
    }
    else if(m == Left){
        noBrickMoves[p2m]++;
        
        switch(heading[p2m]){
            case north:
                heading[p2m] = west;
                break;
            case east:
                heading[p2m] = north;
                break;
            case south:
                heading[p2m] = east;
                break;
            case west:
                heading[p2m] = south;
                break;
            default:
                cout << "Illegal heading for turning left\n";
                break;
        }
    }
    else if(m == Right){
        noBrickMoves[p2m]++;
        
        switch(heading[p2m]){
            case north:
                heading[p2m] = east;
                break;
            case east:
                heading[p2m] = south;
                break;
            case south:
                heading[p2m] = west;
                break;
            case west:
                heading[p2m] = north;
                break;
            default:
                cout << "Illegal heading for turning right\n";
                break;
        }
    }
    else
        cout << "Illegal move\n";
    
    if(noBrickMoves[p2m] >= 9)
        isStalled[p2m] = true;
}

// Test if p2m can move forward
bool Game::canMoveForward(){
    int p2m = moveNumber % 2;
    int opp = (p2m + 1) % 2;
    
    int x = xpos[p2m];
    int y = ypos[p2m];
    Condition opposeBrick = p2m ? brickA : brickB;
    
    switch(heading[p2m]){
        case north:
            x--;
            break;
        case east:
            y++;
            break;
        case south:
            x++;
            break;
        case west:
            y--;
            break;
        default:
            cout << "Illegal heading\n";
            break;
    }
    
    if((x >= N) || (y >= N) || (y < 0) || (x < 0))
        return false;
    else if(board[x][y] == opposeBrick)
        return false;
    else if((x == xpos[opp]) && (y == ypos[opp]))
        return false;
    
    return true;
}

string Game::facing(Condition h){
    switch(h){
        case north:
            return "north";
        case east:
            return "east";
        case south:
            return "south";
        case west:
            return "west";
        default:
            cout << "Error translating heading.\n";
            return "Error translating heading.\n";
    }
}

bool Game::isOver(){
    int nAbricks = 0;
    int nBbricks = 0;
    
    for(int row = 0; row < N; row++)
        for(int col = 0; col < N; col++){
            if (board[row][col] == brickA)
                nAbricks++;
            else if(board[row][col] == brickB)
                nBbricks++;
        }
    computeScore();
    return (nAbricks + nBbricks == N * N) || ((isStalled[0]) && (isStalled[1]));
}

int Game::getNumBricks(int player){
    int count = 0;
    
    for(int row = 0; row < N; row++)
        for(int col = 0; col < N; col++)
            if(((board[row][col] == brickA) && (player == 0)) || ((board[row][col] == brickB) && (player == 1)))
                count++;
    return count;
}

void Game::print(){
    char cboard[N][N];
    
    for(int row = 0; row < N; row++){
        for(int col = 0; col < N; col++){
            switch (board[row][col]){
                case brickA:
                    cboard[row][col] = 'A';
                    break;
                case brickB:
                    cboard[row][col] = 'B';
                    break;
                case empty:
                    cboard[row][col] = '.';
                    break;
                default:
                    cout << "Error printing board\n";
            }
        }
    }
    
    cboard[xpos[0]][ypos[0]] = '0';
    cboard[xpos[1]][ypos[1]] = '1';
    
    cout << "\nmoveNumber # " << moveNumber << " player " << moveNumber % 2 << " to move\n";
    cout << "player 0 facing " << facing(heading[0]) << " with score: " << getNumBricks(0) << endl;
    cout << "player 1 facing " << facing(heading[1]) << " with score: " << getNumBricks(1) << endl;
    
    for(int row = 0; row < N; row++){
        for(int col = 0; col < N; col++){
            cout << cboard[row][col];
        }
        cout << endl;
    }
}

int Game::computeScore(){
    int nBrickA = 0;
    int nBrickB = 0;
    
    for(int row = 0; row < N; row++)
        for(int col = 0; col < N; col++)
            if (board[row][col] == brickA)
                nBrickA++;
            else if(board[row][col] == brickB)
                nBrickB++;
    
    if(nBrickA > nBrickB)
        return 1;
    else if(nBrickA < nBrickB)
        return -1;
    else
        return 0;
}

clock_t Game::totAAticks = 0;
unsigned long Game::totAAmoves = 0;

void Game::accumulateTiming(clock_t ticks){
    totAAticks += ticks;
    totAAmoves++;
}

double Game::getTiming(){
    // Return average cpu time per move in microseconds
    return  1000000. * totAAticks / CLOCKS_PER_SEC / totAAmoves;
}

unsigned long Game::getTotAAmoves(){
    return totAAmoves;
}

// Evolution
// Evolution is a singleton class whose constructor implements the evolutionary cycle
Evolution::Evolution(){
    standardPlayer = new Strategy(0);   // Man-made standard player
    
    // Fill population pool with random strategies
    for(int k = 0; k < 1000; k++){
        Strategy* strategy = new Strategy;
        population.push_back(strategy);
    }
    
    // Beginning of evolutionary cycle
    int iGeneration = 0;
    while(iGeneration < 1000){
        int populationUpperBound;
        
        if(iGeneration % 5 == 0)
            populationUpperBound = 900;
        else
            populationUpperBound = 1000;
        
        while(population.size() >= tournSize){
            for(int k = 0; k < tournSize; k++){
                int i = rand() % population.size();
                tournamentPlayers[k] = population[i];
                population.erase(population.begin() + i);
            }
         
            tentativeWinner = tournament();
            parents.push_back(tentativeWinner);
            
        }
        
        bool beatStandPlayer = false;
        
        for(int i = 0; i < parents.size(); i++)
            if(canBeatStandardPlayer(parents[i])){
                // Player has been able to beat standard player
                tentativeWinner = parents[i];
                tentativeWinner->store2file("wallsWinner");
                beatStandPlayer = true;
                cout << "An AA has beaten the standard player.\n";
                cout << "Winner has " << parents[i]->rules.size() << " rules\n";
                break;
            }
        
        if(beatStandPlayer)
            break;
        
        else {
            // No player has been able to beat the standard player
            // Generate children from the parent pool
            
            assert(population.size() == 0);
            
            // Initialize first parent
            int i = rand() % parents.size();
            Strategy* p0 = parents[i];
            parents.erase(parents.begin() + i);
            
            while(population.size() < populationUpperBound){
                i = rand() % parents.size();
                Strategy* p1 = parents[i];
                parents.erase(parents.begin() + i);
                
               // Mutate p0 & p1 once each to create 2 children
                Strategy* child = Strategy::mutate(p0);
                population.push_back(child);
        
                child = Strategy::mutate(p1);
                population.push_back(child);
                
                // Use recombination with p0 & p1 twice to create 2 more children (total 4)
                child = Strategy::recombine(p0, p1);
                population.push_back(child);
                
                child = Strategy::recombine(p0, p1);
                population.push_back(child);
                
                // Return parent p0 to population pool
                population.push_back(p0);
                
                p0 = p1;
            }
            
            while(population.size() < 1000)
                population.push_back(new Strategy);
        }
        iGeneration++;
        cout << "Generation # " << iGeneration << endl;
    }
    // Evolution is over
    cout << "No strategy could beat the standard player.\n";
}

Strategy* Evolution::getWinner(){
    return tentativeWinner;
}

Strategy* Evolution::tournament(){
    int scores[tournSize] = {0, 0, 0, 0, 0};
    
    for(int row = 0; row < tournSize; row ++){
        for(int col = 0; col < tournSize; col++)
            if(row == col)
                continue;
            else{
                Game* g = new Game(tournamentPlayers[row], tournamentPlayers[col]);
                while(!g->isOver()){
                    g->makeHalfMove();
                    
                }
                scores[row] += g->computeScore();
                scores[col] -= g->computeScore();
                
                delete g;
            }
    }
    
    int maxScore = -1000;
    unsigned long minNumRules = 10000;
    int iWinner = -1;
    
    for(int i = 0; i < tournSize; i++)
        if((scores[i] > maxScore) || ((scores[i] == maxScore) && (tournamentPlayers[i]->rules.size() < minNumRules))){
            maxScore = scores[i];
            minNumRules = tournamentPlayers[i]->rules.size();
            iWinner = i;
        }
    
    for (int i = 0; i < tournSize; i++){
        if(i != iWinner)
            delete tournamentPlayers[i];
    }
    
    return tournamentPlayers[iWinner];
}


bool Evolution::canBeatStandardPlayer(Strategy* s){
    int sScore = 0;
    int standScore = 0;
    
    Game* g = new Game(s, standardPlayer);
    while(!g->isOver()){
        g->makeHalfMove();
    }
    sScore += g->computeScore();
    standScore -= g->computeScore();
    
    delete g;
    
    g = new Game(standardPlayer, s);
    while(!g->isOver()){
        g->makeHalfMove();
    }
    standScore += g->computeScore();
    sScore -= g->computeScore();
    
    delete g;
    
    return sScore > standScore;
}

Evolution::~Evolution(){
     for(int k = 0; k < population.size(); k++)
         delete population[k];
    for(int k = 0; k < parents.size(); k++)
        delete parents[k];
}





