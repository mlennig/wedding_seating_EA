//
//  Mim_Lennig_FinalProject_classes.hpp
//  COEN 432
//  Assignment #1 A
//
//  Created by Miriam Lennig on 2016-11-25.
//  Copyright © 2018 Miriam Lennig. All rights reserved.
//

#ifndef Mim_Lennig_FinalProject_classes_hpp
#define Mim_Lennig_FinalProject_classes_hpp

#include <iostream>
#include <vector>
#include <string>
using namespace std;

static const int N = 3;
static const int maskSize = 3;
static const int tournSize = 5;
enum Move {Left, Right, Forward};
enum Condition {north, south, east, west, empty, brickA, brickB, dontCare, edge, ownBrick, oppBrick};
enum Mutation {Add, Delete, Reorder, Modify};


// This is a Walls rule
class Rule{
public:
    Rule();
    Rule(const Rule&);
    Rule(Condition[maskSize * maskSize], Move);
    Condition mask[N*N];
    Move action;
    bool doesMatch(Condition[maskSize * maskSize]);
};

// This is a Walls strategy
class Strategy{
    int m2b(int, int);
public:
    Strategy(); // Fills strategy with a random # of random rules
    Strategy(const Strategy&);  // Copy constructor
    Strategy(int);  // Standard player
    Strategy(char); // Empty strategy
    Strategy(string); // Standard player from file
    ~Strategy();
    void store2file(string);
    vector<Rule*> rules;
    void normalize(Condition[maskSize][maskSize], Condition[maskSize * maskSize]);
    Move getMove(int, Condition[N][N], int[2], int[2], Condition[2]);   // debug
    static Strategy* mutate(Strategy*);
    static Strategy* recombine(Strategy*, Strategy*);
   
};

class Game{
    int xpos[2];
    int ypos[2];
    Condition heading[2];
    Strategy* strategy[2];
    int noBrickMoves[2] = {0, 0};
    bool isStalled[2] = {false, false};
    int moveNumber;
    Condition board[N][N];
    static clock_t totAAticks;
    static unsigned long totAAmoves;
    
    Move getHumanMove();
    string facing(Condition);
    string move2string(Move);
    static void accumulateTiming(clock_t);
public:
    int computeScore();
    bool isOver();
    int getNumBricks(int);
    void print();
    void makeHalfMove();
    void executeMove(Move);
    bool canMoveForward();
    static double getTiming();
    static unsigned long getTotAAmoves();
    Game(Strategy*, Strategy*);
    };


// This is a Walls evolution which implements the GA to evolve a set of strategies
class Evolution{
    Strategy* tentativeWinner;
    Strategy* standardPlayer;
    vector<Strategy*> population;
    vector<Strategy*> parents;
    Strategy* tournamentPlayers[tournSize];
    Strategy* tournament();   // Returns index of tournament player
    bool canBeatStandardPlayer(Strategy*);
public:
    Evolution();
    ~Evolution();
    Strategy* getWinner();
};































#endif /* Mim_Lennig_FinalProject_classes_hpp */
