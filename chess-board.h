#pragma once
#include"chess-peice.h"
#include <set>
#include <map>


struct Move
{
    std::pair<int,int> start;
    std::pair<int,int> end;
};

struct Move_Candidate
{
    Move move;
    int dScore;
};
struct Figure_Move_Restriction
{
    std::pair<int,int> position;
    std::vector<std::pair<int,int>> unrestrictedPositions;
};
struct Special_Parameter
{
    bool kingAttacked;
    std::vector<std::pair<int,int>> saveKingPath; 
    std::vector<Figure_Move_Restriction> restrictions;
};


class  ChessBoard
{
private:

    ChessPieceBase** createPawnRow(bool white);
    ChessPieceBase** createEmptyRow(int row);
    ChessPieceBase** createFirstRow(bool white);
protected:

    Logger* log;
    ChessPieceBase*** board;
    int difficulty;
    static const int recursiveSubroutine(ChessPieceBase*** board, bool white, int difficulty,int depth);
    static ChessPieceCode askReplacement();
    static int findFigureIndex(const std::vector<Figure_Move_Restriction> restrictions,std::pair<int,int> pos);
public:
    static bool isDangerous(int distance,std::pair<int,int> kingPos,int8_t dX,int8_t dY, ChessPieceBase* suspect);
    static ChessPieceBase* createPeice(int x, int y,bool color, ChessPieceCode code, Logger* log, ChessPieceBase*** board);
    static void revertBoard(ChessPieceBase*** imgainaryBoard,ChessPieceBase*** board);
    static ChessPieceBase*** deleteBoard(ChessPieceBase*** board);
    static ChessPieceBase*** copyBoard(ChessPieceBase*** board, bool notImaginary = false);
    static float performMove(const Move& move,ChessPieceBase*** board,bool overrideRightess=false);
    static void printImaginaryBoard(ChessPieceBase*** board,std::ostream* out = &std::cout);
    static std::vector<std::pair<int,int>> filterMoves(const std::vector<std::pair<int,int>>& input,Special_Parameter checkMate,int usedIndex);
    static Special_Parameter evaluateCheckMate(bool side,ChessPieceBase*** board);
    static std::pair<int,int> findKing(bool side, ChessPieceBase*** board);
    void printBoard(std::ostream* out = &std::cout);
    Move getBestMove(bool white);
    ChessPieceBase*** getBoard() {return board;}
    void debugPrintDanger();
    ChessBoard(Logger* log,int diffficulty);
    static std::set<std::pair<int,int>> getDangerousPoints(ChessPieceBase*** board, bool white);
    void clear();
    void cycleFigure(std::pair<int,int> pos, bool color,ChessPieceCode code);
    ~ ChessBoard();
};
