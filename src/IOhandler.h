#pragma once

#include <algorithm>
#include <fstream>
#include <vector>

#include "chess-board.h"
#include "logger.h"

/**
 * @class IOhandler
 * @brief Handles input and output operations for a chess game, as well as game flow control.
 */
class IOhandler {
private:
  /**
   * @brief Pointer to a Logger instance for logging events.
   */
  Logger *log;

  /**
   * @brief Pointer to a ChessBoard instance representing the game board.
   */
  ChessBoard *ch;

  /**
   * @brief Output stream to which messages are written.
   */
  std::ostream *output;

  /**
   * @brief Input stream from which commands are read.
   */
  std::istream *input;

  /**
   * @brief Holds special information regarding checkmate or game termination.
   */
  Special_Parameter checkMate;

  /**
   * @brief Indicates whether the game is currently active.
   */
  bool gameIsOn = false;

  /**
   * @brief Controls the main loop execution.
   */
  bool loop = true;

  /**
   * @brief Indicates whether the handler is running in server mode.
   */
  bool server = false;

  /**
   * @brief Represents which side (true for white, false for black) is in play if needed.
   */
  bool side;

  /**
   * @brief Initializes and starts a new game with default settings.
   * @return True if the game starts successfully, false otherwise.
   */
  bool startGame();

  /**
   * @brief Initializes and starts a new game from a predefined state (if available).
   * @return True if the predefined game starts successfully, false otherwise.
   */
  bool startPreDefinedGame();

  /**
   * @brief Attempts to perform a move based on the provided move string.
   * @param move String describing the move (e.g., "e2e4").
   */
  void move(const std::string &move);

  /**
   * @brief Retrieves a list of possible commands or moves at the current state.
   * @return A vector of strings representing valid commands or moves.
   */
  std::vector<std::string> getPossibleOptions();

  /**
   * @brief Processes the user input string and delegates to the appropriate handler.
   * @param response A string containing the user command.
   */
  void processInput(const std::string &response);

  /**
   * @brief Gets valid move candidates for a piece at the given starting coordinate.
   * @param start A pair (x, y) representing the starting position.
   * @return A vector of (x, y) pairs representing valid move targets.
   */
  std::vector<std::pair<int, int>> getMoveCandidates(std::pair<int, int> start);

  /**
   * @brief Prints valid move candidates for a piece at the given starting coordinate.
   * @param start A string (e.g., "e2") that will be converted to coordinates.
   */
  void printMoveCandidates(std::string start);

  /**
   * @brief Sets various parameters needed for the game setup or continuation.
   */
  void setParams();

  /**
   * @brief Prints the current state of the board to the output stream.
   */
  void printBoard();

  /**
   * @brief Writes the current game state (e.g., board layout, move history) to a file or log.
   */
  void dumpCurrentGamestate();

  /**
   * @brief Converts a chess notation-like string (e.g., "e2") into board coordinates.
   * @param str The notation string.
   * @return A pair (x, y) representing board coordinates, or (-1, -1) if invalid.
   */
  std::pair<int, int> transcodePosition(std::string str);

public:
  /**
   * @brief Converts the given string to lowercase.
   * @param str Reference to the string to be modified.
   */
  static void toLowercase(std::string &str);

  /**
   * @brief Asks for a piece replacement (e.g., pawn promotion) based on side color.
   * @param side True if it's white's turn, false if black's.
   * @return A ChessPieceCode representing the chosen replacement piece.
   */
  ChessPieceCode askReplacement(bool side);

  /**
   * @brief Main loop that handles the game's progression and user interaction.
   */
  void mainLoop();

  /**
   * @brief Constructs an IOhandler with specific output and input streams.
   * @param output Pointer to an output stream.
   * @param input Pointer to an input stream.
   */
  IOhandler(std::ostream *output, std::istream *input);

  /**
   * @brief Default constructor, may require manual setup of streams later.
   */
  IOhandler();

  /**
   * @brief Destructor.
   */
  ~IOhandler();
};
