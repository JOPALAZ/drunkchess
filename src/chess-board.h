#pragma once

#include "chess-peice.h"
#include <future>
#include <map>
#include <set>
#include <sstream>
#include <thread>

class IOhandler;

/**
 * @struct Move
 * @brief Represents a move in chess, consisting of a starting coordinate and an ending coordinate.
 */
struct Move {
  /// The starting square (row, column).
  std::pair<int, int> start;
  /// The ending square (row, column).
  std::pair<int, int> end;
};

/**
 * @struct Move_Candidate
 * @brief Associates a chess move with a floating-point score or delta.
 */
struct Move_Candidate {
  Move move;     ///< The chess move being considered.
  float dScore;  ///< The score or change in score attributed to this move.
};

/**
 * @struct Figure_Move_Restriction
 * @brief Restricts the valid moves of a specific piece (e.g., when pinned or saving the king).
 */
struct Figure_Move_Restriction {
  /// The board position of the restricted piece.
  std::pair<int, int> position;
  /// The list of positions where this piece can legally move, given the restrictions.
  std::vector<std::pair<int, int>> unrestrictedPositions;
};

/**
 * @struct Special_Parameter
 * @brief Holds information related to special conditions such as checks, pins, or forced moves.
 */
struct Special_Parameter {
  bool kingAttacked;                           ///< Indicates if the king is currently under attack.
  std::vector<std::pair<int, int>> saveKingPath; ///< The squares that would resolve a check situation.
  std::vector<Figure_Move_Restriction> restrictions; ///< Movement restrictions for certain pieces.
};

/**
 * @struct Thread_Parameter
 * @brief Used for multithreading operations in AI or move calculation.
 */
struct Thread_Parameter {
  ChessPieceBase ***board; ///< The chessboard on which operations are performed.
  bool white;              ///< Indicates if this parameter applies to white (true) or black (false).
  int difficulty;          ///< The difficulty level for AI computations.
  int depth;               ///< Current search depth.
  int maxDepth;            ///< Maximum search depth.
  bool ready;              ///< Flag indicating if the thread is ready to start or has completed.
  float score;             ///< The resulting score from this thread's computations.
};

/**
 * @class ChessBoard
 * @brief Manages a chessboard, its pieces, and associated game logic.
 *
 * This class provides methods for creating, copying, and deleting a board,
 * printing it, evaluating moves, determining best moves (for AI), and
 * various utilities to handle check and checkmate logic, pinned pieces,
 * and other chess-specific features.
 */
class ChessBoard {
private:
  /**
   * @brief Creates a row filled with pawns for the specified color.
   * @param white True if creating a row of white pawns, false if black.
   * @return A pointer to an array of ChessPieceBase pointers representing one row.
   */
  ChessPieceBase **createPawnRow(bool white);

  /**
   * @brief Creates an empty row of 8 squares.
   * @param row The row index to initialize (possibly for debugging or clarity).
   * @return A pointer to an array of ChessPieceBase pointers representing one empty row.
   */
  ChessPieceBase **createEmptyRow(int row);

  /**
   * @brief Creates the first (or last) row for a color (Rook, Knight, Bishop, Queen, King, etc.).
   * @param white True if creating pieces for white side, false if black side.
   * @return A pointer to an array of ChessPieceBase pointers for that back rank.
   */
  ChessPieceBase **createFirstRow(bool white);

protected:
  Logger *log;               ///< Pointer to a Logger instance for logging.
  ChessPieceBase ***board;   ///< 2D array (8x8) representing the board.
  int difficulty;            ///< Difficulty level for AI.
  int maxDepth;              ///< Maximum search depth for AI or game logic.

  /**
   * @brief Recursive evaluation function for AI or search algorithms.
   * @param board The board on which to perform the search.
   * @param white True if evaluating for white, false otherwise.
   * @param difficulty Difficulty level that might influence search pruning.
   * @param depth Current depth in the recursive search.
   * @param maxDepth Maximum search depth to stop recursion.
   * @param worth Additional evaluation parameter for weighting.
   * @return A float score representing the evaluation of the board.
   */
  static const float recursiveSubroutine(ChessPieceBase ***board, bool white,
                                         int difficulty, int depth,
                                         int maxDepth, float worth);

  /**
   * @brief Thread function to perform parallel computations in certain AI scenarios.
   * @param param Pointer to a Thread_Parameter containing relevant info for the thread.
   */
  static void threadFunc(Thread_Parameter *param);

  /**
   * @brief Asks for a replacement piece (e.g., when a pawn reaches the last rank).
   * @param side True if white, false if black.
   * @param handler Pointer to an IOhandler for user interaction (may be null if AI).
   * @return The ChessPieceCode chosen for the replacement piece.
   */
  static ChessPieceCode askReplacement(bool side, IOhandler* handler);

  /**
   * @brief Handles an attack move, potentially capturing an opponent's piece.
   * @param move The move describing start and end squares.
   * @param board The board on which the move occurs.
   * @param handler Optional pointer to IOhandler for user interactions (pawn promotion, etc.).
   * @return A float representing the score impact of this attack move.
   */
  static float performAttack(const Move &move, ChessPieceBase ***board,
                             IOhandler* handler);

  /**
   * @brief Handles a normal (non-capturing) move.
   * @param move The move describing start and end squares.
   * @param board The board on which the move occurs.
   * @param handler Optional pointer to IOhandler for user interactions.
   * @return A float representing the score impact of this normal move.
   */
  static float performNormalMove(const Move &move, ChessPieceBase ***board,
                                 IOhandler* handler);

  /**
   * @brief Handles castling if the move is recognized as a castling attempt.
   * @param move The move describing start and end squares (castling logic inside).
   * @param board The board on which the move occurs.
   * @param handler Optional pointer to IOhandler for user interactions.
   * @return A float representing the score impact of castling.
   */
  static float performCastling(const Move &move, ChessPieceBase ***board,
                               IOhandler* handler);

public:
  /**
   * @brief Searches for a piece in the given restrictions vector.
   * @param restrictions A vector of Figure_Move_Restriction objects to search.
   * @param pos The position (row, column) of the piece.
   * @return The index of the piece in the vector, or -1 if not found.
   */
  static int findFigureIndex(const std::vector<Figure_Move_Restriction> &restrictions,
                             std::pair<int, int> pos);

  /**
   * @brief Determines if a piece is dangerous for the king based on directional checks.
   * @param distance The distance from the suspect piece to the king.
   * @param kingPos Position of the king.
   * @param dX Horizontal direction of interest.
   * @param dY Vertical direction of interest.
   * @param suspect Pointer to the piece that might be threatening the king.
   * @return True if the suspect piece is dangerous in that direction, false otherwise.
   */
  static bool isDangerous(int distance, std::pair<int, int> kingPos, int8_t dX,
                          int8_t dY, ChessPieceBase *suspect);

  /**
   * @brief Factory method to create a new chess piece based on the given parameters.
   * @param x The row coordinate.
   * @param y The column coordinate.
   * @param color True if white, false if black.
   * @param code The piece code (KING, QUEEN, etc.).
   * @param log Pointer to a Logger instance.
   * @param board Pointer to the board array.
   * @param moved_ Indicates whether the piece has moved before.
   * @return A pointer to the newly created ChessPieceBase.
   */
  static ChessPieceBase *createPeice(int x, int y, bool color,
                                     ChessPieceCode code, Logger *log,
                                     ChessPieceBase ***board,
                                     bool moved_ = false);

  /**
   * @brief Reverts the board from an imaginary state to a real state.
   * @param imgainaryBoard Source board to revert from.
   * @param board Destination board to restore to.
   */
  static void revertBoard(ChessPieceBase ***imgainaryBoard,
                          ChessPieceBase ***board);

  /**
   * @brief Deletes a dynamically allocated chessboard array.
   * @param board The board to delete.
   * @return A null pointer (for convenience in assignment).
   */
  static ChessPieceBase ***deleteBoard(ChessPieceBase ***board);

  /**
   * @brief Creates a deep copy of the given board.
   * @param board The source board to copy.
   * @param notImaginary If true, indicates a full copy for the main game state, otherwise for simulations.
   * @return A pointer to the new copy of the board.
   */
  static ChessPieceBase ***copyBoard(ChessPieceBase ***board,
                                     bool notImaginary = false);

  /**
   * @brief Performs a move on the given board, potentially involving captures or special moves.
   * @param move The move to perform.
   * @param board The board on which the move occurs.
   * @param handler Optional IOhandler for user interactions (pawn promotion, etc.).
   * @param overrideRightess If true, bypass certain checks to force a move (used by AI or internal logic).
   * @return A float representing the immediate score impact of the move.
   */
  static float performMove(const Move &move, ChessPieceBase ***board,
                           IOhandler* handler, bool overrideRightess = false);

  /**
   * @brief Prints a representation of an imaginary board to the specified output stream.
   * @param board The board to print.
   * @param out The output stream to print to (default is std::cout).
   */
  static void printImaginaryBoard(ChessPieceBase ***board,
                                  std::ostream *out = &std::cout);

  /**
   * @brief Filters a set of candidate moves based on check or pin restrictions.
   * @param input The original list of candidate positions.
   * @param checkMate Special_Parameter with info about king attacks and pins.
   * @param usedIndex The index of the piece in checkMate.restrictions.
   * @return A list of valid candidate positions after applying restrictions.
   */
  static std::vector<std::pair<int, int>>
  filterMoves(const std::vector<std::pair<int, int>> &input,
              Special_Parameter checkMate, int usedIndex);

  /**
   * @brief Evaluates if the side's king is in checkmate or if there are pinned pieces, etc.
   * @param side True if evaluating for white, false if black.
   * @param board The current board.
   * @return A Special_Parameter struct describing the check/pin state.
   */
  static Special_Parameter evaluateCheckMate(bool side, ChessPieceBase ***board);

  /**
   * @brief Simplified checkmate evaluation to see if a side's king is under check and has no moves.
   * @param side True if white, false if black.
   * @param kingPosition The position of the king on the board.
   * @param board The board to evaluate.
   * @return True if the side is in checkmate, false otherwise.
   */
  static bool simplifiedEvaluateCheckMate(bool side,
                                          std::pair<int, int> kingPosition,
                                          ChessPieceBase ***board);

  /**
   * @brief Finds the position of the king for the specified side.
   * @param side True if white, false if black.
   * @param board The board to search.
   * @return A pair (row, col) representing the king's position, or (-1, -1) if not found.
   */
  static std::pair<int, int> findKing(bool side, ChessPieceBase ***board);

  /**
   * @brief Creates a chess piece from a string-based code (used in loading or string-based setups).
   * @param x Row coordinate.
   * @param y Column coordinate.
   * @param color True if white, false if black.
   * @param code The character code representing the piece.
   * @param log Pointer to a Logger instance for logging.
   * @param board Pointer to the board.
   * @param moved_ Indicates if the piece has already moved.
   * @return A pointer to the created ChessPieceBase.
   */
  static ChessPieceBase *createPeiceFromString(int x, int y, bool color,
                                               char code, Logger *log,
                                               ChessPieceBase ***board,
                                               bool moved_ = false);

  /**
   * @brief Returns the intersection (overlap) of two position sets.
   * @param el1 First vector of positions.
   * @param el2 Second vector of positions.
   * @return A vector of positions that appear in both el1 and el2.
   */
  static std::vector<std::pair<int, int>>
  getOverlap(const std::vector<std::pair<int, int>> &el1,
             const std::vector<std::pair<int, int>> &el2);

  /**
   * @brief Getter for the difficulty level.
   * @return The current difficulty setting.
   */
  int getDifficulty() { return difficulty; }

  /**
   * @brief Rebuilds the board from a string representation (for predefined games or test states).
   * @param str The string containing board data.
   */
  void makeBoardFromString(const std::string &str);

  /**
   * @brief Prints the board to the specified output stream, taking side orientation into account.
   * @param white True if the player is white, which may flip how the board is printed.
   * @param out The output stream to print to (default is std::cout).
   * @param server If true, modifies printing to suit server-based formatting (optional).
   */
  void printBoard(bool white, std::ostream *out = &std::cout, bool server = false);

  /**
   * @brief Determines the best move for a given side using the current difficulty.
   * @param white True if calculating the best move for white, false for black.
   * @return A Move struct describing the best move found.
   */
  Move getBestMove(bool white);

  /**
   * @brief Getter for the internal board representation.
   * @return A pointer to the 2D array (8x8) of ChessPieceBase pointers.
   */
  ChessPieceBase ***getBoard() { return board; }

  /**
   * @brief Debug function to print advanced data about the board, possibly threats or pinned pieces.
   */
  void debugPrintDanger();

  /**
   * @brief Constructs a ChessBoard with a logger and a difficulty level.
   * @param log Pointer to a Logger for event logging.
   * @param difficulty Difficulty level for AI computations.
   */
  ChessBoard(Logger *log, int difficulty);

  /**
   * @brief Retrieves all squares on which the opponent can exert immediate danger.
   * @param board The board to analyze.
   * @param white True if analyzing dangerous points for white, false for black.
   * @return A set of board positions that are under threat.
   */
  static std::set<std::pair<int, int>>
  getDangerousPoints(ChessPieceBase ***board, bool white);

  /**
   * @brief Clears the board or resets it to an empty state (implementation depends on usage).
   */
  void clear();

  /**
   * @brief Cycles a piece at a given position to a new piece (for example, for testing).
   * @param pos The position of the piece to change.
   * @param color True if white, false if black.
   * @param code The new piece code to place.
   */
  void cycleFigure(std::pair<int, int> pos, bool color, ChessPieceCode code);

  /**
   * @brief Destructor that performs any necessary cleanup.
   */
  ~ChessBoard();
};
