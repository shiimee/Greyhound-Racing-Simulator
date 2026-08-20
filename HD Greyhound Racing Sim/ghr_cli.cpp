#include "splashkit.h"
#include "utilities.h"
#include "ghr.h"
#include "ghr_view.h"
#include "splashkit-arrays.h"
#include <string>
#include <format>

using std::string;
using std::format;

//---------- STAGE 1: WAITING STAGE ----------//
// 1. create dogs by randomly selecting 8 dogs from database (.json file)
// 2. create players and initalize their balance

//---------- STAGE 2: BETTING STAGE ----------//
// 1. calculate probability and odds of each dog and prints into the terminal
// 2. allow player to place bets

//---------- STAGE 3: RACING STAGE ----------//
// 1. randomly select a track length
// 2. initialise bitmaps and create dog sprites
// 3. perform race

//---------- STAGE 4: RESULT STAGE ----------//
// 1. calculate the amount to payout to user and payout 
// 2. calculate the pnl stats and print in terminal

//---------- STAGE 5: QUIT / PLAY AGAIN STAGE ----------//
// 1. update race number
// 2. check conditions for end game 
// 3. prompts player to play again
// 4. prints the final pnl to player when the game ends

void handle_waiting_stage(Race &race, Player &player)
{
    // 1. create dogs
    race.create_dogs();

    // 2. initialise player balance (if round 1)
    if (race.race_number == 1)
    {
        player.initialise_balance();
    }
}

void handle_betting_stage(Race &race, Player &player)
{
    // 1. calculate probability and odds for each dog and prints into terminal
    // probability calculations
    race.calculate_strength();
    race.calculate_win_prob();
    race.calculate_placing();

    // convert probability to readable odds
    race.convert_to_odds();

    // print odds into the terminal
    print_odds(race);

    // 2. allow the user to place bets
    player.place_bets();
}

void handle_racing_stage(Race &race, Player &player)
{
    bool gui_open = true;

    // load bitmaps (if round 1) and create dog sprites
    if (race.race_number == 1)
    {
        load_bitmaps(race);
    }

    create_dog_sprites(race);

    // randomly select a track length
    int track_length = race.select_track();

    // resize the window according to track_length
    resize_screen(track_length);

    // set position counter
    int position = 1;

    while(gui_open)
    {
        // basic screen commands
        clear_screen(COLOR_WHITE);

        // draw race information
        draw_information(race, track_length);

        // get ending x coordinates and draw track
        int race_end_x_cord = initialise_track(track_length);

        // move and draw all unfinished dogs
        for (int i = 0; i < length(race.racing_dogs); i++)
        {
            // skip all dogs that finished
            if (race.race_dog_data[i].finished == true)
            {
                continue;
            }

            // update and draw sprite
            update_sprite(race.racing_dogs[i]);
            draw_sprite(race.racing_dogs[i]);

            // calculate the race step
            double dog_step = race.race_step(i);

            // moves the dog
            sprite_set_x(race.racing_dogs[i], sprite_x(race.racing_dogs[i]) + dog_step);
        }

        // check if dogs finished and updates the their finished status and position
        for (int i = 0; i < length(race.racing_dogs); i++)
        {
            if (race.race_dog_data[i].finished == true)
            {
                continue;
            }

            if (sprite_x(race.racing_dogs[i]) > race_end_x_cord)
            {
                // update finished status
                race.race_dog_data[i].finished = true;

                // update position
                race.race_dog_data[i].position = position;

                // print result into terminal
                print_position(race, i, race.race_dog_data[i].position);

                // increase position count by 1
                position += 1;

                // free sprite
                free_sprite(race.racing_dogs[i]);
            }
        }

        process_events();
        refresh_screen(60);

        // checks if all the dogs finished the race
        int finished_count = 0;
        for (int i = 0; i < length(race.race_dog_data); i++)
        {

            if (race.race_dog_data[i].finished)
            {
                finished_count += 1;
            }
            
            // once all dogs finished draw the waiting screen and exit loop
            if (finished_count == length(race.race_dog_data))
            {
                gui_open = false;

                // draw waiting screen for round 1-7 only
                if (race.race_number < 8)
                {
                    draw_waiting_screen(race);
                }
                
            }
        }
    }

}

void handle_result_stage(Race &race, Player &player)
{
    handle_payout(race, player);
}

void handle_end_stage(Race &race, Player &player, bool &game_state)
{
    // update race number 
    race.race_number += 1;

    // end game is player bust or all 8 rounds are complete
    if (player.balance <= 0.0 || race.race_number > 8)
    {
        if (player.balance <= 0.0)
        {
            write_line("You depleted your balance, game over!");
        }

        else
        {
            write_line("All 8 races for the day has finished!");
        }

        // print final PNL for the user
        print_player_pnl(race, player, 0, 0, true);

        game_state = false;

    }

    else
    {
        // prompts user for another round
        game_state = read_boolean("Would you like to another round? [y/n]: ");

        // player wants to end game
        if (game_state == false)
        {
            // print final PNL for the user
            print_player_pnl(race, player, 0, 0, true);
        }
        
        // player wants another round
        else
        {
            // reset arrays for the next round
            reset_arrays(race, player);
        }
    }
}


int main()
{
    Race race;
    Player player;
    race.populate_track_length_array();
    bool game_state = true;
    open_window("Greyhound Racing Simulator!", SCREEN_X, SCREEN_Y);
    print_welcome();
    draw_welcome_screen();

    do 
    {
        //---------- STAGE 1: WAITING STAGE ----------//
        handle_waiting_stage(race, player);

        //---------- STAGE 2: BETTING STAGE ----------//
        handle_betting_stage(race, player);

        //---------- STAGE 3: RACING STAGE ----------//
        handle_racing_stage(race, player);

        //---------- STAGE 4: RESULT STAGE ----------//
        handle_result_stage(race, player);

        //---------- STAGE 5: QUIT / PLAY AGAIN STAGE ----------//
        handle_end_stage(race, player, game_state);

    } while (game_state);

    close_all_windows();
}