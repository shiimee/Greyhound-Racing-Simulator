# Greyhound Racing Simulator
A C++ simulation of greyhound racing aims to model the random environment associated with greyhound racing.

### Overview
This project started as an attempt to model the randomness in greyhound racing the way a bookmaker actually would. 

### How it works
1. Attribute generation: A json files contains the stereotypes of 100 different runners which is selected at random for each race.
2. Win Probabilities: Attributes are evaluated into a probability distribution via softmax.
3. Place Probabilities: Harville formula is applied on top of the win probabilities to compute the conditional probability of each dog finishing top 3.
4. Odds applied and rounding: Fair odds are generated from the probability distribution with a house rounding applied.
5. Simulation and rendering: A simple GUI simulation is ran using Splashkit.
6. betting: Users can place win/place bets before the race and the engine settles the bet after, updating a running PnL.

### Tech Stack
Language: C++
Libraries: Splashkit 
