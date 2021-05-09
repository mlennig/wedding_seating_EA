//
//  Mim_Lennig_Assign1_classes.hpp
//  Assignment1
//
//  Created by Miriam Lennig on 2018-01-20.
//  Copyright © 2018 Miriam Lennig. All rights reserved.
//

#ifndef Mim_Lennig_Assign1_classes_hpp
#define Mim_Lennig_Assign1_classes_hpp

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <deque>
using namespace std;

const int POPSIZE = 1000;
const int MAXNUMGUESTS = 3000;
const int WINDOWSIZE = 5;     // Window size for parent tournament selection
const int MAXITERATIONS = 1000;
const int NUMFITNESSES = 20;
const bool USECROWDING = true;

class Table : public vector<int>{
public:
    int countEmptySeats() const;
    void callPush_Back(int);
    void normalize();
    void print();
    void printNoEndl();
    bool nextToAtThisTable(int, int);
    bool bothAtTable(int, int);
    int guestIdOnRight(int);
    int guestIdOnLeft(int);
    Table(const Table&);
    Table();
    //~Table();
};

class Pair{
public:
    int x;
    int y;
};

class Arrangement;
static int preferences[MAXNUMGUESTS][MAXNUMGUESTS];            // TODO: check at beginning of program
bool isLessThan(Table*, Table*);
bool isMoreFit(Arrangement*, Arrangement*);

// One arrangement is one member of the population
// One arrangement holds nTables,
// each table has a set of guests.
// In the context of the Arrangement class
// "index" refers to the linearized seat position.
class Arrangement{
    static int nGuests;
    static int nEmptySeats;
    static vector<string> guestNames;

    static int nTables;
    void pmxHelper(Arrangement&, Arrangement&, int, int);   // Swap segment between cuts between two arrangements and swap guests according to links
    void swapGuest(Arrangement*, Arrangement*, int);
    int indexOfGuest(int);                                  // Returns linearized index of guest in Arrangement
    int guestIDatIndex(int) const;                                // Return guest ID or empty seat
    void distinguishEmptySeats();
    void revertEmptySeats();
    void setGuestAtIndex(int, int);
    
public:
    vector<Table*> tables;
    
    int tableForGuest(int) const;
    static void initializeSettings(int, int);
    bool isSittingNear(int, int) const;
    bool isSittingNextTo(int, int) const;
    void normalize();
    int guest2LeftOfGuest(int) const;
    int guest2RightOfGuest(int) const;
    int diversity(const Arrangement&);
    string getGuestNameAtID(int);                       // Return guest name with guest ID
    vector<Arrangement*> pmx(Arrangement*, Arrangement*);      // Partially Mapped Crossover method
    Arrangement* mutate(Arrangement*);                  // Mutation method
    void print();
    void printLinearized();
    int fitness();
    bool operator==(const Arrangement&);
    Arrangement();
    Arrangement(const Arrangement&);
    Arrangement(const vector<int>);
    void static readSettingsAndGuests(string, string);
    bool isArrangementLegal();
    ~Arrangement();
    
};

class Evolution{
    vector<Arrangement*> parents;
    vector<Arrangement*> fittestArrangements;
    deque<int> lowestFitnesses;
    
    void evolutionHelper();

public:
    static vector<Arrangement*> population;
    
    void deleteFromPopulation(Arrangement*);
    void crowdingHelper(Arrangement*, int, bool&, bool&);
    void crowding(Arrangement*, Arrangement*, int);
    static void testDiversity();
    int diversity();
    bool shouldTerminate();
    static bool populationContainsDuplicate();
    Arrangement* getResult();
    static bool matchesMemberOfPopulation(Arrangement&);
    Evolution();
    ~Evolution();
};




#endif /* Mim_Lennig_Assign1_classes_hpp */
