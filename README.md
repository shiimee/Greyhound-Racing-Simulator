# Greyhound Racing Simulator

A C++ greyhound racing and betting game built on [SplashKit](https://splashkit.io). Each race draws eight runners at random from a pool of 100 greyhounds and turns their attributes (speed, age, luck, win rate, experience) into win and place probabilities. It prices odds from those probabilities with a house margin, then runs an animated race where each dog's speed varies by luck on every frame. Before each race you place win or place bets in the terminal. The engine settles them when the race ends and keeps a running profit and loss. The project shows how bookmaking maths (softmax win probabilities, Harville place probabilities, overround) works in a small interactive game.

## Features

- **Random fields:** 8 unique dogs per race, drawn from `Resources/json/greyhounds.json` (100 runners).
- **Strength model:** a weighted score of speed (35%), win % (50%) and luck (15%), multiplied by an age curve (peaks around age 3.2) and a log-scaled experience factor.
- **Win probabilities:** a softmax over strengths with a tunable temperature (`TEMP = 8`).
- **Place probabilities:** the Harville formula gives P(1st) + P(2nd) + P(3rd) for each dog.
- **Odds:** fair odds reduced by a 20% house margin on win bets and 25% on place bets, with a floor of 1.05.
- **Animated race:** a SplashKit window with sprite-animated dogs. The track is randomly 450 m (sprint), 650 m (standard) or 900 m (stayers), and the window resizes to fit it.
- **Race physics:** each frame, a dog moves by its effective speed plus a luck term. The luck range shrinks as the dog gains experience.
- **Betting and P&L:** multiple win/place bets per race, input validation, payout settlement, and per-race and session P&L summaries.
- **Session limits:** up to 8 races per session. The game also ends if your balance reaches $0.

## Tech Stack

- **Language:** C++20 (uses `std::format`)
- **Libraries:** SplashKit (graphics, sprites, JSON, input), C++ standard library
- **Data:** JSON runner database, PNG sprite sheets, and a SplashKit animation script

## Project Structure

```
HD Greyhound Racing Sim/
├── ghr_cli.cpp      # main(): game loop (waiting → betting → racing → result → end)
├── ghr.cpp / .h     # Dog, Player, Race structs; probability, odds, race-step and payout logic
├── ghr_view.cpp / .h# terminal menus and SplashKit drawing (track, sprites, screens)
├── utilities.cpp/.h # validated terminal input helpers
└── Resources/       # greyhounds.json, Dog_1–8.png, RunningScript.txt, Rye-Regular.ttf
```

## Setup

1. Install SplashKit (this also installs the `skm` command-line tool): https://splashkit.io/installation/
2. Make sure you have a compiler that supports C++20 (clang++ or g++).

## Build and Run

Run these from the source directory so SplashKit can find `Resources/`:

```bash
cd "HD Greyhound Racing Sim"
skm clang++ *.cpp -std=c++20 -o main
./main
```

If you use g++, replace `skm clang++` with `skm g++`. The committed `main` binary is built for macOS on arm64 (Apple Silicon); rebuild it on any other platform.

## Example Usage

The game window opens, and betting happens in the terminal:

```
=========================
Greyhound Racing Simulator!
=========================
Enter your initial balance ($10 minimum): $100

=========================
DOG ODDS FOR RACE 1:
=========================
[1] Rocket Flare: (win odds: $6.12) (place odds: $1.84)
[2] Shadow Dancer: (win odds: $3.40) (place odds: $1.21)
...
[8] Iron Claw: (win odds: $2.95) (place odds: $1.10)
=========================

Would you like to make a bet? [y/n] y
Would you like to make a win or place bet [w or p]: w
Your current balance is $100.00, how much would you like to wager: $20
Enter the index of the dog you would like to bet on [1-8]: 2
Make another bet [y/n]? n
```

The race then plays out in the window. Finishing positions print as each dog crosses the line, followed by your payout:

```
You won $68.00 from a winning bet on Shadow Dancer!
Would you like to another round? [y/n]:
```

Payouts are calculated as stake × the quoted odds. A win bet pays only if your dog finishes 1st. A place bet pays if it finishes in the top 3.
