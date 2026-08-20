#include "splashkit.h"
#include "utilities.h"
#include "ghr.h"
#include "ghr_view.h"
#include "splashkit-arrays.h"
#include <iostream>
#include <cmath>
#include <numeric>
#include <string>
#include <format>

using std::string;
using std::format;

//---------- STAGE 1: WAITING STAGE ----------//
// returns the count of a value in an array
int count_value(const dynamic_array<int> &arr, int value)
{
    int count = 0;
 
    for (int i = 0; i < length(arr); i++)
    {
        if (arr[i] == value)
        {
            count += 1;
        }
    }

    return count;
}

// returns an array of length 8 of unique random integers from 0 to 99
dynamic_array<int> create_array_random()
{
    dynamic_array<int> dummy_array;
    dynamic_array<int> true_array; 

    while(length(true_array) < 8)
    {
        int random_int;
        int count;
        
        random_int = rnd(0, 99);

        // validation to ensure no duplicated index
        add(dummy_array, random_int);
        count = count_value(dummy_array, random_int);

        if (count < 2)
        {
            add(true_array, random_int);
        }
    }

    return true_array;
}

// assigns 8 dogs randomly selected from greyhounds.json and assigns them to race_dog_data
void Race::create_dogs()
{
    dynamic_array<Dog> greyhounds;
    dynamic_array<int> numbers = create_array_random(); // returns an array of randomly selected numbers (without dups)
    
    // load file 
    json data = json_from_file("greyhounds.json");
    
    // read the "dogs" array into a vector of json objects
    vector<json> dogs;
    json_read_array(data, "dogs", dogs);


    for (int i = 0; i < 8; i ++)
    {
        // get an index from an array of unqiue numbers between 0 to 99
        int index = numbers[i];

        // get data 
        Dog dog;
        dog.name = json_read_string(dogs[index], "name");
        dog.age = json_read_number_as_double(dogs[index], "age");
        dog.speed = json_read_number_as_double(dogs[index], "speed");
        dog.luck = json_read_number(dogs[index], "luck");
        dog.win_pct = json_read_number_as_double(dogs[index], "win_pct");
        dog.total_races = json_read_number(dogs[index], "total_races");
        dog.finished = false;

        // add to dynamic array greyhounds for race
        add(greyhounds, dog);
    }

    free_json(data);

    race_dog_data = greyhounds;
}

// reads user input and sets the player's starting balance
void Player::initialise_balance()
{
    initial_balance = read_double("Enter your initial balance ($10 minimum): $");

    // input validation 
    while (initial_balance < 10)
    {
        initial_balance = read_double("You have to cash in a minimum of $10, initialise balance: $");
    }

    balance = initial_balance;

}

// initialises the 3 lengths of tracks
void Race::populate_track_length_array()
{
    add(track_length, SPRINT);
    add(track_length, STANDARD);
    add(track_length, STAYERS);
}

//---------- STAGE 2: BETTING STAGE  ----------//
// calculate all dogs strength's and add them to race_dog_data
void Race::calculate_strength()
{
    for (int i = 0; i < length(race_dog_data); i++)
    {
        double strength = (race_dog_data[i].speed / 100.0 * 0.35 
            + race_dog_data[i].win_pct / 100.0 * 0.5 
            + race_dog_data[i].luck / 100.0 * 0.15) 
            * age_factor(i) * experience_factor(i);
        
        
        race_dog_data[i].strength = strength;

        // verification (uncomment)
        // write_line(format("dog [{}] strength: {:.4f}", i, race.race_dog_data[i].strength));
    }
}

// finds highest strength dog in race_dog_data and returns the max strength value
double Race::find_max_strength()
{
    double max = race_dog_data[0].strength;

    // loop through array of race_dog_data to find next highest strength
    for (int i = 1; i < length(race_dog_data); i++)
    {
        if (race_dog_data[i].strength > max)
        {
            max = race_dog_data[i].strength;
        }
    }

    return max;
}

// calculates the win probability of all dogs and adds them to race_dog_data via a softmax function
void Race::calculate_win_prob()
{
    const int TEMP = 8;                //< can be adjusted (increase: greater probability spread among each dog) (decrease: lesser spread)
    dynamic_array<double> exp;
    double total = 0.0;

    // get the max strength of all dogs
    double max = find_max_strength();

    for (int i = 0; i < length(race_dog_data); i++)
    {
        // calculates exponential of dog's (stength - max_strength) * TEMP and adds them to an array exp (numerator)
        double exponential = std::exp((race_dog_data[i].strength - max) * TEMP);
        add(exp, exponential);

        // sum the exponentials (denominator)
        total += exp[i];

        // verification 1: exponentials/numerator value of each dog (uncomment)
        // write_line(format("exponential [{}]: {:.4f}", i, exponential));

    }

    // verification 2: total summed exponentials of all dogs (uncomment)
    // write_line("total exponentials: " + to_string(total));

    for (int i = 0; i < length(race_dog_data); i++)
    {
        // divide exponential by total to get win odds and assign them to each dog's win probability
        race_dog_data[i].win_probability = exp[i] / total;

        // verfication 3: odds of each dog (uncomment)
        // write_line(format("dog [{}] win_odds: {:.4f}", i, race.race_dog_data[i].win_probability));
    }
}

// calculates the probability P(i second | j first)
double calc_prob_second_given_first(double i, double j)
{
    return i / (1 - j);
}

// calculate the probability of P(i third | k second and j first)
double calc_prob_third_given_second_first(double i, double k, double j)
{
    return i / (1 - k - j);
}

// calculate probability of dog at index coming in second
double calculate_probability_second(const dynamic_array<Dog> &race_dog_data, int index)
{
    double prob_i_second = 0;

    // win probability of i
    double prob_i_first = race_dog_data[index].win_probability; 

    // loop one less than the length of array
    for (int j = 0; j < length(race_dog_data); j++)
    {
        // skip the calculation of dog at index
        if (j == index)
        {
            continue;
        }

        // calculates the odds of i placing second against each dog
        double probability_second_against_each_dog = race_dog_data[j].win_probability 
        * calc_prob_second_given_first(prob_i_first, race_dog_data[j].win_probability);
        
        // sum the odds of being second against each dog
        prob_i_second += probability_second_against_each_dog;

        // verification 1: probability of i for each other dog that wins (uncomment)
        // write_line(format("probability of i given [{}] wins: {:.4f}", j, probability_second_against_each_dog));
    }

    // verification 2: overall odds of i coming in second (uncomment)
    // write_line(format("overall probability of i second: {:.4f}", prob_i_second));
    
    return prob_i_second;
}

// calculate probability of dog at index coming in third
double calculate_probability_third(const dynamic_array<Dog> &race_dog_data, int index)
{
    // win probability of i
    double prob_i_first = race_dog_data[index].win_probability;
    double prob_i_third = 0;
    // int count = 0; (verification uncomment)

    // calculate all scenario where dog j wins first 
    for (int j = 0; j < length(race_dog_data); j++)
    {
        // skip the calculation of dog at index
        if (j == index)
        {
            continue;
        }

        // where j wins first k wins second
        for (int k = 0; k < length(race_dog_data); k++)
        {
            // skip the calculation of dog at index and at j
            if (k == index || k == j)
            {
                continue;
            }

            double i_third_given_k_second_j_first = race_dog_data[j].win_probability 
            * calc_prob_second_given_first(race_dog_data[k].win_probability, race_dog_data[j].win_probability) 
            * calc_prob_third_given_second_first(prob_i_first, race_dog_data[k].win_probability, race_dog_data[j].win_probability);
            prob_i_third += i_third_given_k_second_j_first;
            // count += 1; (verification uncomment)
        }
    }

    // verification 1: count how many scenarios calculated, expect 7 x 6 = 42 scenarios (uncomment)
    // write_line("total scenario count: " + to_string(count));

    // verification 2: overall probability of i coming in third (uncomment)
    // write_line("overall probability of i third: " + to_string(prob_i_third));
    return prob_i_third;
}

// calculates the placing probability of all dogs in the race and adds them to race_dog_data
void Race::calculate_placing()
{
    for (int i = 0; i < length(race_dog_data); i++)
    {
        // calculate placing probability: sum the probability of each dog placing first, second and third
        double place_probability = race_dog_data[i].win_probability
        + calculate_probability_second(race_dog_data, i)
        + calculate_probability_third(race_dog_data, i);

        // add placing probability to race_dog_data
        race_dog_data[i].place_probability = place_probability;
    }

}

// converts win probability to readable odds
void Race::convert_to_odds()
{   
    // house margins
    const double HOUSE_WIN_MARGIN = 0.2;
    const double HOUSE_PLACE_MARGIN = 0.25;

    // minimum odds - handles outliers where place odds go below 1
    const double MIN_ODDS = 1.05;

    for (int i = 0; i < length(race_dog_data); i++)
    {
        // calculate true odds
        double true_win_odds = 1.0 / race_dog_data[i].win_probability;
        double true_place_odds = 1.0 / race_dog_data[i].place_probability;

        // calculate odds after house cut
        double win_odds = true_win_odds / (1.0 + HOUSE_WIN_MARGIN);
        double place_odds = true_place_odds / (1.0 + HOUSE_PLACE_MARGIN);

        // outlier handling - let win and place odds equal min odds
        if (win_odds < MIN_ODDS)
        {
            win_odds = MIN_ODDS;
        }
        if (place_odds < MIN_ODDS)
        {
            place_odds = MIN_ODDS;
        }

        // update win and place odds
        race_dog_data[i].win_odds = win_odds;
        race_dog_data[i].place_odds = place_odds;
    }
}

// prompts the user to place bets
void Player::place_bets()
{
    bool placing_bet = false;

    // print bet menu and current balance
    print_bet_menu(balance);

    // ask user if they want to make an initial bet
    placing_bet = read_boolean("Would you like to make a bet? [y/n]");

    while (placing_bet)
    {
        // ----- USER BET TYPE ----- //
        string user_bet_type = to_lowercase(read_string("Would you like to make a win or place bet [w or p]: "));

        // input validation w and p 
        while (user_bet_type != "w" && user_bet_type != "p")
        {
            user_bet_type = to_lowercase(read_string("Enter a valid win or place bet [w or p]: "));
        }

        // add users bet type to bet type 
        if (user_bet_type == "w")
        {
            add(bet_type, 0);
        }
        else 
        {
            add(bet_type, 1);
        }

        // ----- BET AMOUNT ----- // 
        double user_bet_amount = read_double(format("Your current balance is ${:.2f}, how much would you like to wager: $", balance));

        // input validation 
        while (user_bet_amount > balance || user_bet_amount <= 0)
        {
            write_line(format("Current balance: ${:.2f}", balance));
            user_bet_amount = read_double("Please enter a valid amount to wager: $");
        }

        // deduct balance 
        balance -= user_bet_amount;

        // add bet amount to player's total bet
        total_bet += user_bet_amount;
        add(bet_amount, user_bet_amount);

        // ----- INDEX OF BET DOG ----- // 
        int dog_index = read_integer("Enter the index of the dog you would like to bet on [1-8]: ");

        // input validation
        while (dog_index > 8 || dog_index < 1)
        {
            dog_index = read_integer("Enter a valid dog index [1-8]: ");
        }

        // add dogs index to index_dog_bet
        add(index_dog_bet, dog_index - 1);

        // check if player has bet all their money 
        if (balance <= 0.0)
        {
            placing_bet = false;
        }

        // allow player to bet again 
        else
        {
            placing_bet = read_boolean("Make another bet [y/n]? \n");
        }
    }
}


//---------- STAGE 3: RACING STAGE  ----------//
// select and random track length and returns it
int Race::select_track()
{
    int index = rnd(0, 2);
    return track_length[index];
}

// calculates and returns the age factor of a dog of the given index (0 to 1)
double Race::age_factor(int index)
{
    if (race_dog_data[index].age < 2)
    {
        return 0.75;
    }
    else if (race_dog_data[index].age < 4)
    {
        return 1.0 - (abs(race_dog_data[index].age - 3.2) * 0.04);
    }
    else if (race_dog_data[index].age < 5)
    {
        return - (race_dog_data[index].age - 4) * 0.06 + 0.9;
    }
    else 
    {
        return - (race_dog_data[index].age - 5) * 0.09 + 0.85;
    }
    
}

// calculates and returns the experience factor of a dog of the given index (0 to 1)
double Race::experience_factor(int index)
{
    if (race_dog_data[index].total_races >= 120)
    {
        return 1.0;
    }
    else
    {
        return log(race_dog_data[index].total_races + 1)/log(120);
    }
}

// calculates and returns the moving speed of a dog for each step in the race
double Race::race_step(int index)
{
    // calculates the age and experience modifications
    double age_mod = age_factor(index);
    double exp_mod = experience_factor(index);

    // calculates effective speed
    double effective_speed = race_dog_data[index].speed * age_mod * (0.92 + 0.08 * exp_mod);

    // calculate luck factors
    double luck_range = race_dog_data[index].luck * (1.0 - 0.45 * exp_mod);
    double luck_score = luck_range * ((rnd() - 0.5) * 2);

    // final speed
    double moving_speed = ((effective_speed + luck_score) / 100) * 1.5; // scale (multiply by 1.5 to increase speed of game)

    return moving_speed;
}

//---------- STAGE 4: RESULT STAGE  ----------//
// handles the payout
void handle_payout(const Race &race, Player &player)
{
    // total payout to user from this round
    double payout = 0;
    double amount_won = 0;

    // calculate payout amounts 
    for (int i = 0; i < length(player.bet_type); i++)
    {
        // index of dog player bet on
        int index = player.index_dog_bet[i];

        // bet on win
        if (player.bet_type[i] == 0)
        {   
            // check the position of the index of dog the player bet on is 1st
            if (race.race_dog_data[index].position == 1)
            {
                // multiply the player bet amount by dogs win odds
                amount_won = player.bet_amount[i] * race.race_dog_data[index].win_odds;
                payout += amount_won;

                // print result in terminal
                write_line(format("You won ${:.2f} from a winning bet on {}!", amount_won, race.race_dog_data[index].name));
            }
        }

        // bet on place 
        else
        {
            // check if the position of the dog the player bet on is 1st, 2nd or 3rd
            if (race.race_dog_data[index].position < 4)
            {
                // multiply the player bet amount by dogs place odds
                amount_won = player.bet_amount[i] * race.race_dog_data[index].place_odds;
                payout += amount_won;

                // print result in terminal
                write_line(format("You won ${:.2f} from a place bet on {}!", amount_won, race.race_dog_data[index].name));

            }
        }
    }

    // calculate PNL from race
    double race_total_bet = 0;
    for (int i = 0; i < length(player.bet_amount); i++)
    {
        race_total_bet += player.bet_amount[i];
    }
    double pnl = payout - race_total_bet;

    // update overall pnl 
    player.pnl += pnl;

    // update balance 
    player.balance += payout;

    // verification: check betting stats (uncomment)
    // write_line(format("total bet: {}", player.total_bet));
    // write_line(format("pnl: {}", player.pnl));
    // write_line(format("balance: {}", player.balance));

    // print player pnl summary
    print_player_pnl(race, player, pnl, race_total_bet, false);

}

//---------- STAGE 5: QUIT / PLAY AGAIN STAGE ----------//
// resets arrays for the next round
void reset_arrays(Race &race, Player &player)
{
    // reset all player betting related arrays
    clear(player.bet_amount);
    clear(player.bet_type);
    clear(player.index_dog_bet);

    // reset all race dog data
    clear(race.racing_dogs);
    clear(race.race_dog_data);
}
