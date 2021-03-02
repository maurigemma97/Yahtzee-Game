#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <functional>
#include <numeric>
#include <vector>

#include <stdlib.h>
#include <time.h>


//Unicode from here: 
//  https://unicode-table.com/en/blocks/enclosed-alphanumerics/

//Note:
// Conforms to Wu-Yahtzee

//TODO
//  FIX scoring - or update functionality
//  remove couts that where for debugging
//  improve
//  annotate


//As a player
// I want to quit at any time
// I want the best option highlighted
// I want to see my totals, upper lower and bonuses seperately
// see my scorecard every round

using namespace std;

typedef unsigned int uint;

class Die{

    private:
        uint value = 0;

    public:

        Die(){
        }

        void roll(){

            //help from:
            //  http://www.cplusplus.com/reference/random/uniform_int_distribution/
            //  https://cboard.cprogramming.com/c-programming/71067-getting-random-number-between-1-6-a.html
            
            // range 1-6
            //this->value = (rand() + 1) % 7;
            this->value = rand() % 6 + 1;
        }

        int get_value(){
            return this->value;
        }

        friend ostream& operator<<(ostream &os, Die &die){

            // help from:
            //   https://www.geeksforgeeks.org/overloading-stream-insertion-operators-c/

            if(die.value == 0) die.roll();

            switch(die.value){
                case 1:
                    os << "⚀";
                    break;
                case 2:
                    os << "⚁";
                    break;
                case 3:
                    os << "⚂";
                    break;
                case 4:
                    os << "⚃";
                    break;
                case 5:
                    os << "⚄";
                    break;
                case 6:
                    os << "⚅";
                    break;
            }

            return os;
        }

};

//-----------------------------------------------
class Dice{

    private:

        vector<Die> dice;
        uint number;

    public:

        Dice(int number){

            this->number = number;
            
            for(int i=0; i<number; i++){
                Die d;
                d.roll();
                this->dice.push_back(d);
            }
        }

        void roll_dice(){
            for_each(dice.begin(), dice.end(), [&](Die &d){d.roll();});
        }

        friend ostream& operator<<(ostream &os, Dice &dice){
            
            for_each(dice.dice.cbegin(), dice.dice.cend(), [&](Die d){os << d << " ";});
            return os;
        }

        vector<Die> get_dice(){
            return this->dice;
        }

        vector<uint> values(){

            //the integer value of each roll
            // ie [5, 4, 4, 2, 6, 1]

            vector<uint> vals;

            for(uint i=0; i<this->number; i++){
                vals.push_back(this->dice[i].get_value());
            }

            return vals;
        }

        array<uint, 6> counts(){

            // the number of 1s, 2s, ...       1  2  3  4  5  6
            // so if the roll is 3 2 1 1 1 -> [3, 1, 1, 0, 0, 0]

            array<uint, 6> counts;
            vector<uint> values = this->values();

            fill(counts.begin(), counts.end(), 0);

            for(uint i: values){
                counts[i-1] += 1;
            }

            return counts;
        }

};

//-----------------------------------------------
class ScoreCard{

    //NOTE:
    //keeps current number of dice, ie
    //   [2, 0, 0, 1, 0] ie 2 '1s', 1 '4'
    //*****Scorecard does not count number of rolls!!!*****

    private:

        //sections -> aces, twos, threes, fours, fives, sixes, 3-kind, 4-kind, full-house, sm-straight, yahtzee, straight

        array<bool, 13> sections;               // the upper and lower sections
        array<uint, 13> score_sections;         // the score by section
        //array<uint, 6>  dice_counts;            // the current number die count, running total

        //calculate scores-----------------------
        int upper_score(){

            // without bonus
            
            uint score = 0;
        
            for(uint i=0; i<6; i++){
                //       t/f            1-6    count
                score += this->score_sections[i];
            }
            if(score >= 63)
            {
                score += 35;
            }
            return score;
        }
        
        int lower_score(){

            int score = 0;

            //the lower section -> 3 of a kind up
            for(uint ls=6; ls<13; ls++){
                
                //if it hasn't been claimed don't bother
                if(!this->sections[ls]) continue;

                switch(ls){
                    case 6:
                        score += this->score_sections[ls];
                        break;
                    case 7:
                        score += this->score_sections[ls];
                        break;
                    case 8:
                        score += this->score_sections[ls];
                        break;
                    case 9:
                        score += this->score_sections[ls];
                        break;
                    case 10:
                        score += this->score_sections[ls];
                        break;
                    case 11:
                        score += this->score_sections[ls];
                        break;
                    case 12:
                        score += this->score_sections[ls];
                        break;
                }
            }
            return score;
        }

        //---------------------------------------

        public:

        ScoreCard(){

            //fill(this->dice_counts.begin(), this->dice_counts.end(), 0);
            fill(this->sections.begin(), this->sections.end(), false);
            fill(this->score_sections.begin(), this->score_sections.end(), 0);
        }

        bool has_at_least_1(array<uint, 6> &counts, int kind){
            return counts[kind - 1] >= 1;
        }

        bool has_n_of(array<uint, 6> &counts, int kind, int n_of){
            return counts[kind - 1] == n_of;
        }

        bool has_1kind(array<uint, 6> &counts){
            //array<int, 6> counts = roll.counts();
            return any_of(counts.cbegin(), counts.cend(), [&](int c){return c>=1;});
        }

        bool has_pair(array<uint, 6> &counts){
            //array<int, 6> counts = roll.counts();
            return any_of(counts.cbegin(), counts.cend(), [&](int c){return c==2;});
        }

        bool has_3kind(array<uint, 6> &counts){
            //array<int, 6> counts = roll.counts();
            return any_of(counts.cbegin(), counts.cend(), [&](int c){return c==3;});
        }

        bool has_4kind(array<uint, 6> &counts){
            //array<int, 6> counts = roll.counts();
            return any_of(counts.cbegin(), counts.cend(), [&](int c){return c==4;});
        }

        bool has_fullhouse(array<uint, 6> &counts){
            //array<int, 6> counts = roll.counts();
            bool a = has_3kind(counts);
            bool b = has_pair(counts);
            return a && b;
        }

        bool has_sm_straight(array<uint, 6> &counts){
            // small straight - sequence of 4
            // has to be 0-4 or 1-5
            bool a = all_of(counts.cbegin(), counts.cend() - 1, [&](uint c){return c >= 1;}); //caught this to handle chance of a duplicate number with small straight
            bool b = all_of(counts.cbegin() + 1, counts.cend(), [&](uint c){return c >= 1;});
            return a || b;
        }

        bool has_lg_straight(array<uint, 6> &counts){
            //array<int, 6> counts = roll.counts();
            return all_of(counts.cbegin(), counts.cend(), [&](uint c){return c == 1;});
        }

        bool has_yahtzee(array<uint, 6> &counts){
            //array<int, 6> counts = roll.counts();
            return all_of(counts.cbegin(), counts.cend(), [&](uint c){return c==1;});
        }

        bool is_section_claimed(uint index){
            return this->sections[index];
        }

        array<uint, 14> all_scores(array<uint, 6> &counts){

            //Array has an extra element -- it is the max!

            array<uint, 14> scores;
            uint total = total = accumulate(counts.begin(), counts.end(), 0, plus<uint>());

            scores[0] = counts[0];
            scores[1] = counts[1] * 2;
            scores[2] = counts[2] * 3;
            scores[3] = counts[3] * 4;
            scores[4] = counts[4] * 5;
            scores[5] = counts[5] * 6;
            scores[6] = this->has_3kind(counts) ? total: 0;
            scores[7] = this->has_4kind(counts) ? total: 0;
            scores[8] = this->has_fullhouse(counts) ? 25: 0;
            scores[9] = this->has_sm_straight(counts) ? 30: 0;
            scores[10] = this->has_lg_straight(counts) ? 40: 0;
            scores[11] = this->has_yahtzee(counts) ? 50: 0;
            scores[12] = total;

            //find the index of the maximum
            uint index = 0;

            for(uint i=0; i<13; i++){
                index = scores[index] > scores[i]? index : i;
            }

            scores[13] = index;

            return scores;   
        }
        void show_score(){
            cout << "\n";
            cout << "-----------ScoreCard-----------" <<endl;
            for(uint section = 0; section < score_sections.size(); section++){
                switch(section){
                case 0:
                    cout << "\n";
                    cout << "--Upper Section--" << endl;
                    cout << "Aces Score:           " << score_sections[section] << " Points" << endl;
                    break;
                case 1:
                    cout << "Twos Score:           " << score_sections[section] << " Points" << endl;
                    break;
                case 2:
                    cout << "Threes Score:         " << score_sections[section] << " Points" << endl;
                    break;
                case 3:
                    cout << "Fours Score:          " << score_sections[section] << " Points" << endl;
                    break;
                case 4:
                    cout << "Fives Score:          " << score_sections[section] << " Points" << endl;
                    break;
                case 5:
                    cout << "Sixes Score:          " << score_sections[section] << " Points" << endl;
                    cout << "-Total Upper Score-   " << upper_score() << " Points" << endl;
                    cout << "\n";
                    break;
                case 6:
                    cout << "--Lower Section--" << endl;
                    cout << "\"3-Kind\" Score:       " << score_sections[section] << " Points" << endl;
                    break;
                case 7:
                    cout << "\"4-Kind\" Score:       " << score_sections[section] << " Points" << endl;
                    break;
                case 8:
                    cout << "Full-House Score:     " << score_sections[section] << " Points" << endl;
                    break;
                case 9:
                    cout << "Small Straight Score: " << score_sections[section] << " Points" << endl;
                    break;
                case 10:
                    cout << "Large Straight Score: " << score_sections[section] << " Points" << endl;
                    break;
                case 11:
                    cout << "YAHTZEE Score:        " << score_sections[section] << " Points" << endl;
                    break;
                case 12:
                    cout << "Chance Score:         " << score_sections[section] << " Points" << endl;
                    cout << "-Total Lower Score-   " << lower_score() << " Points" << endl;
                    cout << "\n" << endl;
                    cout << "--Grand Total--       " << upper_score()+lower_score() << " Points" << endl;
                    break;
                }
            }
            cout << "\n";
        }
        void update_claim_and_score(uint section, uint total){

            //update the boolean flags
            this->sections[section] = true;
            //update the score
            this->score_sections[section] = total;
            
            // for debugging
            //cout << "Section: " << section+1 << endl;
            //cout << "is flagged " << this->sections[section] << endl;
            //cout << "points " << this->score_sections[section] << endl;

        }

        //show helper functions------------------
        string is_claimed(uint index){
            return this->sections[index] ? "✓" : " ";
        }

        string score_of(uint index){
            string score = to_string(this->score_sections[index]);
            return score;
        }

        friend ostream& operator<<(ostream &os, ScoreCard &scorecard){
        
            string GAP = "    ";

            os << setw(6) << "Aces" << scorecard.is_claimed(0) << setw(3) << scorecard.score_of(0) << GAP;
            os << setw(12) << "3 of a kind" << scorecard.is_claimed(6) << setw(3) << scorecard.score_of(6) << "\n";

            os << setw(6) << "Twos" << scorecard.is_claimed(1) << setw(3) << scorecard.score_of(1) << GAP;
            os << setw(12) << "4 of a kind" << scorecard.is_claimed(7) << setw(3) << scorecard.score_of(7) << "\n";

            os << setw(6) << "Threes" << scorecard.is_claimed(2) << setw(3) << scorecard.score_of(2) << GAP;
            os << setw(12) << "Full House" << scorecard.is_claimed(8) << setw(3) << scorecard.score_of(8) << "\n";

            os << setw(6) << "Fours" << scorecard.is_claimed(3) << setw(3) << scorecard.score_of(3) << GAP;
            os << setw(12) <<  "SM Straight" << scorecard.is_claimed(9) << setw(3) << scorecard.score_of(9) << "\n";

            os << setw(6) << "Fives" << scorecard.is_claimed(4) << setw(3) << scorecard.score_of(4) << GAP;
            os << setw(12) << "LG Straight" << scorecard.is_claimed(10) << setw(3) << scorecard.score_of(10) << "\n";

            os << setw(6) << "Sixes" << scorecard.is_claimed(5) << setw(3) << scorecard.score_of(5) << GAP;
            os << setw(12) <<  "YAHTZEE" << scorecard.is_claimed(11) << setw(3) << scorecard.score_of(11) << "\n";

            os << "              " << setw(12) << "Chance" << scorecard.is_claimed(12) << setw(3) << scorecard.score_of(12) << "\n";

            return os;
        }

        //utilities------------------------------
        bool has_played_all(){
            return all_of(this->sections.cbegin(), this->sections.cend(), [&](bool s){return s == true;});
        }

};

//-----------------------------------------------

class Game{

    private:

        uint current_round = 0;         //ie every player has completed their turn
        bool in_play = true;            //is the game still active
        string player_name;

        ScoreCard scores{};
        Dice dice{5};

    public:

        Game(){
        }

        //show options-------------------------------------

        void show_welcome(){

            string menu;
            
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            cout << "*             Yahtzee                  *\n";
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            cout << "\n";
        }

        void clear_screen(){

            //the hacky, ugly way
            //string clear(100, "\n");
            cout << string(100, '\n');
            cout << endl;
            
        }

        void show_play_options(){

            cout << "\nPlease choose:\n";
            cout << "  ① Select dice to keep " << " ② Roll again\n" << "  ③ End turn " << "            ④ Quit Game";
            cout << endl;
        }

        void show_section_options(array<uint, 6> &counts, array<uint, 14> &scores){

            //fill an array with empty strings, set the maximum to a star
            array<string, 14> max_or_blank;
            string blank{"  "};

            // The problem: max may have already been claimed --> it may not always show a star
            uint max = scores[13];
            uint max_index = 0;

            //max & unclaimed
            if(!this->scores.is_section_claimed(max)){
                max_or_blank[max] = " ★";
            }
            else{
                //find a max that is an unclaimed section
                for(uint i=0; i<13; i++){
                    if(!this->scores.is_section_claimed(i)){
                        max_index = scores[max_index] > scores[i] ? max_index : i;
                    }
                } 
                 //there is still a problem - unclaimed are all 0. Pick the first unclaimed one.
                if(this->scores.is_section_claimed(max_index)){
                   for(uint i=0; i<13; i++){
                       if(!this->scores.is_section_claimed(i)){
                           max_index = i;
                           break;
                       }
                   }
                }

                 max_or_blank[max_index] = " ★";
            }

            cout << "Please select the section you would like to use:\n";
            cout << "Section………Score (best)" << endl;
            
            for(uint section=0; section<13; section++){

                //only show the sections that have not been claimed
                if(this->scores.is_section_claimed(section)) continue;

                switch(section){
                    case 0:
                        cout << "⑴ Aces……………………" << scores[0]  << max_or_blank[0];
                        break;
                    case 1:
                        cout << "⑵ Twos……………………" << scores[1]  << max_or_blank[1];
                        break;
                    case 2:
                        cout << "⑶ Threes………………" << scores[2]  << max_or_blank[2];
                        break;
                    case 3:
                        cout << "⑷ Fours…………………"  << scores[3]  << max_or_blank[3];
                        break;
                    case 4:
                        cout << "⑸ Fives…………………"  << scores[4]  << max_or_blank[4];
                        break;
                    case 5:
                        cout << "⑹ Sixes…………………"  << scores[5]  << max_or_blank[5];  
                        break;
                    case 6:
                        cout << "⑺ 3 of a kind…"  << scores[6]  << max_or_blank[6];
                        break;
                    case 7:
                        cout << "⑻ 4 of a kind…"  << scores[7]  << max_or_blank[7];
                        break;
                    case 8:
                        cout << "⑼ Full House……"  << scores[8]  << max_or_blank[8];
                        break;
                    case 9:
                        cout << "⑽ SM Straight…"  << scores[9]  << max_or_blank[9];
                        break;
                    case 10:
                        cout << "⑾ LG Straight…"  << scores[10]  << max_or_blank[10];
                        break;
                    case 11:
                        cout << "⑿ YAHTZEE……………"  << scores[11]  << max_or_blank[11];
                        break;
                    case 12:
                        cout << "⒀ Chance………………"  << scores[12]  << max_or_blank[12];
                        break;
                }


                cout << endl;
            }
        }

        //get game info------------------------------------
        int get_play_options(){

            uint nchoice;

            cout << "> ";
            cin >> nchoice;

            if(nchoice == 4){
                this->in_play = false;
                return 4;
            }

            cout << endl;

            return nchoice;
        }

        uint get_section_choice(){

            uint nchoice;

            cout << "> ";
            cin >> nchoice;

            cout << endl;

            return nchoice;
        }

        //game-----------------------------------
        void play_round(){

            uint selection;
            bool is_valid = false;
            array<uint, 6> counts;

            cout << "Roll:\n";
            this->dice.roll_dice();
            cout << "\t" << this->dice << endl;

            cout << endl;

            counts = dice.counts();
            cout << "\n" << endl;
            cout << "--Dice Count By Value--" << endl;
            cout << "Ones:   " << counts[0] << endl;
            cout << "Twos:   " << counts[1] << endl;
            cout << "Threes: " << counts[2] << endl;
            cout << "Fours:  " << counts[3] << endl;
            cout << "Fives:  " << counts[4] << endl;
            cout << "Sixes:  " << counts[5] << endl;
            cout << "\n" << endl;

            array<uint, 14> all_scores = this->scores.all_scores(counts);

            while(!is_valid){

                this->show_section_options(counts, all_scores);
                selection = this->get_section_choice() - 1;

                is_valid = !this->scores.is_section_claimed(selection);

                if(!is_valid){
                    cout << "Section has already been taken" << endl;
                }
                
                //for debugging
                //cout << "is valid " << is_valid << endl;
            }

            //the index of the max is the last element of the array
            uint best_score = all_scores[selection];
            scores.update_claim_and_score(selection, best_score);
            scores.show_score();

            return;
        }

        void play(){
            
            this->show_welcome();

            // to quit - it looks at the scorecard - if all slots are full it continues
            //   or checks in play - a catch-all boolean

            while(!this->scores.has_played_all() && this->in_play){
                this->play_round();
            }
        }
};