#pragma once

#include "chess-peice-codes.h"
#include "logger.h"
#include <algorithm>
#include <memory>
#include <set>
#include <vector>

/**
 * @brief The size of a standard chessboard side.
 */
#define BOARDSIZE 8

/**
 * @brief The maximum allowed recursion depth (for example, in AI search).
 */
#define MAXDEPTH 256

/**
 * @class ChessPieceBase
 * @brief Base class for all chess pieces.
 *
 * This class provides common fields and methods for any chess piece:
 * piece code, color, status flags, coordinates, a logger instance, and a pointer 
 * to the board. It also declares virtual methods to retrieve possible moves and attacks.
 */
class ChessPieceBase {
protected:
  /**
   * @brief The code of this piece (from ChessPieceCode enumeration).
   */
  ChessPieceCode code = NONE;

  /**
   * @brief Indicates whether the piece is white (true) or black (false).
   */
  bool white = true;

  /**
   * @brief Indicates whether the piece is currently playable (active).
   */
  bool playable = false;

  /**
   * @brief Indicates whether this piece has been moved at least once.
   */
  bool moved = false;

  /**
   * @brief Pointer to a logger instance for logging moves or events.
   */
  Logger *log = nullptr;

  /**
   * @brief Pointer to the 2D array (board) of ChessPieceBase pointers.
   */
  ChessPieceBase ***board = nullptr;

  /**
   * @brief The current x-coordinate of the piece on the board.
   */
  int x = -1;

  /**
   * @brief The current y-coordinate of the piece on the board.
   */
  int y = -1;

public:
  /**
   * @brief Returns the single-character symbol for a given piece code.
   * @param code The piece code.
   * @return The symbol representing the piece.
   */
  static char getSymb(ChessPieceCode code);

  /**
   * @brief Returns the piece code for a given symbol.
   * @param chr The character symbol.
   * @return The ChessPieceCode corresponding to the symbol.
   */
  static ChessPieceCode getPieceCode(char chr);

  /**
   * @brief Moves the piece to the specified board coordinates.
   * @param dest A pair of integers representing the (x, y) coordinates.
   */
  void move(const std::pair<int, int> &dest);

  /**
   * @brief Getter for the current x-coordinate of the piece.
   * @return The current x-coordinate.
   */
  int getX() { return x; }

  /**
   * @brief Getter for the current y-coordinate of the piece.
   * @return The current y-coordinate.
   */
  int getY() { return y; }

  /**
   * @brief Provides access to the board pointer.
   * @return A triple-pointer to ChessPieceBase.
   */
  ChessPieceBase ***getBoard() { return board; }

  /**
   * @brief Provides access to the logger instance.
   * @return A pointer to Logger.
   */
  Logger *getLogger() { return log; }

  /**
   * @brief Indicates if the piece has moved at least once.
   * @return True if moved, false otherwise.
   */
  bool hasMoved() { return moved; }

  /**
   * @brief Retrieves the code of this piece.
   * @return The piece code.
   */
  ChessPieceCode getCode();

  /**
   * @brief Indicates if this is a white piece.
   * @return True if white, false if black.
   */
  bool isWhite();

  /**
   * @brief Default constructor.
   */
  ChessPieceBase() = default;

  /**
   * @brief Retrieves all possible move positions for this piece.
   * @return A vector of (x, y) pairs representing valid moves.
   */
  virtual std::vector<std::pair<int, int>> getMoveCandidates() = 0;

  /**
   * @brief Retrieves all possible attack positions for this piece.
   * @param all If true, considers every potential attack path or position.
   * @return A vector of (x, y) pairs representing valid attacks.
   */
  virtual std::vector<std::pair<int, int>> getAttackCandidates(bool all) = 0;

  /**
   * @brief Checks if the piece can move to a specific position.
   * @param pnt The target (x, y) coordinates.
   * @return True if moving to pnt is valid, false otherwise.
   */
  virtual bool canMoveTo(const std::pair<int, int> &pnt);

  /**
   * @brief Checks if the piece can attack a specific position.
   * @param pnt The target (x, y) coordinates.
   * @return True if attacking pnt is valid, false otherwise.
   */
  virtual bool canAttack(const std::pair<int, int> &pnt);

  /**
   * @brief Returns a string representation of the piece for debugging or display.
   * @return A string describing the piece.
   */
  std::string print();

  /**
   * @brief Indicates if the piece is playable (not an empty placeholder).
   * @return True if the piece can be used in the game, false otherwise.
   */
  bool isPlayable();
};

/**
 * @class ChessPieceEmpty
 * @brief Represents an empty square on the chessboard.
 */
class ChessPieceEmpty : public ChessPieceBase {
public:
  /**
   * @brief Constructs an empty piece at the given coordinates.
   * @param x The x-coordinate.
   * @param y The y-coordinate.
   * @param log Pointer to the logger instance.
   * @param board Pointer to the board array.
   */
  ChessPieceEmpty(int x, int y, Logger *log, ChessPieceBase ***board);

  /**
   * @brief Retrieves possible moves for an empty square (none).
   * @return An empty vector of coordinates.
   */
  virtual std::vector<std::pair<int, int>> getMoveCandidates() override;

  /**
   * @brief Retrieves possible attack positions for an empty square (none).
   * @param all If true, considers all possible positions (still none).
   * @return An empty vector of coordinates.
   */
  virtual std::vector<std::pair<int, int>>
  getAttackCandidates(bool all) override;

  /**
   * @brief Checks if an empty square can move to a position (it cannot).
   * @param pnt The target position.
   * @return Always false.
   */
  virtual bool canMoveTo(const std::pair<int, int> &pnt) override;

  /**
   * @brief Checks if an empty square can attack a position (it cannot).
   * @param pnt The target position.
   * @return Always false.
   */
  virtual bool canAttack(const std::pair<int, int> &pnt) override;

  /**
   * @brief Destructor.
   */
  ~ChessPieceEmpty();
};

/**
 * @class ChessPiecePawn
 * @brief Represents a pawn piece.
 */
class ChessPiecePawn : public ChessPieceBase {
public:
  /**
   * @brief Constructs a pawn at the given coordinates.
   * @param x The x-coordinate.
   * @param y The y-coordinate.
   * @param color True for white, false for black.
   * @param log Pointer to the logger instance.
   * @param board Pointer to the board array.
   * @param moved_ Indicates whether the pawn has moved before.
   */
  ChessPiecePawn(int x, int y, bool color, Logger *log,
                 ChessPieceBase ***board, bool moved_ = false);

  /**
   * @brief Retrieves possible moves for this pawn.
   * @return A vector of (x, y) coordinates representing valid moves.
   */
  virtual std::vector<std::pair<int, int>> getMoveCandidates() override;

  /**
   * @brief Retrieves possible attack positions for this pawn.
   * @param all If true, includes all possible attack squares.
   * @return A vector of (x, y) coordinates representing valid attacks.
   */
  virtual std::vector<std::pair<int, int>>
  getAttackCandidates(bool all) override;

  /**
   * @brief Destructor.
   */
  ~ChessPiecePawn();
};

/**
 * @class ChessPieceKnight
 * @brief Represents a knight piece.
 */
class ChessPieceKnight : public ChessPieceBase {
public:
  /**
   * @brief Constructs a knight at the given coordinates.
   * @param x The x-coordinate.
   * @param y The y-coordinate.
   * @param color True for white, false for black.
   * @param log Pointer to the logger instance.
   * @param board Pointer to the board array.
   * @param moved_ Indicates whether the knight has moved before.
   */
  ChessPieceKnight(int x, int y, bool color, Logger *log,
                   ChessPieceBase ***board, bool moved_ = false);

  /**
   * @brief Retrieves possible moves for the knight (in an L shape).
   * @return A vector of (x, y) coordinates representing valid moves.
   */
  virtual std::vector<std::pair<int, int>> getMoveCandidates() override;

  /**
   * @brief Retrieves possible attack positions for the knight.
   * @param all If true, includes all possible positions in an L shape.
   * @return A vector of (x, y) coordinates representing valid attacks.
   */
  virtual std::vector<std::pair<int, int>>
  getAttackCandidates(bool all) override;

  /**
   * @brief Destructor.
   */
  ~ChessPieceKnight();
};

/**
 * @class ChessPieceRook
 * @brief Represents a rook piece.
 */
class ChessPieceRook : public ChessPieceBase {
public:
  /**
   * @brief Constructs a rook at the given coordinates.
   * @param x The x-coordinate.
   * @param y The y-coordinate.
   * @param color True for white, false for black.
   * @param log Pointer to the logger instance.
   * @param board Pointer to the board array.
   * @param moved_ Indicates whether the rook has moved before.
   */
  ChessPieceRook(int x, int y, bool color, Logger *log,
                 ChessPieceBase ***board, bool moved_ = false);

  /**
   * @brief Retrieves possible moves for the rook (horizontal and vertical).
   * @return A vector of (x, y) coordinates representing valid moves.
   */
  virtual std::vector<std::pair<int, int>> getMoveCandidates() override;

  /**
   * @brief Retrieves possible attack positions for the rook.
   * @param all If true, includes all possible squares along ranks and files.
   * @return A vector of (x, y) coordinates representing valid attacks.
   */
  virtual std::vector<std::pair<int, int>>
  getAttackCandidates(bool all) override;

  /**
   * @brief Destructor.
   */
  ~ChessPieceRook();
};

/**
 * @class ChessPieceBishop
 * @brief Represents a bishop piece.
 */
class ChessPieceBishop : public ChessPieceBase {
public:
  /**
   * @brief Constructs a bishop at the given coordinates.
   * @param x The x-coordinate.
   * @param y The y-coordinate.
   * @param color True for white, false for black.
   * @param log Pointer to the logger instance.
   * @param board Pointer to the board array.
   * @param moved_ Indicates whether the bishop has moved before.
   */
  ChessPieceBishop(int x, int y, bool color, Logger *log,
                   ChessPieceBase ***board, bool moved_ = false);

  /**
   * @brief Retrieves possible moves for the bishop (diagonal directions).
   * @return A vector of (x, y) coordinates representing valid moves.
   */
  virtual std::vector<std::pair<int, int>> getMoveCandidates() override;

  /**
   * @brief Retrieves possible attack positions for the bishop.
   * @param all If true, includes all possible squares along diagonal lines.
   * @return A vector of (x, y) coordinates representing valid attacks.
   */
  virtual std::vector<std::pair<int, int>>
  getAttackCandidates(bool all) override;

  /**
   * @brief Destructor.
   */
  ~ChessPieceBishop();
};

/**
 * @class ChessPieceQueen
 * @brief Represents a queen piece.
 */
class ChessPieceQueen : public ChessPieceBase {
public:
  /**
   * @brief Constructs a queen at the given coordinates.
   * @param x The x-coordinate.
   * @param y The y-coordinate.
   * @param color True for white, false for black.
   * @param log Pointer to the logger instance.
   * @param board Pointer to the board array.
   * @param moved_ Indicates whether the queen has moved before.
   */
  ChessPieceQueen(int x, int y, bool color, Logger *log,
                  ChessPieceBase ***board, bool moved_ = false);

  /**
   * @brief Retrieves possible moves for the queen (combining rook and bishop moves).
   * @return A vector of (x, y) coordinates representing valid moves.
   */
  virtual std::vector<std::pair<int, int>> getMoveCandidates() override;

  /**
   * @brief Retrieves possible attack positions for the queen.
   * @param all If true, includes all squares along ranks, files, and diagonals.
   * @return A vector of (x, y) coordinates representing valid attacks.
   */
  virtual std::vector<std::pair<int, int>>
  getAttackCandidates(bool all) override;

  /**
   * @brief Destructor.
   */
  ~ChessPieceQueen();
};

/**
 * @class ChessPeiceKing
 * @brief Represents a king piece.
 */
class ChessPeiceKing : public ChessPieceBase {
public:
  /**
   * @brief Constructs a king at the given coordinates.
   * @param x The x-coordinate.
   * @param y The y-coordinate.
   * @param color True for white, false for black.
   * @param log Pointer to the logger instance.
   * @param board Pointer to the board array.
   * @param moved_ Indicates whether the king has moved before.
   */
  ChessPeiceKing(int x, int y, bool color, Logger *log, 
                 ChessPieceBase ***board, bool moved_ = false);

  /**
   * @brief Retrieves possible moves for the king (one square in any direction).
   * @return A vector of (x, y) coordinates representing valid moves.
   */
  virtual std::vector<std::pair<int, int>> getMoveCandidates() override;

  /**
   * @brief Retrieves possible attack positions for the king.
   * @param all If true, includes all squares adjacent to the king.
   * @return A vector of (x, y) coordinates representing valid attacks.
   */
  virtual std::vector<std::pair<int, int>>
  getAttackCandidates(bool all) override;

  /**
   * @brief Destructor.
   */
  ~ChessPeiceKing();
};
