# Hangman Game
Welcome to the Hangman Game, a fun and interactive console-based game written in C++. This game offers several modes including Singleplayer, Two Player, and an Interactive Two Player mode where one player provides a word and a hint, and the other guesses it. Additionally, the game features a word list management system for adding and viewing words.

## Features
### Singleplayer
Play against the computer with words randomly selected from the included list.
### Two Player
Two players can play against each other, taking turns to guess words.
### Interactive Two Player
A unique mode where one player inputs a word and a hint, and the other player attempts to guess the word.
### Word List Management
Add new words to the game's word list or view existing words.

## Prerequisites
Ensure you have the following installed:

* CMake (minimum required version 3.10)
*  A C++ compiler supporting at least C++11 standard
   
## Building the Game
To build the game, follow these steps:

1. Clone the repository:
```sh
git clone https://github.com/milanfusco/Hangman.git
```
2. Navigate to the project directory:
```sh
cd Hangman
```
3. Create a build directory:
```sh
mkdir build && cd build
```
4. Run CMake:
```sh
cmake ..
```
5. Build the project:
```sh
make
```
This will compile the game and generate an executable named 'HangmanGame' in the 'build' directory.

## Running the Game 
To run the game, navigate to the 'build' directory and execute:
```sh
./HangmanGame
```

## Game Menu 
When you start the game, you'll be greeted with the main menu, where you can select from the following options:

1. Play Singleplayer
2. Play Two Player
3. Play Interactive Two Player
4. Manage Word List
* 1. View Existing Words
* 2. Add Words
* 3. Return to the Main Menu

Choose an option by entering the corresponding number.

## Difficulty Level
For Singleplayer, Two Player, and Interactive Two Player, you will be prompted to select a difficulty level.
1. Noob - 8 Guesses
2. Intermediate - 4 Guesses
3. Veteran - 2 Guesses

## Add New Words
To add new words, select the "Manage Word List" option from the menu, then choose to add a new word. Follow the prompts to enter the word and optionally, a hint.

## Contributions
Contributions are welcome! If you have suggestions for improvements or new features, feel free to create an issue or a pull request.


*Have Fun!*
