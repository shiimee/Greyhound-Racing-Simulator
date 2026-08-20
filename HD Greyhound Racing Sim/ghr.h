#pragma once
#include "splashkit.h"
#include "utilities.h"
#include "splashkit-arrays.h"

//---------- DATA STRUCTURES ----------//
struct Dog
{
    string name;                        //< name of dog
    double age;                         //< age of dog (1 to 8) in years
    double speed;                       //< speed of dog (1 to 100)     
    int luck;                           //< luck of dog (1 to 100)
    double win_pct;                     //< win percentage of dog (0 to 100)
    int total_races;                    //< total races of dog (max 150)

    double win_probability;             //< calculated win probability
    double place_probability;           //< calculated place probability
    double win_odds;                    //< calculated odds for win payout
    double place_odds;                  //< calculated odds for place payout
    
    double strength;                    //< calculated strength of dog, used for betting odds calculation
    int position;                       //< ending position of dog after the race
    bool finished = false;              //< switched to true when dog finishes race
};

struct Player
{
    double initial_balance;             //< starting balance
    double balance;                     //< running balance
    dynamic_array<double> bet_amount;   //< amount bet for the round
    dynamic_array<int> bet_type;        //< bet type
    dynamic_array<int> index_dog_bet;   //< index of dog bet
    
    double total_bet = 0.0;             //< total amount of money staked by the player
    double pnl = 0.0;                   //< net profit and loss

    //----- METHODS -----//
    /// @brief reads user input and sets the player's starting balance
    void initialise_balance();

    /// @brief prompts the user to place bets
    void place_bets();
};

struct Race
{
    dynamic_array<Dog> race_dog_data;   //< array of all dog's data in the race
    dynamic_array<sprite> racing_dogs;  //< array of all dog sprites in the race
    dynamic_array<bitmap> dog_bitmaps;  //< array of all dog bitmaps
    
    dynamic_array<int> track_length;    //< track length (400m sprint, 550m standard, 750m stayers)
    int race_number = 1;                //< race number (max 8 races)

    //----- METHODS -----//
    // Stage 1: Waiting Stage ----------
    /// @brief assigns 8 dogs randomly selected from greyhounds.json and assigns them to race_dog_data
    void create_dogs();

    /// @brief initialises the 3 lengths of tracks
    void populate_track_length_array();

    // Stage 2: Betting Stage ----------
    /// @brief calculate all dogs strength's and add them to race_dog_data
    void calculate_strength();

    /// @brief finds highest strength dog in race_dog_data
    /// @return the max strength value
    double find_max_strength();

    /// @brief calculates the win probability of all dogs and adds them to race_dog_data via a softmax function
    void calculate_win_prob();

    /// @brief calculates the placing probability of all dogs in the race and adds them to race_dog_data
    void calculate_placing();

    /// @brief converts win probability to readable odds
    void convert_to_odds();

    // Stage 3: Racing Stage ----------
    /// @brief selects and random track length and returns it
    int select_track();

    /// @brief calculates and returns the age factor of a dog of the given index (0 to 1)
    /// @param index index of dog to calculate age factor for
    /// @return double of age factor calculated
    double age_factor(int index);

    /// @brief calculates and returns the experience factor of a dog of the given index (0 to 1)
    /// @param index index of dog to calculate experience factor for
    /// @return double of experience factor calculated
    double experience_factor(int index);

    /// @brief calculates and returns the moving speed of a dog for each step in the race
    /// @param index index of dog to get the race step of
    /// @return a double value of the amount to shift the x value of a moving dog
    double race_step(int index);
};

//---------- CONSTANTS ----------//
const int SCREEN_X = 1100;
const int SCREEN_Y = 500;
const int SCREEN_MARGIN = 180;
const int DOG_BITMAP_WIDTH = 512;
const int DOG_BITMAP_HEIGHT = 432;

const int SPRINT = 450;
const int STANDARD = 650;
const int STAYERS = 900;

//---------- FUNCTIONS  ----------//
//---------- STAGE 3: RACING STAGE  ----------//
/// @brief draws track length and race information into the screen
/// @param race main race struct
void draw_information(const Race &race, int track_length);

//---------- STAGE 4: RESULT STAGE  ----------//
/// @brief calculates payouts, updates player data and prints the pnl summary
void handle_payout(const Race &race, Player &player);

//---------- STAGE 5: QUIT / PLAY AGAIN STAGE ----------//
/// @brief resets arrays for the next round
void reset_arrays(Race &race, Player &player);