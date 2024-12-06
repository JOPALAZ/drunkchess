#pragma once
#include "chess-peice.h"
#include <future>
#include <map>
#include <set>
#include <sstream>
#include <thread>

class IOhandler;

struct Move {
  std::pair<int, int> start;
  std::pair<int, int> end;
};

struct Move_Candidate {
  Move move;
  float dScore;
};

struct Figure_Move_Restriction {
  std::pair<int, int> position;
  std::vector<std::pair<int, int>> unrestrictedPositions;
};

struct Special_Parameter {
  bool kingAttacked;
  std::vector<std::pair<int, int>> saveKingPath;
  std::vector<Figure_Move_Restriction> restrictions;
};

struct Thread_Parameter {
  ChessPieceBase ***board;
  bool white;
  int difficulty;
  int depth;
  int maxDepth;
  bool ready;
  float score;
};

class ChessBoard {
private:
  ChessPieceBase **createPawnRow(bool white);
  ChessPieceBase **createEmptyRow(int row);
  ChessPieceBase **createFirstRow(bool white);

protected:
  Logger *log;
  ChessPieceBase ***board;
  int difficulty;
  int maxDepth;

  static const float recursiveSubroutine(ChessPieceBase ***board, bool white,
                                         int difficulty, int depth,
                                         int maxDepth, float worth);
  static void threadFunc(Thread_Parameter *param);
  static ChessPieceCode askReplacement(bool side, IOhandler* handler);

  // New helper function declarations
  static float performAttack(const Move &move, ChessPieceBase ***board,
                             IOhandler* handler);
  static float performNormalMove(const Move &move, ChessPieceBase ***board,
                                 IOhandler* handler);
  static float performCastling(const Move &move, ChessPieceBase ***board,
                               IOhandler* handler);

public:
  static int
  findFigureIndex(const std::vector<Figure_Move_Restriction> &restrictions,
                  std::pair<int, int> pos);
  static bool isDangerous(int distance, std::pair<int, int> kingPos, int8_t dX,
                          int8_t dY, ChessPieceBase *suspect);
  static ChessPieceBase *createPeice(int x, int y, bool color,
                                     ChessPieceCode code, Logger *log,
                                     ChessPieceBase ***board,
                                     bool moved_ = false);
  static void revertBoard(ChessPieceBase ***imgainaryBoard,
                          ChessPieceBase ***board);
  static ChessPieceBase ***deleteBoard(ChessPieceBase ***board);
  static ChessPieceBase ***copyBoard(ChessPieceBase ***board,
                                     bool notImaginary = false);
  static float performMove(const Move &move, ChessPieceBase ***board,
                           IOhandler* handler, bool overrideRightess = false);
  static void printImaginaryBoard(ChessPieceBase ***board,
                                  std::ostream *out = &std::cout);
  static std::vector<std::pair<int, int>>
  filterMoves(const std::vector<std::pair<int, int>> &input,
              Special_Parameter checkMate, int usedIndex);
  static Special_Parameter evaluateCheckMate(bool side,
                                             ChessPieceBase ***board);
  static bool simplifiedEvaluateCheckMate(bool side,
                                          std::pair<int, int> kingPosition,
                                          ChessPieceBase ***board);
  static std::pair<int, int> findKing(bool side, ChessPieceBase ***board);
  static ChessPieceBase *createPeiceFromString(int x, int y, bool color,
                                               char code, Logger *log,
                                               ChessPieceBase ***board,
                                               bool moved_ = false);
  static std::vector<std::pair<int, int>>
  getOverlap(const std::vector<std::pair<int, int>> &el1,
             const std::vector<std::pair<int, int>> &el2);
  int getDifficulty() { return difficulty; }
  void makeBoardFromString(const std::string &str);
  void printBoard(bool white, std::ostream *out = &std::cout,
                  bool server = false);
  Move getBestMove(bool white);
  ChessPieceBase ***getBoard() { return board; }
  void debugPrintDanger();
  ChessBoard(Logger *log, int difficulty);
  static std::set<std::pair<int, int>>
  getDangerousPoints(ChessPieceBase ***board, bool white);
  void clear();
  void cycleFigure(std::pair<int, int> pos, bool color, ChessPieceCode code);
  ~ChessBoard();
};
