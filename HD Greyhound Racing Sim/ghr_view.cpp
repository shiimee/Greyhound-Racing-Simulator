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
// prints the greyhound racing banner in the terminal
void print_welcome()
{
    write_line("=========================");
    write_line("Greyhound Racing Simulator!");
    write_line("=========================");
}

// draws the welcome words into the screen 
void draw_welcome_screen()
{
    // load font
    font font = load_font("Bold", "Rye-Regular.ttf");

    // set text and font size
    string text = "Welcome to Greyhound Racing Simulator!";
    int font_size = 34;

    // get x and y coordindates to center text
    int x = (SCREEN_X - text_width(text, font, font_size)) / 2;
    int y = (SCREEN_Y - text_height(text, font, font_size)) / 2;

    // clear the screen and draw the text 
    clear_screen(COLOR_WHITE);
    draw_text(text, COLOR_BLACK, font, font_size, x, y);

    // refresh and process events
    refresh_screen();
    process_events();
}

//---------- STAGE 2: BETTING STAGE  ----------//

// prints the odds menu for the user 
void print_odds(const Race &race)
{
    write_line("\n=========================");
    write_line(format("DOG ODDS FOR RACE {}:", race.race_number));
    write_line("=========================");

    for (int i = 0; i < length(race.race_dog_data); i++)
    {
        write_line(format("[{}] {}: (win odds: ${:.2f}) (place odds: ${:.2f})", i + 1, race.race_dog_data[i].name, race.race_dog_data[i].win_odds, race.race_dog_data[i].place_odds));
    }
    
    write_line("=========================");
}

// prints the betting menu
void print_bet_menu(double balance)
{
    write_line("\n=========================");
    write_line("Bet Menu:");
    write_line("=========================");
    write_line("[w] Bet on Win");
    write_line("[p] Bet on Placing\n");
    write_line(format("Current balance: ${:.2f}\n", balance)); 
}

//---------- STAGE 3: RACING STAGE  ----------//
// resize the screen to match the track_length
void resize_screen(int track_length)
{
    int screen_x = track_length + SCREEN_MARGIN;
    resize_current_window(screen_x, SCREEN_Y);
}

// draws track length and race information into the screen
void draw_information(const Race &race, int track_length)
{   
    draw_text(format("Race Number : {}", race.race_number), COLOR_BLACK, 50, 20);
    draw_text(format("Track Length: {}m", track_length), COLOR_BLACK, 50, 30);
}

// draw start and finish black and white line onto the screen
void draw_SF_line(int x, int y, int rec_width, int rec_height)
{
    int count = 0;

    for (int i = 0; i < 12; i++)
    {
        draw_rectangle(COLOR_BLACK, x, y + i * rec_height, rec_width, rec_height);
        draw_rectangle(COLOR_BLACK, x + 10, y + i * rec_height, rec_width, rec_height);
        
        if (count % 2 == 0)
        {
            fill_rectangle(COLOR_BLACK, x, y + i * rec_height, rec_width, rec_height);
        }
        else 
        {
            fill_rectangle(COLOR_BLACK, x + 10, y + i * rec_height, rec_width, rec_height);
        }
    
        count += 1;
    } 
}

// draw the side lines of the track into the screen, color side lines according to track length
void draw_side_lines(int track_length, int SL_X, int SL_Y1, int SL_Y2, int SL_Width, int SL_Height)
{   
    // draw outlines
    draw_rectangle(COLOR_BLACK, SL_X, SL_Y1, SL_Width, SL_Height);
    draw_rectangle(COLOR_BLACK, SL_X, SL_Y2, SL_Width, SL_Height);

    // sprint color side lines dark green
    if (track_length == 450)
    {
        fill_rectangle(COLOR_DARK_GREEN, SL_X, SL_Y1, SL_Width, SL_Height);
        fill_rectangle(COLOR_DARK_GREEN, SL_X, SL_Y2, SL_Width, SL_Height);    
    }

    // standard color side lines cadet blue
    else if (track_length == 650)
    {
        fill_rectangle(COLOR_CADET_BLUE, SL_X, SL_Y1, SL_Width, SL_Height);
        fill_rectangle(COLOR_CADET_BLUE, SL_X, SL_Y2, SL_Width, SL_Height);
    }

    // stayer color sides lines dark red
    else
    {
        fill_rectangle(COLOR_DARK_RED, SL_X, SL_Y1, SL_Width, SL_Height);
        fill_rectangle(COLOR_DARK_RED, SL_X, SL_Y2, SL_Width, SL_Height);
    }
}

// draws the track into the screen according to the track length and returns the finish line coordinates
int initialise_track(int track_length)
{   
    //----- START AND FINISH LINE -----//
    int SLine_X = 110;
    int SLine_Y = 120;
    int SFLine_Width = 10;
    int SFLine_Height = 20;

    // draw start line
    draw_SF_line(SLine_X, SLine_Y, SFLine_Width, SFLine_Height);

    // draw finish line
    int FLine_X = SLine_X + track_length; // scale track
    draw_SF_line(FLine_X, SLine_Y, SFLine_Width, SFLine_Height);

    //----- SIDE LINES -----//
    int SideLine_X = SLine_X;
    int SideLine_Y_1 = SLine_Y - 10;
    int SideLine_Y_2 = SLine_Y + 12 * SFLine_Height;
    int SideLine_Width = FLine_X - SLine_X + 20;
    int SideLine_Height = 10;

    // draw side lines
    draw_side_lines(track_length, SideLine_X, SideLine_Y_1, SideLine_Y_2, SideLine_Width, SideLine_Height);
    
    return FLine_X;
}

// loads all 8 bitmaps into an array in Race
void load_bitmaps(Race &race)
{
    // load all 8 bitmaps
    for (int i = 1; i < 9; i++)
    {
        string name = format("Dog{}", i); 
        string file = format("Dog_{}.png", i);

        bitmap bmp = load_bitmap(name, file);
        bitmap_set_cell_details(bmp, DOG_BITMAP_WIDTH/8, DOG_BITMAP_HEIGHT/9, 8, 9, 68);
        add(race.dog_bitmaps, bmp);
    }

}

// creates 8 dogs, one per lane 
void create_dog_sprites(Race &race)
{
    animation_script running_script = load_animation_script("WalkRight", "RunningScript.txt");

    for (int i = 0; i < 8; i++)
    {
        sprite dog = create_sprite(race.dog_bitmaps[i], running_script);
        sprite_start_animation(dog, "WalkRight");
        sprite_set_x(dog, 50);
        sprite_set_y(dog, 100 + i * 30);
        add(race.racing_dogs, dog);

    }
}

// draws "Wating for round x..." into the screen 
void draw_waiting_screen(const Race &race)
{
    // resize the window back to original length
    resize_current_window(SCREEN_X, SCREEN_Y);

    // load font
    font font = load_font("Bold", "Rye-Regular.ttf");

    // set text and font size
    string text = format("Waiting for round {}...", race.race_number + 1);
    int font_size = 34;

    // get x and y coordindates to center text
    int x = (SCREEN_X - text_width(text, font, font_size)) / 2;
    int y = (SCREEN_Y - text_height(text, font, font_size)) / 2;

    // clear the screen and draw the text 
    clear_screen(COLOR_WHITE);
    draw_text(text, COLOR_BLACK, font, font_size, x, y);

    // refresh and process events
    refresh_screen();
    process_events();
}

//---------- STAGE 4: RESULT STAGE  ----------//
// prints the winning order into the terminal
void print_position(const Race &race, int index, int position)
{
    if (position == 1)
    {
        write_line("\n=========================");
        write_line(format("Race {} Winners!", race.race_number));
        write_line("=========================");
        write_line(format("[1st] {}", race.race_dog_data[index].name));
    }

    else if (position == 2)
    {
        write_line(format("[2nd] {}", race.race_dog_data[index].name));
    }

    else if (position == 3)
    {
        write_line(format("[3rd] {}", race.race_dog_data[index].name));
    }

    else 
    {
        write_line(format("[{}th] {}", position, race.race_dog_data[index].name));
    }
}


// calculates and prints the user's overall pnl or both race and overall pnl
void print_player_pnl(const Race &race, Player &player, double race_pnl, double race_total_bet, bool final_pnl)
{   
    double race_pnl_perc = 0.0;
    double overall_pnl_perc = 0.0;

    //calulate race pnl %
    if (race_total_bet > 0)
    {
        race_pnl_perc =  (race_pnl / race_total_bet) * 100.0;
    }
    
    // calculate overall pnl%
    if (player.total_bet > 0)
    {
        overall_pnl_perc = (player.pnl / player.initial_balance) * 100.0;
    }
    
    // prints race and overall pnl
    if (final_pnl == false)
    {
        write_line("\n=========================");
        write_line("Player PNL Summary:");
        write_line("=========================");

        // print data
        write_line(format("Bankroll PNL: ${:.2f} ({:.2f}%)", player.pnl, overall_pnl_perc));
        write_line(format("Race {} PNL: ${:.2f} ({:.2f}%)", race.race_number, race_pnl, race_pnl_perc));
    }

    // prints only the overall pnl
    else
    {
        write_line("\n=========================");
        write_line("Final PNL Summary:");
        write_line("=========================");

        // print data
        write_line(format("Bankroll PNL: ${:.2f} ({:.2f}%)", player.pnl, overall_pnl_perc));
    }
    
}