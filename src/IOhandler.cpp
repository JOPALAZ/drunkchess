#include "IOhandler.h"
#include "chess-peice-codes.h"
#include <unistd.h>

/**
 * @brief Checks for checkmate or stalemate conditions for a given side.
 *
 * Evaluates the board state to determine if the current side is in checkmate,
 * stalemate, or can still make valid moves.
 *
 * @param side Indicates the side to evaluate (true for white, false for black).
 * @param board Pointer to the 2D array (8x8) of ChessPieceBase pointers.
 * @param checkMate Reference to a Special_Parameter struct that will be filled
 *        with information about any discovered checkmate or restricted moves.
 * @return 
 * - 1 if there are valid moves available (no checkmate/stalemate).
 * - 0 if a stalemate is detected (no moves, not in check).
 * - -1 if a checkmate is detected.
 */
int patOrMate(bool side, ChessPieceBase ***board, Special_Parameter &checkMate) {
  // Evaluate the board to see if the king is in check and gather any restrictions.
  checkMate = ChessBoard::evaluateCheckMate(side, board);

  // Loop through all squares to find pieces belonging to the given side.
  for (int row = 0; row < BOARDSIZE; ++row) {
    for (int col = 0; col < BOARDSIZE; ++col) {
      if (board[row][col]->isWhite() == side) {
        // Collect attack candidates.
        std::vector<std::pair<int, int>> candidateMoves =
            board[row][col]->getAttackCandidates(false);

        // Also collect normal move candidates.
        for (std::pair<int, int> move : board[row][col]->getMoveCandidates()) {
          candidateMoves.push_back(move);
        }

        // If the piece is restricted because the king is in check or pinned.
        int id = ChessBoard::findFigureIndex(checkMate.restrictions, {row, col});
        if ((checkMate.kingAttacked || id != -1) &&
            board[row][col]->getCode() != KING) {
          // Filter out moves that are not allowed under the current check constraints.
          candidateMoves = ChessBoard::filterMoves(candidateMoves, checkMate, id);
        }

        // If we find any legal move, we can return immediately.
        if (!candidateMoves.empty()) {
          return 1; // Valid moves exist
        }
      }
    }
  }
  // If no valid moves are found, return negative or zero based on whether the king is attacked.
  // -1 indicates checkmate (kingAttacked == true), 0 indicates stalemate (kingAttacked == false).
  return -1 * checkMate.kingAttacked;
}

/**
 * @brief Converts the given string to lowercase in place.
 * @param str String to be transformed to lowercase.
 */
void IOhandler::toLowercase(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(),
                 [](unsigned char c) { return std::tolower(c); });
}

/**
 * @brief Constructor that initializes an IOhandler with specific streams and asks for logging preferences.
 * 
 * This constructor asks the user if they want to enable logging. Based on the user's response,
 * it sets up the Logger to either:
 * - be silent (no logging),
 * - log to stdout,
 * - or log to a specified file.
 *
 * @param output Pointer to the output stream used for user-facing messages.
 * @param input Pointer to the input stream used for reading user commands.
 */
IOhandler::IOhandler(std::ostream *output, std::istream *input)
    : output(output), input(input) {
  checkMate = {false, {}, {}};
  log = nullptr;
  ch = nullptr;

  std::string response;
  std::ostream *out = nullptr;
  bool silent;

  *output << "Do you want to log? [y/N] " << std::endl;
  std::getline(*input, response);

  silent = response.empty() || response[0] == 'N' || response[0] == 'n';

  // If user wants server logging
  if (!silent && response[0] == 'S') {
    server = true;
    out = new std::ofstream(std::to_string(getpid()) + ".log",
                            std::ofstream::out | std::ofstream::app);
    silent = false;
  } 
  // If not silent, ask the user where to log
  else if (!silent) {
    *output << "Where do you want to log? [standart/file] " << std::endl;
    std::getline(*input, response);
    toLowercase(response);

    if (response == "standart") {
      out = &std::cout;
    } else if (response == "file") {
      *output << "Enter filename (will be overwritten) " << std::endl;
      std::getline(*input, response);
      toLowercase(response);
      out = new std::ofstream(response, std::ofstream::out | std::ofstream::app);
    } else {
      *output << "Unknown input, assuming - standart" << std::endl;
      out = &std::cout;
    }
  }

  // Create the logger with the determined settings.
  log = new Logger(silent, out);
}

/**
 * @brief The main loop handling user commands and game flow.
 *
 * Continues to prompt for commands until the user chooses to exit or
 * an exception occurs.
 */
void IOhandler::mainLoop() {
  loop = true;
  bool ok = true;
  std::string response;

  while (loop) {
    try {
      // If in server mode and previous command was OK, send "OK" once
      // otherwise prompt user for command in non-server mode.
      response = server ? "OK" : "~ (help for help): ";
      if (!server || ok) {
        *output << response << std::endl;
      }

      std::getline(*input, response);
      toLowercase(response);
      processInput(response);
      ok = true;
    } catch (std::out_of_range &range) {
      if (!server) {
        *output << "YOUR COMMAND '" << response
                << "' WAS GIVEN WRONG: " << range.what() << std::endl;
      } else {
        *output << "NOT OK" << std::endl;
        std::cerr << range.what() << std::endl;
      }
      ok = false;
    } catch (std::invalid_argument &arg) {
      if (!server) {
        *output << "YOUR COMMAND '" << response
                << "' WAS GIVEN WRONG: " << arg.what() << std::endl;
      } else {
        *output << "NOT OK" << std::endl;
        std::cerr << arg.what() << std::endl;
      }
      ok = false;
    } catch (std::logic_error &logic) {
      if (!server) {
        *output << "YOUR COMMAND '" << response
                << "' WAS GIVEN WRONG: " << logic.what() << std::endl;
      } else {
        *output << "NOT OK" << std::endl;
        std::cerr << logic.what() << std::endl;
      }
      ok = false;
    } catch (std::exception &ex) {
      std::cerr << ex.what() << "UNABLE TO PROCEED" << std::endl;
      loop = false;
    }
  }
}

/**
 * @brief Retrieves a list of possible commands or moves given the current game state.
 * @return A vector of strings describing the commands/moves available.
 */
std::vector<std::string> IOhandler::getPossibleOptions() {
  std::vector<std::string> out;
  out.push_back("exit\t\t\texits the game");
  out.push_back("help\t\t\tdisplays this help");

  if (!gameIsOn) {
    out.push_back("start\t\t\tstarts a game");
  } else {
    out.push_back("move <start:end>\tperforms specified move");
    out.push_back("surrender\t\tyou instantly lose");
    out.push_back("print\t\t\tprints a board");
  }
  return out;
}

/**
 * @brief Processes a single user input (command) and performs the corresponding action.
 * @param response The user input.
 * @throws std::invalid_argument If the input command is not recognized.
 */
void IOhandler::processInput(const std::string &response) {
  std::string response_;
  if (response == "help") {
    for (const std::string &el : getPossibleOptions()) {
      *output << el << std::endl;
    }
  } else if (response == "exit") {
    loop = false;
    if (ch) {
      delete ch;
      ch = nullptr;
    }
  } else if (response == "dump" && gameIsOn) {
    dumpCurrentGamestate();
  } else if (!gameIsOn && response == "start") {
    response_ = server ? "OK" : "Chose a side [w/b]";
    *output << response_ << std::endl;
    std::getline(*input, response_);
    toLowercase(response_);

    if (response_[0] == 'w') {
      side = true;
      gameIsOn = startGame();
    } else if (response_[0] == 'b') {
      side = false;
      gameIsOn = startGame();
    } else {
      if (!server) {
        *output << "Unknown input " << std::endl;
      }
    }
  } else if (gameIsOn && response.size() == 10 && response.substr(0, 4) == "move") {
    move(response.substr(5, 10));
  } else if (gameIsOn && response.size() == 8 && response.substr(0, 5) == "moves") {
    printMoveCandidates(response.substr(6, 8));
  } else if (gameIsOn && response == "surrender") {
    if (ch) {
      delete ch;
      ch = nullptr;
    }
    if (log) {
      log->log("PLAYER SURRENDERED");
    }
    *output << "You lost!!!" << std::endl;
    checkMate = {false, {}, {}};
    gameIsOn = false;
  } else if (response == "prestart") {
    gameIsOn = startPreDefinedGame();
  } else if (response == "set params") {
    setParams();
  } else if (gameIsOn && response == "print") {
    printBoard();
  } else {
    throw std::invalid_argument("Unknown input ");
  }
}

/**
 * @brief Starts a predefined game using a string-based board setup.
 * 
 * Reads a line from the input for difficulty and side, then reconstructs
 * the game board from a string representation.
 *
 * @return True if the game starts successfully, false otherwise.
 */
bool IOhandler::startPreDefinedGame() {
  int difficulty;
  if (ch) {
    delete ch;
    ch = nullptr;
  }
  checkMate = {false, {}, {}};

  if (server) {
    *output << "OK" << std::endl;
  }

  std::string response_;
  std::getline(*input, response_);
  difficulty = 1;
  // Determine side from last character, expecting '0' or '1'
  side = response_.back() - '0';
  ch = new ChessBoard(log, difficulty);
  ch->makeBoardFromString(response_);
  difficulty = ch->getDifficulty();

  if (!server) {
    printBoard();
  }

  if (log) {
    std::string color = side ? "WHITE" : "BLACK";
    log->log("PREDEFINED GAME STARTED WITH DIFFICULTY " +
             std::to_string(difficulty) + " AND PLAYER IS " + color);
  }

  return ch != nullptr;
}

/**
 * @brief Converts a two-digit string (like "47") into board coordinates based on the current player side.
 * 
 * For side = true (white), "47" is interpreted as row=4, col=7.
 * For side = false (black), the same string is mirrored to row=7-4, col=7-7.
 *
 * @param str The position string; must be exactly 2 characters, each digit 0-7.
 * @return A pair (row, col) with valid board coordinates.
 * @throws std::out_of_range If the input string does not have size 2 
 *         or cannot be converted to integers.
 */
std::pair<int, int> IOhandler::transcodePosition(std::string str) {
  if (str.size() != 2) {
    throw std::out_of_range("SIZE MUST BE 2");
  }

  int number = std::stoi(str);
  int col = number % 10;
  int row = number / 10;

  // Mirror coordinates if side == false
  if (side) {
    return {col, row};
  } else {
    return {7 - col, 7 - row};
  }
}

/**
 * @brief Starts a new game from scratch, asking for a difficulty level.
 * 
 * @return True if a valid difficulty is set and the board is initialized, false otherwise.
 */
bool IOhandler::startGame() {
  if (ch) {
    delete ch;
    ch = nullptr;
  }
  checkMate = {false, {}, {}};

  std::string response_ =
      server ? "OK" : "Chose a difficulty [1-10 (more is not recomended)]";
  *output << response_ << std::endl;
  std::getline(*input, response_);
  toLowercase(response_);

  int difficulty = std::stoi(response_);
  if (difficulty >= 1) {
    ch = new ChessBoard(log, difficulty);
    if (ch && !server) {
      printBoard();
    }
  }

  if (log) {
    std::string color = side ? "WHITE" : "BLACK";
    log->log("GAME STARTED WITH DIFFICULTY " + std::to_string(difficulty) +
             "AND PLAYER IS " + color);
  }

  return ch != nullptr;
}

/**
 * @brief Outputs the current game state to std::cout, including piece info and AI parameters.
 *
 * This method serializes various parameters, including board arrangement and any custom
 * scoring factors, into a single string, which it then prints to std::cout.
 */
void IOhandler::dumpCurrentGamestate() {
  int i, j;
  std::string outData;
  short val;
  for (i = 0; i < BOARDSIZE; ++i) {
    for (j = 0; j < BOARDSIZE; ++j) {
      val = ChessPieceBase::getSymb(ch->getBoard()[i][j]->getCode());
      val = val << 3;
      val += ch->getBoard()[i][j]->isWhite() << 1;
      val += ch->getBoard()[i][j]->hasMoved();
      outData += std::to_string(val) + ' ';
    }
  }

  // Serialize global parameter values (presumably defined elsewhere).
  for (i = 0; i < 8; ++i) {
    outData += std::to_string(prices[i]) + ' ';
  }
  outData += std::to_string(Pate) + ' ';
  outData += std::to_string(Mate) + ' ';
  outData += std::to_string(FirstMove) + ' ';
  outData += std::to_string(Castling) + ' ';
  outData += std::to_string(ATTACK_COST) + ' ';
  outData += std::to_string(worth) + ' ';
  outData += std::to_string(ch->getDifficulty()) + ' ';
  outData += std::to_string(side);

  std::cout << outData << std::endl;
}

/**
 * @brief Allows user to modify several game-related parameters, such as piece prices and AI constants.
 *
 * Reads values from the input stream one by one, prompting either to the user or, in server mode,
 * simply printing "OK" on success.
 */
void IOhandler::setParams() {
  std::string response;
  std::string figureNames[] = {"KING",   "QUEEN", "ROOK", "BISHOP",
                               "KNIGHT", "PAWN",  "EMPTY"};
  int i;

  if (server) {
    *output << "OK" << std::endl;
  }
  try {
    // Prices for 7 piece types (KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN, EMPTY)
    for (i = 0; i < 7; ++i) {
      if (!server) {
        *output << "Price for " + figureNames[i] << std::endl;
      }
      std::getline(*input, response);
      prices[i] = std::stoi(response);
      if (server) {
        *output << "OK" << std::endl;
      }
    }

    if (!server) {
      *output << "Price for Mate" << std::endl;
    }
    std::getline(*input, response);
    Mate = std::stoi(response);
    if (server) {
      *output << "OK" << std::endl;
    } else {
      *output << "Price for Pate" << std::endl;
    }
    std::getline(*input, response);
    Pate = std::stoi(response);
    if (server) {
      *output << "OK" << std::endl;
    } else {
      *output << "Price for First Move" << std::endl;
    }
    std::getline(*input, response);
    FirstMove = std::stoi(response);
    if (server) {
      *output << "OK" << std::endl;
    } else {
      *output << "Price for Castling" << std::endl;
    }
    std::getline(*input, response);
    Castling = std::stoi(response);
    if (server) {
      *output << "OK" << std::endl;
    } else {
      *output << "Price for Attack Cost" << std::endl;
    }
    std::getline(*input, response);
    ATTACK_COST = std::stof(response);
    if (server) {
      *output << "OK" << std::endl;
    } else {
      *output << "Price for Worth of predictions" << std::endl;
    }
    std::getline(*input, response);
    worth = std::stof(response);

  } catch (...) {
    std::cerr << "INVALID VALUE";
  }
}

/**
 * @brief Executes a move command (e.g., "move 12:34") or triggers AI move if the command is "enemy".
 *
 * Moves the piece from start to end if valid, checks for checkmate or stalemate,
 * and handles finishing the game if necessary. If "enemy" is specified, calls the AI to move.
 *
 * @param move The input string with move coordinates or the word "enemy".
 * @throws std::logic_error If the move is not valid under current game constraints.
 */
void IOhandler::move(const std::string &move) {
  Move mv;
  Move bestMove;
  bool isGood = true;
  int id;

  // If "enemy" is specified, let the AI move.
  if (move == "enemy") {
    try {
      bestMove = ch->getBestMove(!this->side);
      if (bestMove.start.first == -1) {
        // If AI has no moves, check whether it's checkmate or stalemate.
        if (ChessBoard::simplifiedEvaluateCheckMate(
                !this->side, ChessBoard::findKing(!this->side, ch->getBoard()),
                ch->getBoard())) {
          if (log) {
            log->log("COMPUTER LOST");
          }
          *output << "YOU WON!!!" << std::endl;
        } else {
          if (log) {
            log->log("TIE");
          }
          *output << "TIE!!!" << std::endl;
        }
        printBoard();
        if (ch) {
          delete ch;
          ch = nullptr;
        }
        checkMate = {false, {}, {}};
        gameIsOn = false;
        return;
      }

      ch->performMove(bestMove, ch->getBoard(), nullptr, true);
      if (log) {
        log->log("COMPUTER MOVED: " + Logger::moveToString(bestMove));
      }

      id = patOrMate(side, ch->getBoard(), checkMate);
      if (id == -1) {
        if (log) {
          log->log("PLAYER LOST");
        }
        *output << "YOU LOST!!!" << std::endl;
      } else if (id == 0) {
        if (log) {
          log->log("TIE");
        }
        *output << "TIE!!!" << std::endl;
      }

      if (id != 1) {
        printBoard();
        if (ch) {
          delete ch;
          ch = nullptr;
        }
        checkMate = {false, {}, {}};
        gameIsOn = false;
        return;
      }
    } catch (...) {
      if (log) {
        log->log("COMPUTER DECIDED TO SURRENDER");
      }
      *output << "ENEMY DECIDED TO SURRENDER -> YOU WON!!!" << std::endl;
      printBoard();
      if (ch) {
        delete ch;
        ch = nullptr;
      }
      checkMate = {false, {}, {}};
      gameIsOn = false;
      return;
    }
    return;
  }

  // Otherwise, parse the user's move coordinates.
  mv = {transcodePosition(move.substr(0, 2)),
        transcodePosition(move.substr(3, 5))};

  // Check if the piece belongs to the current player and is not empty.
  if (ch->getBoard()[mv.start.first][mv.start.second]->isWhite() == this->side &&
      ch->getBoard()[mv.start.first][mv.start.second]->getCode() != EMPTY) {

    // Prevent moves to the same square.
    if (mv.start == mv.end) {
      throw std::logic_error("INVALID MOVE");
    }

    // Evaluate checkmate restrictions for current side.
    checkMate = ChessBoard::evaluateCheckMate(this->side, ch->getBoard());
    id = ChessBoard::findFigureIndex(checkMate.restrictions, mv.start);

    // If the piece is pinned or the king is under attack, verify that the chosen move is valid.
    if (id != -1) {
      isGood = std::find(
                   checkMate.restrictions.at(id).unrestrictedPositions.begin(),
                   checkMate.restrictions.at(id).unrestrictedPositions.end(),
                   mv.end) != checkMate.restrictions.at(id).unrestrictedPositions.end();
    }
    if (checkMate.kingAttacked && isGood) {
      isGood = std::find(checkMate.saveKingPath.begin(),
                         checkMate.saveKingPath.end(),
                         mv.end) != checkMate.saveKingPath.end();
    }

    // A king is free to move if the move is valid for the king, ignoring certain pinned constraints.
    if (isGood || ch->getBoard()[mv.start.first][mv.start.second]->getCode() == KING) {
      try {
        ch->performMove(mv, ch->getBoard(), this);
        if (log) {
          log->log("PLAYER MOVED: " + Logger::moveToString(mv));
        }
        // If not in server mode, let the AI respond immediately.
        if (!server) {
          this->move("enemy");
        }
      } catch (std::logic_error &le) {
        throw std::logic_error("YOU CAN'T MOVE THERE");
      }
    } else {
      throw std::logic_error("CANT MOVE THERE, KING IS ATTACKED");
    }
  } else {
    throw std::logic_error("YOU CAN'T MOVE THAT FIGURE");
  }
}

/**
 * @brief Asks the user for a replacement piece (e.g., pawn promotion).
 * 
 * If the side matches the current player side, the user is prompted for a character code.
 * If not, defaults to QUEEN for the opponent side.
 *
 * @param side True if white, false if black.
 * @return The chosen ChessPieceCode, or QUEEN by default if the input is invalid.
 */
ChessPieceCode IOhandler::askReplacement(bool side) {
  std::string response;
  ChessPieceCode code;

  if (side == this->side) {
    *output << "CODE?" << std::endl;
    std::getline(*input, response);
    code = ChessPieceBase::getPieceCode(response[0]);
    if (code == NONE) {
      code = QUEEN;
    }
  } else {
    code = QUEEN;
  }
  return code;
}

/**
 * @brief Prints possible move coordinates for a piece at a given start square.
 * 
 * Converts the board coordinates to the user-friendly format (based on side)
 * and prints them to the output.
 *
 * @param start Two-digit string (like "12") representing the initial position.
 */
void IOhandler::printMoveCandidates(std::string start) {
  std::pair<int, int> pos = transcodePosition(start);
  std::vector<std::pair<int, int>> candidates =
      ch && ch->getBoard()[pos.first][pos.second]->isWhite() == side
          ? getMoveCandidates(pos)
          : std::vector<std::pair<int, int>>{};

  if (candidates.empty()) {
    *output << "NONE" << std::endl;
  } else {
    int counter = 0;
    for (std::pair<int, int> el : candidates) {
      int row, col;
      // Convert back for printing if the user plays black (reverse board).
      if (side) {
        col = el.first;
        row = el.second;
      } else {
        col = 7 - el.first;
        row = 7 - el.second;
      }
      ++counter;
      *output << row << col;
      if (counter != (int)candidates.size()) {
        *output << ',';
      }
    }
    *output << std::endl;
  }
}

/**
 * @brief Collects combined move and attack candidates for a piece at the given position.
 * 
 * Takes into account any checkmate restrictions (like pinned pieces, or mandatory moves 
 * to save the king).
 *
 * @param start Pair of integers (row, col) indicating the position of the piece.
 * @return A vector of valid (row, col) moves for the piece at @p start.
 */
std::vector<std::pair<int, int>>
IOhandler::getMoveCandidates(std::pair<int, int> start) {
  std::vector<std::pair<int, int>> out;
  int id = ChessBoard::findFigureIndex(checkMate.restrictions, start);

  // Collect moves from attackCandidates and moveCandidates.
  std::vector<std::pair<int, int>> attacks =
      ch->getBoard()[start.first][start.second]->getAttackCandidates(false);
  out.insert(out.end(), attacks.begin(), attacks.end());

  std::vector<std::pair<int, int>> moves =
      ch->getBoard()[start.first][start.second]->getMoveCandidates();
  out.insert(out.end(), moves.begin(), moves.end());

  // If the king is attacked and the piece is not a king, only allow moves that save the king.
  if (checkMate.kingAttacked &&
      ch->getBoard()[start.first][start.second]->getCode() != KING) {
    out = ChessBoard::getOverlap(out, checkMate.saveKingPath);
  }

  // If there's a restriction on the piece (e.g., pinned), intersect with restricted moves.
  if (id != -1 &&
      ch->getBoard()[start.first][start.second]->getCode() != KING) {
    out = ChessBoard::getOverlap(
        out, checkMate.restrictions.at(id).unrestrictedPositions);
  }

  return out;
}

/**
 * @brief Prints the current state of the chessboard.
 *
 * Uses ChessBoard's built-in print function to display the board layout,
 * reflecting which side is currently in control (if needed).
 */
void IOhandler::printBoard() {
  if (ch) {
    ch->printBoard(side, output, server);
  }
}

/**
 * @brief Default constructor that sets IOhandler to use std::cout and std::cin for I/O, with silent logging.
 */
IOhandler::IOhandler() : output(&std::cout), input(&std::cin) {
  log = new Logger(true, nullptr);
}

/**
 * @brief Destructor that cleans up the logger and the chessboard (if any).
 */
IOhandler::~IOhandler() {
  if (log) {
    delete log;
  }
  if (ch) {
    delete ch;
  }
}
