//
//  Mim_Lennig_Assign1_classes.cpp
//  Assignment1
//
//  Created by Miriam Lennig on 2018-01-20.
//  Copyright © 2018 Miriam Lennig. All rights reserved.
//

#include "Mim_Lennig_Assign1_classes.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <list>
#include <deque>

int tableCapacity = 0;               // Number of seats available at each table
int Arrangement::nGuests = 0;        // Number of guests to be seated
int Arrangement::nTables = 0;        // Number of tables needed
int Arrangement::nEmptySeats = 0;    // How many empty seats we will have

vector<string> Arrangement::guestNames; // List of guest names taken from preferences file

// Check if arrangement already exists in population
bool Evolution::matchesMemberOfPopulation(Arrangement& a){
    for(int i = 0; i < population.size(); i++){
        if(a == *(population[i]))
            return true;
    }
    return false;
}

// Check if two guests are sitting at the same table
bool Arrangement::isSittingNear(int iGuest, int iOtherGuest) const{
    for(int iTable = 0; iTable < tables.size(); iTable++){
        if (tables[iTable]->bothAtTable(iGuest, iOtherGuest))
            return true;
    }
    return false;
};

// Check if two guests are sitting next to one another
bool Arrangement::isSittingNextTo(int iGuest, int iOtherGuest) const{
    for(int iTable = 0; iTable < tables.size(); iTable++){
        if (tables[iTable]->nextToAtThisTable(iGuest, iOtherGuest))
            return true;
    }
    return false;
}

// Calculate fitness of arrangement
int Arrangement::fitness(){
    int penalty = 0;
    for(int iGuest = 0; iGuest < nGuests; iGuest++)
        for(int iOtherGuest = 0; iOtherGuest < nGuests; iOtherGuest++)
            switch (preferences[iGuest][iOtherGuest]){
                case 1:
                    if (isSittingNextTo(iGuest, iOtherGuest))
                        penalty += 15;
                    else if (isSittingNear(iGuest, iOtherGuest))
                        penalty += 10;
                    break;
                case 2:
                    if(isSittingNextTo(iGuest, iOtherGuest))
                        penalty += 15;
                    break;
                case 3:
                case 9:
                    break;
                case 4:
                    if(!isSittingNear(iGuest, iOtherGuest))
                        penalty += 10;
                    break;
                case 5:
                    if (!isSittingNextTo(iGuest, iOtherGuest) && isSittingNear(iGuest, iOtherGuest))
                        penalty += 15;
                    else if (!isSittingNear(iGuest, iOtherGuest) && !isSittingNextTo(iGuest, iOtherGuest))
                        penalty += 20;
                    break;
            }
    return penalty; // fitness = penalty
};

// Find name of guest having ID guestID
string Arrangement::getGuestNameAtID(int guestID){
    return guestNames[guestID];
}

// Test to see if two arrangements are the same
bool Arrangement::operator==(const Arrangement& a){
    for(int index = 0; index < tableCapacity * nTables; index++)
        if (guestIDatIndex(index) != a.guestIDatIndex(index))
            return false;
    return true;
}

// Read settings and guest preferences from files
void Arrangement::readSettingsAndGuests(string settings, string guestPreferences){
    string line;
    ifstream settingsFile (settings);
    
    // Read the settings files to obtain
    // the number of guests and the
    // table capacity.
    if(settingsFile.is_open()){
        getline (settingsFile, line);
        tableCapacity = stoi(line);
        
        getline(settingsFile, line);
        nGuests = stoi(line);
        if(nGuests > MAXNUMGUESTS){
            cout << "The number of guests has exceeded the maximum number of guests allowed. Please change MAXNUMGUESTS in the source code with a greater number and restart. \n";
            exit(1);
        }
        settingsFile.close();
    }
    else{
        cout << "Unable to open file";
        exit(1);
    }
    
    
    unsigned long pStart;
    unsigned long pEnd;
    ifstream guestsFile (guestPreferences);
    
    // Read preferences from the guests file
    if(guestsFile.is_open()){
        for(int iRow = 0; iRow <= nGuests; iRow++){
            getline (guestsFile, line);
            pStart = 0;
            
            if(iRow == 0)
                continue;   // Skip row 0
            
            for(int iCol = 0; iCol <= nGuests; iCol++){
                pEnd = line.find(",", pStart);
                
                if(pEnd > line.size()){
                    if(line[line.size() - 1] == ','){
                        preferences[iRow - 1][iCol - 1] = 9;
                        break;
                    }
                    else
                        pEnd = line.size();
                }
                
                if(iCol == 0){
                    guestNames.push_back(line.substr(pStart, pEnd - pStart));    // Store guest names
                    pStart = pEnd + 1;
                    continue;   // Skip column 0
                }
                
                if(pStart == pEnd)
                    preferences[iRow - 1][iCol - 1] = 9;    // 9 represents a neutral relationship between a guest and themselves
                else
                    preferences[iRow - 1][iCol - 1] = stoi(line.substr(pStart, pEnd - pStart));
                
                pStart = pEnd + 1;
            }
        }
        
        guestsFile.close();
        
        nTables = ceil(double(nGuests) / tableCapacity);    // Calculate the number of tables needed
        nEmptySeats = tableCapacity * nTables - nGuests;    // Calculate how many empty seats we will have
        
        cout << "Preferences Matrix (9 represents missing data): \n";
        for(int iRow = 0; iRow < nGuests; iRow++){
            for(int iCol = 0; iCol < nGuests; iCol++){
                cout << preferences[iRow][iCol] << "\t";
            }
            cout << endl;
        }
    }
    else{
        cout << "Unable to open file";
        exit(1);
    }
}

// Arrangement default constructor.
// Initialize a random arrangement.
Arrangement::Arrangement(){
    // If files have not already been read
    // read in number of guests, number of seats
    // at each table, and the preferences of
    // the guests. Only execute this block of code
    // the first time an arrangement is constructed.
    // The data is stored in static variables, so it only
    // needs to be read once.
    
    vector<int> tempContainer;
    
    if(nGuests == 0){
        readSettingsAndGuests("settings.txt", "guests.csv");
    }
    
    for(int i = 0; i < nGuests; i++)
        tempContainer.push_back(i);
    
    for(int i = 0; i < nEmptySeats; i++)
        tempContainer.push_back(-1);
    
    for(int i = 0; i < 1000 * nGuests; i++){
        swap(tempContainer[rand() % tempContainer.size()], tempContainer[rand() % tempContainer.size()]);
    };
    
    for (int i = 0; i < nTables; i++)
        tables.push_back(new Table);
    
    for (int i = 0; i < nTables; i++){
        for(int k = 0; k < tableCapacity; k++){
            tables[i]->callPush_Back(tempContainer[i * tableCapacity + k]);
        };
        
        tables[i]->normalize(); // Normalize the table
    }
    
    normalize();    // Normalize the arrangment
};

// Normalize the sequence of tables so that
// the table with the lowest element is at
// position 0
void Arrangement::normalize(){
    sort(tables.begin(), tables.end(), isLessThan);
}

// Print the arrangement as (numTables x numSeatsPerTable) matrix
// where the entry at row iTable and column iSeat is the guest id
// of person sitting at table iTable and seat iSeat.
// Empty seats are denoted -1.
void Arrangement::print(){
    cout << "The arrangement (-1 indicates an empty seat): \n";
    for(int i = 0; i < tables.size(); i++)
        tables[i]->print();
}

// Print out the arrangement without as a linear
// list of guestIDs. Without showing where
// the table boundaries are.
void Arrangement::printLinearized(){
    for(int i = 0; i < tables.size(); i++)
        tables[i]->printNoEndl();
    cout << "Fitness: " << fitness() << endl;
}

// Find the guestID to the right of the guest
// sitting at iSeat.
int Table::guestIdOnRight(int iSeat){
    if(iSeat == 0)
        return this->at(int(this->size() - 1));
    return this->at(iSeat - 1);
}

// Find the guestID to the left of the guest
// sitting at iSeat.
int Table::guestIdOnLeft(int iSeat){
    if(iSeat == this->size() - 1)
        return this->at(0);
    return this->at(iSeat + 1);
}

// Find the ID of the guest sitting to the left of guestID
int Arrangement::guest2LeftOfGuest(int guestID) const{
    int theTable = -1;
    int theSeat = -1;
    
    for(int iTable = 0; iTable < nTables; iTable++)
        for(int iSeat = 0; iSeat < tableCapacity; iSeat++){
            if(tables[iTable]->at(iSeat) == guestID){
                theTable = iTable;
                theSeat = iSeat;
                break;
            }
        }
    if(theTable < 0)
        return -1;
    else
        return tables[theTable]->guestIdOnLeft(theSeat);
}

// Find the ID of the guest sitting to the right of guestID
int Arrangement::guest2RightOfGuest(int guestID) const{
    int theTable = -1;
    int theSeat = -1;
    
    for(int iTable = 0; iTable < nTables; iTable++)
        for(int iSeat = 0; iSeat < tableCapacity; iSeat++){
            if(tables[iTable]->at(iSeat) == guestID){
                theTable = iTable;
                theSeat = iSeat;
                break;
            }
        }
    if(theTable < 0)
        return -1;
    else
        return tables[theTable]->guestIdOnRight(theSeat);
}

// Find which table number guestID is sitting at.
int Arrangement::tableForGuest(int guestID) const{
    for(int iTable = 0; iTable < nTables; iTable++)
        for(int iSeat = 0; iSeat < tableCapacity; iSeat++){
            if(tables[iTable]->at(iSeat) == guestID){
                return iTable;
            }
        }
    return -1;
}

// Calculate genomic distance between two arrangements
int Arrangement::diversity(const Arrangement& b){
    Arrangement& a = *this;
    enum Position {Right, Left};
    int diversity = 0;
    bool seatingMatched = false;
    bool positionMatched = Right;
    
    for(int theGuest = 0; theGuest < nGuests; theGuest++){
        int gLa = a.guest2LeftOfGuest(theGuest);
        int gRa = a.guest2RightOfGuest(theGuest);
        int gLb = b.guest2LeftOfGuest(theGuest);
        int gRb = b.guest2RightOfGuest(theGuest);
        
        // Is the guest who's sitting to the right
        // of theGuest in arrangement a also sitting
        // next to theGuest in arrangement b
        // (on either side).
        if(!b.isSittingNextTo(theGuest, gRa))
            diversity++;
        else{
            seatingMatched = true;
            // Is the guest who's sitting to the right of
            // of the guest in arrangement a sitting
            // to the right or the left of the guest
            // in arrangement b.
            if(gRa == gRb)
                positionMatched = Right;
            else
                positionMatched = Left;
        }
        
        if(!seatingMatched){
            if(!b.isSittingNextTo(theGuest, gLa))
                diversity++;
        }
        else{
            if((!positionMatched) && (gLa != gLb))
                diversity++;
            else if (positionMatched && (gLa != gRb))
                diversity++;
        }
        
        int temp = 0;
        int iTable = a.tableForGuest(theGuest);
        int jTable = b.tableForGuest(theGuest);
        
        // For each guest at theGuests's table in arrangement a
        // check to see if that guest is sitting at theGuests's
        // table in arrangement b. If so, increment temp.
        for(int iSeat = 0; iSeat < tableCapacity; iSeat ++){
            int guestIniSeat = a.tables[iTable]->at(iSeat);
            if((guestIniSeat == theGuest) || (guestIniSeat == -1))
                continue;
            if(b.isSittingNear(theGuest, guestIniSeat))
               temp++;
        }
        
        int minEmptySeatCount = min(a.tables[iTable]->countEmptySeats(), b.tables[jTable]->countEmptySeats());
        temp += minEmptySeatCount;
        
        int diversityBonus = tableCapacity - 1 - temp;
        
        diversity += diversityBonus;
    }
    return diversity;
}

// Use Partially Mapped Crossover method to generate 2 children from 2 parents
vector<Arrangement*> Arrangement::pmx(Arrangement* parent1, Arrangement* parent2){
    vector<Arrangement*>* crossoverChildren = new vector<Arrangement*>;
    
    // Copy both parents. The children start off as
    // exact copies of their parents and get modified
    // later.
    Arrangement* child1 = new Arrangement(*parent1);
    Arrangement* child2 = new Arrangement(*parent2);
    


    // Choose two random cut locations, where cut0
    // at a lower index than cut 1.
    bool suitableCutsFound = false;
    int cut0 = 0;
    int cut1 = 0;
    
    
    // Find suitable cuts such that the guests between
    // the two cuts are nonmatching across the children.
    while(!suitableCutsFound){
        cut0 = rand() % (tableCapacity * nTables);
        cut1 = rand() % (tableCapacity * nTables);
        
        if(cut0 > cut1){
            int temp = cut0;
            cut0 = cut1;
            cut1 = temp;
        }
        
        for(int i = cut0; i <= cut1; i++){
            if(child1->guestIDatIndex(i) != child2->guestIDatIndex(i)){
                suitableCutsFound = true;
                break;
            }
        }
    }
    
    pmxHelper(*child1, *child2, cut0, cut1);
    
    // Check if child already exists in population pool.
    // If it already exists, delete the child and return
    // 0 for that element.
    if(Evolution::matchesMemberOfPopulation(*child1)){
        delete child1;
        crossoverChildren->push_back(0);
    }
    else
        crossoverChildren->push_back(child1);
    
    if(Evolution::matchesMemberOfPopulation(*child2)){
        delete child2;
        crossoverChildren->push_back(0);
    }
    else
        crossoverChildren->push_back(child2);
    
    return *crossoverChildren;
}

// Table copy constructor
Table::Table(const Table& t){
    for(int i = 0; i < tableCapacity; i++){
        this->push_back(t[i]);
    }
}

// Arrangement copy constructor
Arrangement::Arrangement(const Arrangement& arrangement){
    for(int i = 0; i < nTables; i++){
        tables.push_back(new Table(*(arrangement.tables[i])));
    }
}

// Arrangement from a vector of ints
Arrangement::Arrangement(const vector<int> v):Arrangement(){
    if(nGuests == 0){
        readSettingsAndGuests("settings.txt", "guests.csv");
    }
    
    if(nTables * tableCapacity != v.size()){
        cout << "nTables * tableCapacity is not the same size as the vector" << endl;
        return;
    }
    
    for(int i = 0; i < nTables * tableCapacity; i++){
        setGuestAtIndex(i, v[i]);
    }
}

// Return guestID located at linearized index
int Arrangement::guestIDatIndex(int index) const{
    int iTable = index / tableCapacity;
    int iSeat = index % tableCapacity;
    return tables[iTable]->at(iSeat);
}

// Return the linearized index of a guestID
int Arrangement::indexOfGuest(int guestID){
    for(int i = 0; i < tableCapacity * nTables; i++)
        if(guestIDatIndex(i) == guestID)
            return i;
    return -1;  // Return -1 if a guest is missing. This is an error.
}

// Set a seat to a specific guestID using the linearized index
// into the arrangement
void Arrangement::setGuestAtIndex(int index, int guestID){
    int iTable = index / tableCapacity;
    int iSeat = index % tableCapacity;
    tables[iTable]->at(iSeat) = guestID;
}

// Set all empty seats to hold a distinct
// negative value
void Arrangement::distinguishEmptySeats(){
    int emptySeatCounter = -1;
    
    for(int i = 0; i < tableCapacity * nTables; i++){
       if(guestIDatIndex(i) < 0)
           setGuestAtIndex(i, emptySeatCounter--);
    }
}

// Set all empty seats to -1
void Arrangement::revertEmptySeats(){
    for(int i = 0; i < tableCapacity * nTables; i++){
        if(guestIDatIndex(i) < 0)
            setGuestAtIndex(i, -1);
    }
}

// Used for debugging.
// Checks whether there are duplicate guests in the arrangement.
// Checks whether any guest has been omitted from the arrangmenet.
bool Arrangement::isArrangementLegal(){
    for(int i = 0; i < tableCapacity * nTables; i++)
        for(int k = 0; k < tableCapacity * nTables; k++)
            if((guestIDatIndex(i) == guestIDatIndex(k)) && (i != k) && (guestIDatIndex(i) > -1))
                return false;
    
    for(int iGuest = 0; iGuest < nGuests; iGuest++)
        if(indexOfGuest(iGuest) < 0)
            return false;
    
    return true;
}

// Swap segment of guests between cuts between two arrangements
void Arrangement::pmxHelper(Arrangement& child1, Arrangement& child2, int cut0, int cut1){
   
    vector<Pair*> links;
    bool restartRequired;
    bool finishedComputingLinks = false;
    
    child1.distinguishEmptySeats();
    child2.distinguishEmptySeats();
 
    // Identify and store links between
    // arrangements within the segment cut0
    // to cut 1.
    for(int i = cut0; i <= cut1; i++){
        Pair* link = new Pair;
        link->x = child2.guestIDatIndex(i);
        link->y = child1.guestIDatIndex(i);
        links.push_back(link);
    }
    
    for(long i = links.size() - 1; i >= 0; i--){
        if(links[i]->x == links[i]->y){
            delete links[i];
            links.erase(links.begin() + i);
        }
    }
    
    // Identify cycles and generate new
    // link pairs.
    while(!finishedComputingLinks){
        restartRequired = false;
        for(int k = 0; k < links.size(); k++){
            for(int j = 0; j < links.size(); j++){
                if(k == j)
                    continue;
                if(links[k]->x == links[j]->y){
                    Pair* newLink = new Pair;
                    newLink->x = links[j]->x;
                    newLink->y = links[k]->y;
                    restartRequired = true;    // New link generated. Restart of the algorithm will be required.
                    
                    // Erase redundant links, being careful
                    // to erase the one with the higher
                    // index first.
                    if(k < j){
                        delete links[j];
                        links.erase(links.begin() + j);
                        delete links[k];
                        links.erase(links.begin() + k);
                    }
                    else if(k > j){
                        delete links[k];
                        links.erase(links.begin() + k);
                        delete links[j];
                        links.erase(links.begin() + j);
                    }
                    
                    // Exclude self-links of the form (z,z)
                    if(newLink->x != newLink->y){
                        links.push_back(newLink);
                        break;
                    }
                    else
                        delete newLink;
                }
            }
            if(restartRequired)
                break;
        }
        if(!restartRequired)
            finishedComputingLinks = true;
    }
    
    
    // Use links to identify which guests outside of the
    // segment to swap.
    for(int i = 0; i < links.size(); i++){
        child1.setGuestAtIndex(child1.indexOfGuest(links[i]->x), links[i]->y);
        child2.setGuestAtIndex(child2.indexOfGuest(links[i]->y), links[i]->x);
    }
    
    // Swap segments of guests between arrangements
    for(int i = cut0; i <= cut1; i++){
        swapGuest(&child1, &child2, i);
    }
    
    // Reset all empty seats to constant -1.
    child1.revertEmptySeats();
    child2.revertEmptySeats();
    
}

// Swap a pair of guests who are sitting
// in the same position between two arrangements
void Arrangement::swapGuest(Arrangement* child1, Arrangement* child2, int position){
    int iTable = position / tableCapacity;
    int iSeat = position % tableCapacity;
    int temp;
    
    temp =  child1->tables[iTable]->at(iSeat);
    child1->tables[iTable]->at(iSeat) = child2->tables[iTable]->at(iSeat);
    child2->tables[iTable]->at(iSeat) = temp;
}

Arrangement::~Arrangement(){
    // Need to delete each of the tables in tables
    for (int i = 0; i < tables.size(); i++)
        delete tables[i];
}

void Table::print(){
    printNoEndl();
    cout << endl;
}

void Table::printNoEndl(){
    for(int i = 0; i < tableCapacity; i++)
        cout << this->at(i) << "\t";
}

// Add a guest to a table
void Table::callPush_Back(int guest){
    push_back(guest);
}

// Normalize the arrangement of guests around each table
// so that the lowest numbered guest is at position 0
// and that the guest on the left of the lowest numbered guest
// is a lower numbered guest than the guest on the right
// of the lowest numbered guest
void Table::normalize(){
    if(this->at(0) == -1){
        int j = 1;
        while(this->at(j) < 0)
            j++;
        int temp = this->at(0);
        this->at(0) = this->at(j);
        this->at(j) = temp;
    }
    
    int ilowestElement = 0;
    for (int i = 0; i < this->size(); i++){
        if ((this->at(i) < this->at(ilowestElement)) && (this->at(i) != -1)){
            ilowestElement = i;
        }
    }
    rotate(this->begin(), this->begin() + ilowestElement, this->end());
    
    // Check to the right & left of the lowest element.
    // The person at the highest index of the table vector is the person to the right of
    // the lowest element.
    // The person at seating position i + 1 is the person to the left of the person at seating position i.
    if((this->at(this->size() - 1) < this->at(1)) && (this->at(this->size() - 1) != -1)){
        reverse(this->begin(), this->end());
        rotate(this->begin(), this->begin() + this->size() - 1, this->end());
    }
}

// Compare 0th element of two tables
bool isLessThan(Table* table1, Table* table2){
    return (table1[0] < table2[0]);
}

// Compare the fitnesses of two arrangements
bool isMoreFit(Arrangement* a1, Arrangement* a2){
    return (a1->fitness() < a2->fitness());
}

// Check if theGuest and theOtherGuest are next to each other
// at this table.
bool Table::nextToAtThisTable(int theGuest, int theOtherGuest){
    if (!bothAtTable(theGuest, theOtherGuest))
        return false;
    
    int iGuestSeat = -1;
    for(int iSeat = 0; iSeat < tableCapacity; iSeat++){
        if(this->at(iSeat) == theGuest){
            iGuestSeat = iSeat;
            break;
        }
    }
    
    return this->at((iGuestSeat + 1) % tableCapacity) == theOtherGuest || this->at((iGuestSeat - 1 + tableCapacity) % tableCapacity) == theOtherGuest;
}
                    
// Evaluates whether or not both guests in question
// are at this table.
bool Table::bothAtTable(int theGuest, int theOtherGuest){
    bool theGuestIsAtTable = 0;
    bool theOtherGuestIsAtTable = 0;
    
    for(int iSeat = 0; iSeat < tableCapacity; iSeat++){
        if(this->at(iSeat) == theGuest)
            theGuestIsAtTable = 1;
        if(this->at(iSeat) == theOtherGuest)
            theOtherGuestIsAtTable = 1;
    }
  
    return theGuestIsAtTable && theOtherGuestIsAtTable;
}
                    
Table::Table(){
    
}

// Count the number of empty seats at this table
int Table::countEmptySeats() const{
    int counter = 0;
    
    for(int iSeat = 0; iSeat < tableCapacity; iSeat ++)
        if(this->at(iSeat) == -1)
            counter++;
    return counter;
}

void Evolution::evolutionHelper(){
    Arrangement* tempBest;
    int tempBestBasePlusOffset = 0;
    
    // Ensure we have an even number of tournaments
    // so that we end up with an even number of parents.
    // loopUpperBound is the upper bound of the tournament
    // loop.
    int loopUpperBound = POPSIZE - (POPSIZE % WINDOWSIZE);
    
    // Check if WINDOWSIZE divides loopUpperBound an
    // even number of times or an odd number of
    // times. If odd, subtract WINDOWSIZE to guarantee
    // there will always be an even number of
    // parents.
    if((loopUpperBound / WINDOWSIZE) % 2 == 1)
        loopUpperBound -= WINDOWSIZE;
    
    // Use tournament selection to generate parents with tournaments
    // of size WINDOWSIZE.
    for(int base = 0; base < POPSIZE - (POPSIZE % WINDOWSIZE); base += WINDOWSIZE){
        tempBest = population[base];
        tempBestBasePlusOffset = base;
        
        for(int offset = 0; offset < WINDOWSIZE; offset++){
            if(tempBest->fitness() > population[base + offset]->fitness()){
                tempBest = population[base + offset];
                tempBestBasePlusOffset = base + offset;
            }
        }
        
        parents.push_back(population[tempBestBasePlusOffset]);
    }
    
    for(int i = 0; i < parents.size(); i += 2){
        // Generate children using the partially mapped crossover method
        vector<Arrangement*> children = parents[i]->pmx(parents[i], parents[i + 1]);
        
        if(USECROWDING)
            // Use crowding method to hypothetically increase diversity
            crowding(children[0], children[1], i);
        else{
            if(children[0])
                population.push_back(children[0]);
            if(children[1])
                population.push_back(children[1]);
        }
        
    }
    
    // Erase all pointers in parents vector
    parents.clear();
    
    // Sort population pool in ascending order of fitness
    sort(population.begin(), population.end(), isMoreFit);
    
    if(lowestFitnesses.size() > NUMFITNESSES)
        lowestFitnesses.pop_front();
    
    lowestFitnesses.push_back(population[0]->fitness());
    cout << "Lowest fitness = " << population[0]->fitness() << endl;
    
    // Remove arrangements with lowest fitness
    for(unsigned long i = population.size() - 1; i >= POPSIZE; i--){
        population.erase(population.begin() + i);
    }
    
    // Re-randomize the population pool order
    for(int i = 0; i < 1000 * population.size(); i++){
        swap(population[rand() % population.size()], population[rand() % population.size()]);
    }
}

void Evolution::crowdingHelper(Arrangement* child, int iParents, bool& deleteParent1, bool& deleteParent2){
    if(child){
        int d1 = child->diversity(*(parents[iParents]));
        int d2 = child->diversity(*(parents[iParents + 1]));
        
        // Check which parent is genomically closer to child
        if(d1 < d2){
            // Parent1 is genomically closer to child
            // Check if parent1 or child has better fitness
            if(child->fitness() < parents[iParents]->fitness()){
                // Child has better fitness, so child survives.
                population.push_back(child);
                // Plan to kill parent1.
                deleteParent1 = true;
            }
            else
                // Parent1 has better fitness than child.
                // Plan to kill child.
                delete child;
        }
        else if(child->fitness() < parents[iParents + 1]->fitness()){
            // Child has better fitness than parent2, so child survives.
            population.push_back(child);
            // Plan to kill parent2.
            deleteParent2 = true;
        }
        else
            // Parent2 has better fitness than child, so plan to kill child.
            delete child;
    }
}

void Evolution::crowding(Arrangement* child0, Arrangement* child1, int iParents){
    bool deleteParent1 = false;     // Do we need to delete parent1?
    bool deleteParent2 = false;     // Do we need to delete parent2?
    
    crowdingHelper(child0, iParents, deleteParent1, deleteParent2);
    crowdingHelper(child1, iParents, deleteParent1, deleteParent2);
    
    if(deleteParent1)
        deleteFromPopulation(parents[iParents]);
    
    if(deleteParent2)
        deleteFromPopulation(parents[iParents + 1]);
}

// Takes a preferences file and a setting file and runs a genetic
// evolution with specified termination criteria.
Evolution::Evolution(){
    lowestFitnesses.clear();
    
    for(int i = 0; i< NUMFITNESSES; i++)
        lowestFitnesses.push_back(999999999 + i);
    
    // Fill population pool with normalized,
    // unique random arrangements
    while(population.size() < POPSIZE){
        Arrangement* a = new Arrangement;
        
        if(!matchesMemberOfPopulation(*a)){
            population.push_back(a);
        }        
        else
            delete a;
    }
    
    // Iterate up to MAXITERATIONS times or until termination
    // criteria is met.
    for(int i = 0; i < MAXITERATIONS; i++){
        cout << "*****************************************************************************\n";
        cout << "Evolution iteration i = " << i << endl;
        evolutionHelper();
        if(shouldTerminate()){
            break;
        }
    }
    
    // Sort population from best to worst fitness.
    sort(population.begin(), population.end(), isMoreFit);
    
    // Save the arrangements with (equal) best fitnesses
    // into fittestArrangements.
    int k = 1;
    int bestFitness = population[0]->fitness();
    while(bestFitness == population[k]->fitness()){
        fittestArrangements.push_back(population[k]);
        if(k > population.size())
            break;
        k++;
    }
    cout << "Diversity = " << diversity() << endl;
}

// Termination criterion for evolution.
bool Evolution::shouldTerminate(){
    // Terminate when we get NUMFITNESSES arrangements
    // that all have equal (best) fitnesses.
    for(int i = 1; i < lowestFitnesses.size(); i++)
        if(lowestFitnesses[0] != lowestFitnesses[i])
            return false;
    return true;
}

// Check if population contains identical arrangements.
bool Evolution::populationContainsDuplicate(){
    for(int i = 0; i < population.size() - 1; i++){
        for(int k = i + 1; k < population.size(); k++)
            if(*(population[i]) == *(population[k]))
                return true;
    }
    return false;
}

// Calculate the diversity of the arrangements in the fittestArrangements container
int Evolution::diversity(){
    int diversity = 0;
    
    for(int i = 0; i < fittestArrangements.size() - 1; i++)
        for(int j = i + 1; j < fittestArrangements.size(); j++){
            diversity += fittestArrangements[i]->diversity(*(fittestArrangements[j]));
        }
    return diversity;
}

void Arrangement::initializeSettings(int seatsPerTable,int numberOfGuests)
{
    nGuests = numberOfGuests;
    tableCapacity = seatsPerTable;
    nTables = ceil(double(nGuests) / tableCapacity);
    nEmptySeats = tableCapacity * nTables - nGuests;
}

// Delete an arrangement from the population
void Evolution::deleteFromPopulation(Arrangement* a){
    for(int k = 0; k < population.size(); k++)
        if(population[k] == a){
            delete a;
            population.erase(population.begin() + k);
        }
}

// Run diversity test cases
void Evolution::testDiversity(){
    cout << "Running diversity() test cases...\n";
    
    // Test case #1
    // tableCapacity = 4; nGuests = 12
    Arrangement::initializeSettings(4, 12);
    {
        vector<int> v1 = {0,1,2,3,4,5,6,7,8,9,10,11};
        vector<int> v2 = {6,1,2,3,4,10,0,7,8,9,5,11};
        Arrangement a = Arrangement(v1);
        Arrangement b = Arrangement(v2);
        cout << "36 ?= " << a.diversity(b) << endl;
    }
    
    // Test case #2
    Arrangement::initializeSettings(4, 10);
    {
        vector<int> v1 = {0,1,2,3,4,5,6,7,8,9,-1,-1};
        vector<int> v2 = {6,1,2,3,4,-1,0,7,8,9,5,-1};
        Arrangement a = Arrangement(v1);
        Arrangement b = Arrangement(v2);
        cout << "29 ?= " << a.diversity(b) << endl;
    }
    
    // Test case #3
    Arrangement::initializeSettings(4, 10);
    {
        vector<int> v1 = {0,1,2,3,4,5,6,7,8,9,-1,-1};
        vector<int> v2 = {0,1,2,3,4,5,6,7,-1,9,-1,8};
        Arrangement a = Arrangement(v1);
        Arrangement b = Arrangement(v2);
        cout << "2 ?= " << a.diversity(b) << endl;
    }
    
    // Test case #4
    Arrangement::initializeSettings(6, 15);
    {
        vector<int> v1 = {4,3,5,0,1,2,14,11,-1,12,-1,13,8,7,10,-1,9,6};
        vector<int> v2 = {4,-1,0,1,-1,5,14,11,8,-1,13,12,3,2,6,7,9,10};
        Arrangement a = Arrangement(v1);
        Arrangement b = Arrangement(v2);
        cout << "57 ?= " << a.diversity(b) << endl;
    }
    
    Arrangement::initializeSettings(0, 0);
}

vector<Arrangement*> Evolution::population;

Evolution::~Evolution(){
    population.clear();
}










