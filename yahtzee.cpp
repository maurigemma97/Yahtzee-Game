#include <iostream>

#include "yahtzee_utils.cpp"

using namespace std;

int main(){

    srand(time(0));
    
    Game yahtzee;
    yahtzee.play();

    return 0;
}
