//
//  Mim_Lennig_FinalProject.cpp
//  COEN 432
//  Assignment #1 A
//
//  Created by Miriam Lennig on 2016-11-25.
//  Copyright © 2018 Miriam Lennig. All rights reserved.
//

#include <iostream>
#include "Mim_Lennig_FinalProject_classes.hpp"

int main() {
    enum RunMode{humanVSevolved = 0, humanVSstandard, standardVSevolved};
    RunMode runmode;
    int rm;
    
    cout << "Choose a run mode by entering an integer as follows:\n";
    cout << "\t0 : human player vs winner of evolution.\n";
    cout << "\t1 : human player vs standard player.\n";
    cout << "\t2 : standard player vs evolved player.\n";
    cout << "Enter integer run mode: ";
    cin >> rm;
    runmode = RunMode(rm);
    
    if(runmode == humanVSevolved){
        // Human player can play against winner of evolution
        Evolution e;
        Strategy* AA = e.getWinner();
        Game g0(AA, 0);
        
        while(!g0.isOver()){
            g0.print();
            g0.makeHalfMove();
        }
        cout << "Game over. Player 0 score: " << g0.getNumBricks(0) << "; Player 1 score: " << g0.getNumBricks(1) << endl;
        
        Game g1(0, AA);
        while(!g1.isOver()){
            g1.print();
            g1.makeHalfMove();
        }
        cout << "Game over. Player 0 score: " << g1.getNumBricks(0) << "; Player 1 score: " << g1.getNumBricks(1) << endl;
    }
    
    else if(runmode == humanVSstandard){
        // Human player can play against standard player
        Strategy* standardPlayer = new Strategy(0);
        
        Game g0(standardPlayer, 0);
        while(!g0.isOver()){
            g0.print();
            g0.makeHalfMove();
        }
        cout << "Game over. Player 0 score: " << g0.getNumBricks(0) << "; Player 1 score: " << g0.getNumBricks(1) << endl;
        
        Game g1(0, standardPlayer);
        while(!g1.isOver()){
            g1.print();
            g1.makeHalfMove();
        }
        cout << "Game over. Player 0 score: " << g1.getNumBricks(0) << "; Player 1 score: " << g1.getNumBricks(1) << endl;
    }
    
    else if(runmode == standardVSevolved){
        // Standard player and evolution winner play against each other
        Evolution e;
        Strategy* AA = e.getWinner();
        Strategy* standardPlayer = new Strategy(0);
        
        cout << "\nGame 0: Evolved moves first and standard player moves second.\n";
        Game g0(AA, standardPlayer);
        while(!g0.isOver()){
            g0.print();
            g0.makeHalfMove();
        }
        cout << "Game over. Player 0 score: " << g0.getNumBricks(0) << "; Player 1 score: " << g0.getNumBricks(1) << endl;
        
        cout << "\nGame 1: Standard player moves first and evolved moves second.\n";
        Game g1(standardPlayer, AA);
        while(!g1.isOver()){
            g1.print();
            g1.makeHalfMove();
        }
        cout << "Game over. Player 0 score: " << g1.getNumBricks(0) << "; Player 1 score: " << g1.getNumBricks(1) << endl << endl;
        if(g0.getNumBricks(0) > g0.getNumBricks(1))
            cout << "Evolved player played first and won against the standard player in the first game.\n";
        else if(g0.getNumBricks(0) < g0.getNumBricks(1))
            cout << "Evolved player played first and lost to the standard player in the first game.\n";
        else
            cout << "Evolved player played first and tied with the standard player in the first game.\n";
        
        if(g1.getNumBricks(0) > g1.getNumBricks(1))
            cout << "Standard player played first and won against the evolved player in the second game.\n";
        else if(g1.getNumBricks(0) < g1.getNumBricks(1))
            cout << "Standard player played first and lost against the evolved player in the second game.\n";
        else
            cout << "Standard player played first and tied with the evolved player in the second game.\n";

    }
    

    cout << "\nAA average CPU time per move = " << Game::getTiming() << " microseconds" << endl;
    cout << "based upon " << Game::getTotAAmoves() << " moves."<< endl << endl;
}
