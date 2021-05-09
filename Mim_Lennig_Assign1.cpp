//
//  Mim_Lennig_Assign1.cpp
//  Assignment1
//
//  Created by Miriam Lennig on 2018-01-20.
//  Copyright © 2018 Miriam Lennig. All rights reserved.
//

#include "Mim_Lennig_Assign1_classes.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>

using namespace std;

int main(int argc, const char * argv[]) {
    //srand (unsigned (time(NULL)));
    
    // First run a set of test cases on the diversity function
    Evolution::testDiversity();    
    cout << endl;
    
    ifstream prefixesFile("vaskenTestCases");
    string prefix;
    
    while(getline(prefixesFile, prefix)){
        string guests = prefix + ".csv";
        string settings = prefix + "settings.txt";
        
        cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
        cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
        cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
        cout << "Vasken test case: " << guests << endl;
        
        Arrangement::readSettingsAndGuests(settings, guests);
        
        Evolution x;
    }
    
}
