#pragma once
#include "chess-board.h"
#include "logger.h"
#include <algorithm>
#include <fstream>
#include <vector>
class IOhandler {
private:
  Logger *log;
  ChessBoard *ch;
  std::ostream *output;
  std::istream *input;
  Special_Parameter checkMate;
  bool gameIsOn = false;
  bool loop = true;
  bool server = false;
  bool side;
  bool startGame();
  bool startPreDefinedGame();
  void move(const std::string &move);
  std::vector<std::string> getPossibleOptions();
  void processInput(const std::string &response);
  std::vector<std::pair<int, int>> getMoveCandidates(std::pair<int, int> start);
  void printMoveCandidates(std::string start);
  void setParams();
  void printBoard();
  void dumpCurrentGamestate();
  std::pair<int, int> transcodePosition(std::string str);

public:
  static void toLowercase(std::string &str);
  ChessPieceCode askReplacement(bool side);
  void mainLoop();
  IOhandler(std::ostream *output, std::istream *input);
  IOhandler();

  ~IOhandler();
};
