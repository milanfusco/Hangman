/**
 * @file main.cpp
 *
 * *** Compiling the program requires a C++11 compatible compiler. ***
 *
 * Hangman Game
 * @authors Milan Fusco
 *
 *
 * ====== TECHNICAL DOCUMENTATION ======
 * This program implements a text-based Hangman game with single-player, multi-player, and interactive multiplayer modes (plus a word list management mode).
 * The game additionally features a variable difficulty level, whole word guessing, word list management, game statistics, and interactive menus.
 * The program utilizes enum to define game modes, struct to store words & hints/game state+stats, and a wrapper struct for player state (to persist stats).
 * Vector is used to store the word list in a dynamic array, pointers are used in multiplayer to iterate between player turns, and arrays are used for gallows drawing.
 * References are used to pass game/player states, word list, and filenames.
 *
 * ====== GAMEPLAY LOGIC ======
 * main() initializes the game by reading the word list from a file into a vector and calling playGame() to start the game menu.
 * playGame() displays the game mode menu, processes user input, and calls the appropriate game mode function based on the user's choice.
 * The game modes include single-player, multiplayer, interactive multiplayer, and word list management.
 * Single-player mode selects a random word from the list for the player to guess, updating the game state and displaying the gallows and word.
 * Multiplayer mode allows two players to take turns guessing the same word, with pointers to player states for easy iteration between turns.
 * Interactive multiplayer mode has one player input a word and hint, while the other player guesses the word, with the game state updating after each guess.
 * Word list management mode allows viewing and adding words to the list, displaying the current words and hints, and appending new words to the file.
 * The game prompts are handled using getValidatedInput() to ensure valid input, with clearScreen() to improve readability and clearInputBuffer() to prevent invalid input.
 * The full word guess is handled by wordGuess(), while handleCharacterGuess() processes single letter guesses, updating the game state accordingly.
 * The game difficulty is set using selectDifficultyLevel() and setupDifficulty(), and  GameState is initialized with the chosen difficulty level.
 * The datafile (data.csv) is included as a separate file.
 */
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

#define MAXSIZE 10  // Maximum number of words in the list

/**
 * @enum GameMode
 * @brief Enumerates available game modes.
 * Provides different gameplay options for the user, including single-player, multiplayer variants, and word list management.
 */
enum GameMode {
    EXIT_GAME = 0,
    SINGLE_PLAYER,
    TWO_PLAYER,
    INTERACTIVE_TWO_PLAYER,
    MANAGE_WORDLIST
};

/**
 * @struct wordItem
 * @brief Represents a word and its associated hint within the game.
 * Struct to hold word items that include both the word itself and a hint to aid the player in guessing.
 */
struct WordItem {
    string word;  // The word to be guessed
    string hint;  // A hint to help the player guess the word
};

/**
 * @struct GameState
 * @brief Tracks the state of a Hangman game.
 * Contains all essential details of the current game session, such as the chosen word, hints, and tracking of player guesses.
 */
struct GameState {
    string chosenWord;                                              // Currently selected word for the player to guess.
    string guessedLetters;                                          // Cumulative string of letters guessed by the player.
    string chosenHint;                                              // Hint associated with the chosen word to aid guessing.
    int incorrectGuesses = 0;                                       // Count of the player's incorrect guesses, affecting game progression.
    int maxGuesses;                                                 // Configurable maximum number of incorrect guesses before game over.
    bool wordGuessed = false;                                       // Indicator whether the chosen word has been completely guessed.
    int wonRounds = 0;                                              // Number of rounds won by the player.
    int lostRounds = 0;                                             // Number of rounds lost by the player.
    int totalRounds = 0;                                            // Total number of rounds played by the player.
    double winRate = 0.0;                                           // Percentage of rounds won by the player.
    double lossRate = 0.0;                                          // Percentage of rounds lost by the player.
    explicit GameState(int maxGuesses) : maxGuesses(maxGuesses) {}  // Initializes the game state with a specific difficulty level. (max incorrect guesses allowed)
};

/**
 * @brief wrapper struct for player state
 * Manages the state and statistics of a player in the game.
 * Includes tracking of game state specifics for a player and their overall game statistics across multiple sessions.
 */
struct PlayerState {
    GameState state;
    string playerName;
    int totalWins = 0;    // Total wins across multiple games
    int totalLosses = 0;  // Total losses across multiple games

    PlayerState(const GameState& state, const string& playerName) : state(state), playerName(playerName){};  // Constructor to initialize the player state
};

// =========== FUNCTION PROTOTYPES ============ //

GameMode modeMenu();
void clearScreen();
char getValidatedInput(const string& prompt, const string& validOptions);
void displayWords(const string& filename);
void appendWord(const string& filename);
void readIntoWordItem(vector<WordItem>& wordList, const string& filename);
void manageWordList(const string& filename);
void gameStats(GameState& state);
void convertToUpper(string& str);
int selectDifficultyLevel();
void setupDifficulty(int& maxGuesses);
void displayGameState(const GameState& state);
bool wordGuess(GameState& state, const string& fullGuess);
bool handleCharacterGuess(GameState& state, char guess);
bool processPlayerGuess(GameState& state);
bool checkWordGuessed(GameState& state);
void drawGallows(int incorrect, int maxGuesses);
void endGameDisplay(GameState& state);
bool promptToPlayAgain();
void playSingleplayer(const vector<WordItem>& wordList);
void playInteractiveMultiplayer();
void multiplayerSetup(GameState& state1, GameState& state2, const vector<WordItem>& wordList);
void multiplayerEndGameDisplay(PlayerState& playerState);
void printMultiplayerStats(const PlayerState& player1, const PlayerState& state2);
void playMultiplayer(const vector<WordItem>& wordList);
void playGame(const vector<WordItem>& wordList);

// =========== MAIN ============ //

int main() {
    srand(time(nullptr));  // Seed the random number generator.
    vector<WordItem> wordList;
    readIntoWordItem(wordList, "data.csv");
    playGame(wordList);
    return 0;
}

// =========== HELPER FUNCTIONS ============ //

/**
 * @brief Prompts the user to select a game mode from the available options.
 * Displays the game mode menu and uses validated input to choose the mode.
 * Handles invalid input by defaulting to Single Player mode.
 * @return The chosen game mode as an enum value.
 */
GameMode modeMenu() {
    cout << "Welcome to Hangman!\n"
         << " _____\n |   |\n 0   |\n/|\\  |\n/ \\  |\n    /|\\ \n======\n";
    char choice = getValidatedInput("Select a game mode:\n1. Single Player\n2. Two Player\n3. Interactive Two Player\n4. Manage Word List\n['0'to exit]\n>>> ", "01234") - '0';
    switch (choice) {
        case 0:
            return EXIT_GAME;
        case 1:
            return SINGLE_PLAYER;
        case 2:
            return TWO_PLAYER;
        case 3:
            return INTERACTIVE_TWO_PLAYER;
        case 4:
            return MANAGE_WORDLIST;
        default:
            cout << "Invalid game mode selected. Defaulting to Single Player." << endl;
            return SINGLE_PLAYER;
    }
}

/**
 * @brief clears the input buffer
 * This function clears the input buffer to prevent any invalid input from being processed.
 */
void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clear the input buffer before returning
}

/**
 * @brief Prints a series of newline characters to clear the console screen, improving readability for the user.
 * This function is used to maintain a clean and clear interface before showing new output to the user.
 */
void clearScreen() {
    // Clear the screen by printing enough newlines.
    cout << string(100, '\n');
}

/**
 * @brief Converts a given string to uppercase to standardize user input and facilitate case-insensitive comparisons.
 * Range-based for loop is used to iterate over an unknown number of characters, a nifty feature of C++11 to simplify string manipulation.
 * @param str The string to be converted to uppercase. The conversion is done in-place using a reference to avoid unnecessary copying.
 */
void convertToUpper(string& str) {
    for (char& c : str) {
        c = toupper(c);
    }
}

/**
 * @brief Prompts the user for a single character input and validates it against a string of acceptable characters.
 * Repeatedly prompts until a valid character is entered, which is then returned as the user's choice.
 * @param prompt The message displayed to the user asking for input.
 * @param validOptions A string containing all acceptable characters for input validation.
 * @return The validated character input by the user.
 */
char getValidatedInput(const string& prompt, const string& validOptions) {
    string input;
    cout << prompt;
    while (true) {
        getline(cin, input);  // Using getline to handle full strings

        if (isdigit(input[0]) && validOptions.find(input[0]) != string::npos) {
            return input[0];  // If the input is a number and is not the end of a string, return it.
        }
        if (input.length() == 1 && (validOptions.find(toupper(input[0])) != string::npos || input[0] == '1')) {
            return toupper(input[0]);  // If the input is a single character and in the valid options, return it.
        } else {                       // If the input is invalid, prompt the user again.
            cout << "Invalid response.\nPlease enter only one of\n [" << validOptions << "]\nor '1' to guess the entire word.\n>>> ";
        }
    }
}

// =========== WORD LIST FUNCTIONS ============ //

/**
 * @brief Displays all words and their associated hints from a specified file.
 * Reads each line from the file and prints it, handling file access errors gracefully.
 * @param filename The path to the file containing words and hints.
 */
void displayWords(const string& filename) {
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Failed to open file for reading: " << filename << endl;
        return;
    }
    string line;
    cout << "Existing Words and Hints:\n";
    while (getline(inputFile, line)) {
        cout << line << endl;
    }
    inputFile.close();
}

/**
 * @brief Appends a new word and its hint to the specified file.
 * Prompts the user for a word and a hint, then writes them to the file, ensuring data consistency by converting to uppercase.
 * @param filename The path to the file where the new word and hint are appended.
 */
void appendWord(const string& filename) {
    ofstream outputFile(filename, ios::app);  // Open the file in append mode
    if (!outputFile) {
        cerr << "Failed to open file for appending: " << filename << endl;
        return;
    }
    string word;
    string hint;
    cout << "Enter a new word: ";
    getline(cin, word);  // Use getline to allow spaces in the word
    cout << "Enter a hint for the word: ";
    getline(cin, hint);  // Use getline to allow spaces in the hint

    convertToUpper(word);  // Convert the word to uppercase for consistency
    convertToUpper(hint);  // Convert the hint to uppercase for consistency

    // Ensure to start a new line if the file is not empty
    outputFile << "\n"
               << word << "," << hint;
    cout << "New word and hint added successfully.\n";
    outputFile.close();  // Close the file to flush changes
}

/**
 * @brief Reads words and hints from a file into a vector, parsing each line into a wordItem struct.
 * Ensures that each word and hint is correctly stored in the vector for game use.
 * @param wordList A reference to the vector where the words and hints will be stored.
 * @param filename The path to the file containing the words and hints.
 */
void readIntoWordItem(vector<WordItem>& wordList, const string& filename) {
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }
    string line;
    while (getline(inputFile, line)) {
        size_t delimiterPos = line.find(',');
        if (delimiterPos != string::npos) {
            WordItem item;
            item.word = line.substr(0, delimiterPos);
            item.hint = line.substr(delimiterPos + 1);
            wordList.push_back(item);
        }
    }
}

/**
 * @brief Provides a menu system for managing the word list, allowing viewing and addition of words.
 * Validates user input to navigate through the options of viewing words, adding new ones, or returning to the main menu.
 * @param filename The path to the file used for word storage and retrieval.
 */
void manageWordList(const string& filename) {
    bool continueManagement = true;  // Flag to keep
    string validOptions = "123";     // Valid options for management menu

    while (continueManagement) {
        clearScreen();  // Clears the screen for better readability
        cout << "Word List Management:\n";
        cout << "1. View Words\n";
        cout << "2. Add Word\n";
        cout << "3. Return to Main Menu\n";

        char choice = getValidatedInput("Choose an option (1-View, 2-Add, 3-Return):\n>>> ", validOptions);  // Get user input
        switch (choice) {
            case '1':
                displayWords(filename);  // Display the words and hints
                break;
            case '2':
                appendWord(filename);  // Append a new word and hint to the file
                break;
            case '3':
                continueManagement = false;  // Break the loop to return to the main menu;
                break;
            default:
                cout << "Invalid option selected. Please try again.\n";  // Display an error message for invalid input
        }

        if (choice == '1' || choice == '2') {
            cout << "\nPress enter to continue...";
            clearInputBuffer();  // Wait for user input before continuing
        }
    }
}

// =========== GAME FUNCTIONS ============ //

/**
 * @brief Calculates and updates the win and loss rates based on the game's current state.
 * Ensures accurate tracking of game statistics by updating win and loss rates after each round.
 * @param state The current game state containing the counts of won and lost rounds.
 */
void gameStats(GameState& state) {
    if (state.wonRounds > 0 || state.lostRounds > 0) {
        state.totalRounds = state.wonRounds + state.lostRounds;
    }
    state.winRate = (state.totalRounds == 0) ? 0 : static_cast<double>(state.wonRounds) / state.totalRounds * 100;
    state.lossRate = (state.totalRounds == 0) ? 0 : static_cast<double>(state.lostRounds) / state.totalRounds * 100;
}

/**
 * @brief Allows the user to select a difficulty level, affecting the number of allowed incorrect guesses.
 * Prompts the user to choose among predefined difficulty levels, returning the corresponding max guesses allowed.
 * @return The number of incorrect guesses allowed based on the selected difficulty level.
 */
int selectDifficultyLevel() {
    int DEFAULT_LEVEL = 8;       // 8 guesses by default
    int INTERMEDIATE_LEVEL = 4;  // 4 guesses for intermediate level
    int VETERAN_LEVEL = 2;       // 2 guesses for veteran level

    char choice = getValidatedInput("Select a difficulty level:\n1. Noob (8 guesses)\n2. Intermediate (4 guesses)\n3. Veteran (2 guesses)\n>>> ", "123");

    switch (choice) {  // switch(choice) returns maxGuesses based on user level selection
        case '1':
            return DEFAULT_LEVEL;
        case '2':
            return INTERMEDIATE_LEVEL;
        case '3':
            return VETERAN_LEVEL;
        default:  // Default to Noob level if invalid input
            cout << "Invalid input. Defaulting to Noob level (8 guesses).\n";
            return DEFAULT_LEVEL;
    }
}

/**
 * @brief Sets up the difficulty of the game by determining the maximum number of incorrect guesses allowed.
 * Uses the selected difficulty level to set the maxGuesses for the game session.
 * @param maxGuesses A reference to store the maximum number of allowed incorrect guesses.
 */
void setupDifficulty(int& maxGuesses) {
    maxGuesses = selectDifficultyLevel();  // Adjusted to return an int
}

/**
 * @brief Displays the current state of the game, including the word with guessed and unguessed letters, and the current gallows drawing.
 * Provides visual feedback on the game's progress, showing guessed letters and the visual state of the gallows.
 * @param state The current game state to be displayed.
 */
void displayGameState(const GameState& state) {
    drawGallows(state.incorrectGuesses, state.maxGuesses);
    cout << "Hint: " << (state.incorrectGuesses == 0 ? "" : state.chosenHint) << endl;
    cout << "Guessed Letters: " << state.guessedLetters << endl;
    for (char letter : state.chosenWord) {
        cout << (state.guessedLetters.find(letter) != string::npos ? letter : '_') << ' ';
    }
    cout << endl;
}

/**
 * @brief Processes a complete word guess from the user, comparing it against the chosen word in the game state.
 * Updates the game state based on whether the guess was correct or not, potentially ending the game.
 * @param state The current game state, which includes the correct word.
 * @param fullGuess The full word guessed by the user.
 * @return True if the guess was correct, otherwise false.
 */
bool wordGuess(GameState& state, const string& fullGuess) {
    if (fullGuess == state.chosenWord) {
        state.wordGuessed = true;
        cout << "Correct! The word was: " << state.chosenWord << endl;
        return true;
    } else {
        cout << "Incorrect! The correct word was: " << state.chosenWord << endl;
        state.incorrectGuesses = state.maxGuesses;  // Set incorrect guesses to max to end the game.
        return false;
    }
}

/**
 * @brief Handles the user's guess of a single letter, updating the game state based on whether the guess was correct.
 * Checks if the letter has already been guessed and updates the count of incorrect guesses if necessary.
 * @param state The current game state which will be updated.
 * @param guess The character guessed by the player.
 * @return True if the guessed letter is in the word, false if not.
 */
bool handleCharacterGuess(GameState& state, char guess) {
    if (state.guessedLetters.find(guess) != string::npos) {  // Check if the letter has already been guessed.
        cout << "You have already guessed '" << guess << "'. No penalty." << endl;
        return false;
    }
    state.guessedLetters += guess;
    if (state.chosenWord.find(guess) == string::npos) {  // Check if the guessed letter is in the chosen word.
        cout << '"' << guess << '"' << " is incorrect!" << endl;
        state.incorrectGuesses++;
        return false;  // Return false if the guessed letter is not in the chosen word.
    } else {
        cout << '"' << guess << '"' << " is correct!" << endl;
        return checkWordGuessed(state);  // Check if the word has been fully guessed.
    }
}

/**
 * @brief Manages the user input for guessing letters or the entire word, updating the game state accordingly.
 * Facilitates the main interaction in the game, processing each guess and updating the game state.
 * @param state The current game state to be updated based on the user's guess.
 * @return True if the user's guess was correct or the word has been fully guessed, false otherwise.
 */
bool processPlayerGuess(GameState& state) {
    char guess = getValidatedInput("Enter your guess 'A-Z' or enter '1' to guess the entire word.\n>>> ", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    if (guess == '1') {
        cout << "Type your guess for the word.\n>>> ";
        string fullGuess;
        getline(cin, fullGuess);
        convertToUpper(fullGuess);  // Convert to uppercase to standardize input handling
        return wordGuess(state, fullGuess);
    } else {
        return handleCharacterGuess(state, guess);
    }
}

/**
 * @brief Checks if the entire word has been guessed correctly based on the letters guessed so far.
 * Determines if the game has been won by checking each letter of the word against the guessed letters.
 * @param state The current game state containing the word and the guessed letters.
 * @return True if all letters in the word have been guessed, false otherwise.
 */
bool checkWordGuessed(GameState& state) {
    for (char letter : state.chosenWord) {                        // Check each letter in the chosen word.
        if (state.guessedLetters.find(letter) == string::npos) {  // If the letter is not in the guessed letters string.
            return false;                                         // Return false if any letter in the word has not been guessed yet.
        }
    }
    state.wordGuessed = true;  // Set the wordGuessed flag to true if the guess is correct.
    return true;               // If all letters have been guessed correctly, return true.
}

/**
 * @brief Draws the gallows based on the current number of incorrect guesses, depicting the player's progress towards losing.
 * Dynamically updates the gallows display to visually represent the stakes of the game as guesses are made.
 * @param incorrectGuesses The number of incorrect guesses made so far.
 * @param maxGuesses The maximum number of incorrect guesses allowed before the game is lost.
 */
void drawGallows(int incorrectGuesses, int maxGuesses) {
    const int MAX_GUESS_STAGE = 9;  // Maximum stages for gallows drawing

    array<string, MAX_GUESS_STAGE> stages = {
        "     \n     \n     \n     \n     \n     \n     ",               // Stage 0: Empty gallows.
        "     \n     \n     \n     \n     \n     \n======",              // Stage 1: Base of the gallows.
        "     \n     | \n     | \n     | \n     | \n    /|\\ \n======",  // Stage 2: Base and vertical pole.
        " _____\n |   |\n     |\n     |\n     |\n    /|\\ \n======",     // Stage 3: Base, vertical pole, and horizontal beam.
        " _____\n |   |\n 0   |\n     |\n     |\n    /|\\ \n======",     // Stage 4: Base, vertical pole, horizontal beam, and head.
        " _____\n |   |\n 0   |\n/|   |\n     |\n    /|\\ \n======",     // Stage 5: Base, vertical pole, horizontal beam, head, and one arm.
        " _____\n |   |\n 0   |\n/|\\  |\n     |\n    /|\\ \n======",    // Stage 6: Base, vertical pole, horizontal beam, head, and both arms.
        " _____\n |   |\n 0   |\n/|\\  |\n/    |\n    /|\\ \n======",    // Stage 7: Base, vertical pole, horizontal beam, head, both arms, and one leg.
        " _____\n |   |\n 0   |\n/|\\  |\n/ \\  |\n    /|\\ \n======"    // Stage 8: Base, vertical pole, horizontal beam, head, both arms, and both legs.
    };

    int totalStages = stages.size();  // Get the correct number of stages.

    // Ensure the index calculation is safe.
    if (incorrectGuesses < 0 || maxGuesses <= 0) {
        cerr << "Invalid parameters for drawGallows function." << endl;
        return;
    }

    int index = (incorrectGuesses * (totalStages - 1)) / maxGuesses;  // Calculate the index based on the ratio of incorrect guesses to max guesses.
    index = min(index, totalStages - 1);                              // Ensure index does not exceed the array bounds.
    cout << stages[index] << endl;                                    // Display the gallows stage.
}

/**
 * @brief Displays the end of the game message, showing whether the player has won or lost and the correct word.
 * Updates game statistics and shows a detailed message regarding the game's outcome.
 * @param state The final state of the game, used to determine and display the outcome.
 */
void endGameDisplay(GameState& state) {
    if (state.wordGuessed) {
        state.wonRounds++;
        cout << "Congratulations, you've guessed the word: " << state.chosenWord << endl;
    } else if (state.incorrectGuesses >= state.maxGuesses) {
        state.lostRounds++;
        drawGallows(state.incorrectGuesses, state.maxGuesses);
        cout << "Sorry, you've been hanged." << endl;
        cout << "The correct word was: " << state.chosenWord << "\n"
             << endl;
    }

    // Only update total rounds and rates at the end of a player's game
    state.totalRounds = state.wonRounds + state.lostRounds;
    state.winRate = (state.totalRounds > 0) ? static_cast<double>(state.wonRounds) / state.totalRounds * 100.0 : 0.0;
    state.lossRate = (state.totalRounds > 0) ? static_cast<double>(state.lostRounds) / state.totalRounds * 100.0 : 0.0;
    cout << "You have won " << state.wonRounds << " rounds and lost " << state.lostRounds << " rounds.\n";
    cout << "Win rate: " << state.winRate << "%, Loss rate: " << state.lossRate << "%" << endl;
}

/**
 * @brief Prompts the player to decide whether to play another game, returning their choice.
 * Ensures continued play based on user interest, maintaining engagement with the game.
 * @return True if the player chooses to play again, false if they decide to stop.
 */
bool promptToPlayAgain() {
    char response = getValidatedInput("Would you like to play again? (Y/N) ", "YyNn");
    return (response == 'Y' || response == 'y');
}

// =========== SINGLEPLAYER FUNCTION ============ //

/**
 * @brief Executes the singleplayer mode of Hangman.
 * This function orchestrates the singleplayer game by randomly selecting a word from the provided list,
 * initializing the game state, and managing the game loop. Players guess letters or the entire word
 * to try to solve the hangman before they run out of guesses.
 * @param wordList A vector of wordItem structures containing words and hints to be used in the game.
 */
void playSingleplayer(const vector<WordItem>& wordList) {
    cout << "Starting the singleplayer game with " << wordList.size() << " words." << endl;
    int maxGuesses;
    setupDifficulty(maxGuesses);

    do {
        GameState state(maxGuesses);
        int wordIndex = rand() % wordList.size();  // Randomly select a word from the list
        state.chosenWord = wordList[wordIndex].word;
        state.chosenHint = wordList[wordIndex].hint;
        convertToUpper(state.chosenWord);
        convertToUpper(state.chosenHint);

        cout << "Welcome to Hangman!" << endl;
        while (state.incorrectGuesses < state.maxGuesses && !state.wordGuessed) {  // Game loop
            displayGameState(state);
            if (!processPlayerGuess(state)) {
                cout << "You have " << state.maxGuesses - state.incorrectGuesses << " incorrect guesses remaining." << endl;
            }
        }
        endGameDisplay(state);
    } while (promptToPlayAgain());
}

// =========== INTERACTIVE MULTIPLAYER FUNCTION ============ //

/**
 * @brief Facilitates the interactive multiplayer mode of Hangman.
 * In this mode, one player inputs a word and a hint, which another player tries to guess.
 * The game proceeds with guessing turns until the word is guessed or attempts are exhausted.
 * The game state is displayed after each guess, and players are prompted to continue or end the game after each round.
 * @param maxGuesses The maximum number of incorrect guesses allowed before the game ends.
 */
void playInteractiveMultiplayer() {
    string word;
    string hint;
    int maxGuesses;
    setupDifficulty(maxGuesses);

    do {
        cout << "Welcome to Hangman Interactive Multiplayer!\n";

        // player 1 sets the word and the hint
        cout << "Player 1, please enter the word to be guessed: ";
        getline(cin, word);
        cout << "Player 1, please enter a hint for the word: ";
        getline(cin, hint);
        clearScreen();

        GameState state(maxGuesses);
        state.chosenWord = word;
        state.chosenHint = hint;
        convertToUpper(state.chosenWord);
        convertToUpper(state.chosenHint);

        // player 2 guesses the word
        cout << "Player 2, you will now guess the word.\n";
        while (state.incorrectGuesses < state.maxGuesses && !state.wordGuessed) {
            displayGameState(state);
            if (!processPlayerGuess(state)) {
                cout << "You have " << state.maxGuesses - state.incorrectGuesses << " incorrect guesses remaining." << endl;
            }
        }
        endGameDisplay(state);

    } while (promptToPlayAgain());
    return;
}

// =========== MULTIPLAYER FUNCTIONS ============ //

/**
 * @brief Displays the final game results in multiplayer mode, detailing each player's outcome.
 * This function reveals whether each player guessed the word correctly or was "hanged", displays the correct word,
 * and updates the total wins and losses for each player.
 * @param playerState The game state and statistics for the player whose turn just ended.
 */
void multiplayerEndGameDisplay(PlayerState& playerState) {
    if (playerState.state.wordGuessed) {
        playerState.totalWins++;
        cout << "Congratulations, " << playerState.playerName << ", you've guessed the word: " << playerState.state.chosenWord << endl;
    } else if (playerState.state.incorrectGuesses == playerState.state.maxGuesses) {
        playerState.totalLosses++;
        drawGallows(playerState.state.incorrectGuesses, playerState.state.maxGuesses);
        cout << "Sorry, " << playerState.playerName << ", you've been hanged." << endl;
        cout << "The correct word was: " << playerState.state.chosenWord << endl;
    } else {
        cerr << "Invalid game state for player " << playerState.playerName << endl;
    }

    cout << playerState.playerName << " has won " << playerState.totalWins << " rounds and lost " << playerState.totalLosses << " rounds.\n";
}

/**
 * @brief Outputs the multiplayer game statistics, showing wins and losses for each player.
 * This function is called at the end of a multiplayer game session to display the final results,
 * including the total number of games won and lost by each player.
 * @param player1 The game state for player 1, including win/loss statistics.
 * @param player2 The game state for player 2, including win/loss statistics.
 */
void printMultiplayerStats(const PlayerState& player1, const PlayerState& player2) {
    cout << "Player 1: " << player1.totalWins << " wins, " << player1.totalLosses << " losses. \n";
    cout << "Player 2: " << player2.totalWins << " wins, " << player2.totalLosses << " losses. \n";
}

/**
 * @brief Sets up a multiplayer game by selecting a random word and hint from the provided list for both players.
 * Initializes the game state for both players with the same word and hint to ensure a fair game.
 * @param state1 Game state for player 1.
 * @param state2 Game state for player 2.
 * @param wordList Vector containing the list of wordItem structures to choose from.
 */
void multiplayerSetup(GameState& state1, GameState& state2, const vector<WordItem>& wordList) {
    int wordIndex = rand() % wordList.size(); // Randomly select a word from the list for both players
    state1.chosenWord = wordList[wordIndex].word;
    state1.chosenHint = wordList[wordIndex].hint;
    state2.chosenWord = wordList[wordIndex].word;
    state2.chosenHint = wordList[wordIndex].hint;
}

/**
 * @brief Manages the main multiplayer game loop, alternating turns between players.
 * Each player gets a chance to guess letters or the whole word, with the game updating and displaying
 * the state after each guess. The game continues until one or both players have guessed the word or exhausted their guesses.
 * @param wordList Vector of wordItem structures containing the words and hints for the game.
 */
void playMultiplayer(const vector<WordItem>& wordList) {
    int maxGuesses;
    setupDifficulty(maxGuesses);

    PlayerState player1{GameState(maxGuesses), "Player 1"}; // Construct player states with the same word and hint
    PlayerState player2{GameState(maxGuesses), "Player 2"};
    multiplayerSetup(player1.state, player2.state, wordList);

    bool playAgain;
    do {
        cout << "Welcome to Hangman Multiplayer!" << endl;
        bool gameActive = true;

        while (gameActive) {  // game loop for multiplayer mode, pointers to player states allow for easy iteration for player turns
            for (auto& currentPlayer : {&player1, &player2}) {
                if (!currentPlayer->state.wordGuessed && currentPlayer->state.incorrectGuesses < currentPlayer->state.maxGuesses) {
                    cout << currentPlayer->playerName << "'s turn." << endl;
                    displayGameState(currentPlayer->state);
                    processPlayerGuess(currentPlayer->state);
                    cout << "You have " << currentPlayer->state.maxGuesses - currentPlayer->state.incorrectGuesses << " incorrect guesses remaining." << endl;

                    if (currentPlayer->state.wordGuessed) {
                        multiplayerEndGameDisplay(*currentPlayer);
                        gameActive = false;  // End the game immediately if any player guesses correctly
                        break;
                    } else if (currentPlayer->state.incorrectGuesses >= currentPlayer->state.maxGuesses) {
                        multiplayerEndGameDisplay(*currentPlayer);
                    }
                }
            }
            // Continue if both players still have unguessed words and guesses left
            gameActive = gameActive && ((player1.state.incorrectGuesses < player1.state.maxGuesses && !player1.state.wordGuessed) ||
                                        (player2.state.incorrectGuesses < player2.state.maxGuesses && !player2.state.wordGuessed));
        }
        printMultiplayerStats(player1, player2);

        if (promptToPlayAgain()) {
            player1.state = GameState(maxGuesses);
            player2.state = GameState(maxGuesses);
            multiplayerSetup(player1.state, player2.state, wordList);
        } else {
            break;
        }
    } while (playAgain);
}

// =========== GAME LOOP ============ //

/**
 * @brief Manages the main game loop and mode selection for Hangman.
 * This central function controls the flow of the game, starting with mode selection and transitioning
 * between different game modes based on user choice. It continuously updates the game state and re-prompts
 * the mode selection until the exit condition is met. Each game mode utilizes a shared list of words
 * and hints for gameplay, ensuring consistency across game sessions.
 * The function concludes by thanking the player once they decide to exit the game.
 * @param wordList Vector of wordItem structures containing words and hints. This list is passed to game modes
 * to select words for the player(s) to guess.
 */
void playGame(const vector<WordItem>& wordList) {
    GameMode mode = modeMenu();  // Set the initial mode
    while (mode != EXIT_GAME) {
        switch (mode) {
            case SINGLE_PLAYER:
                playSingleplayer(wordList);
                break;
            case TWO_PLAYER:
                playMultiplayer(wordList);
                break;
            case INTERACTIVE_TWO_PLAYER:
                playInteractiveMultiplayer();
                break;
            case MANAGE_WORDLIST:
                manageWordList("data.csv");
                break;
            default:
                cout << "Returning to main menu." << endl;
                mode = modeMenu();
        }
        if (mode != EXIT_GAME) {
            mode = modeMenu();  // Prompt again after a mode completes unless exiting
        }
    }
    cout << "Thank you for playing!" << endl;
}