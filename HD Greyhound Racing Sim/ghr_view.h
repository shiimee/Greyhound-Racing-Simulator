#pragma once
#include "ghr.h"
#include "splashkit.h"
#include "utilities.h"

//----- VIEW RELATED FUNCTIONS -----//
/// @brief prints the greyhound racing banner in the terminal
void print_welcome();

/// @brief draws the welcome words into the screen 
void draw_welcome_screen();

//---------- STAGE 2: BETTING STAGE  ----------//
/// @brief prints the odds menu into the terminal for the user 
/// @param race main race struct
void print_odds(const Race &race);

/// @brief prints the betting menu into the terminal
void print_bet_menu(double balance);

//---------- STAGE 3: RACING STAGE  ----------//
/// @brief resize the screen to match the track_length
/// @param track_length track_length to resize screen to
void resize_screen(int track_length);

/// @brief draws track length and race information into the screen
/// @param race main race struct
void draw_information(const Race &race, int track_length);

/// @brief draw start and finish black and white line onto the screen
void draw_SF_line(int x, int y, int rec_width, int rec_height);

/// @brief draw the side lines of the track into the screen, color side lines according to track length
void draw_side_lines(int track_length, int SL_X, int SL_Y1, int SL_Y2, int SL_Width, int SL_Height);

/// @brief draws the track into the screen
/// @param track_length desired track length
/// @return the x coordinate of the finishing line
int initialise_track(int track_length);

/// @brief loads all 8 bitmaps into an array in Race
/// @param race main race struct
void load_bitmaps(Race &race);

/// @brief creates 8 dogs, one per lane 
/// @param race main race struct
void create_dog_sprites(Race &race);

/// @brief prints the winning order into the terminal
void print_position(const Race &race, int index, int position);

/// @brief draws "Wating for round x..." into the screen 
/// @param race main race struct
void draw_waiting_screen(const Race &race);

//---------- STAGE 4: RESULT STAGE  ----------//
/// @brief calculates and prints the user's overall pnl or both race and overall pnl
/// @param race main race struct
/// @param player main player struct
/// @param race_pnl pnl of current race (used in handle_payout function)
/// @param race_total_bet total amount bet in current race (used in handle_payout function)
/// @param final_pnl set to true if this is the final pnl
void print_player_pnl(const Race &race, Player &player, double race_pnl, double race_total_bet, bool final_pnl);