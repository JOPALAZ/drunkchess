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
    static const std::vector<Move_Candidate> recursiveSubroutine(ChessPieceBase*** board, bool white, int difficulty);
public:
    static ChessPieceBase* createPeice(int x, int y,bool color, ChessPieceCode code, Logger* log, ChessPieceBase*** board);
    static void revertBoard(ChessPieceBase*** imgainaryBoard,ChessPieceBase*** board);
    static ChessPieceBase*** deleteBoard(ChessPieceBase*** board);
    static ChessPieceBase*** copyBoard(ChessPieceBase*** board, bool notImaginary = false);
    static int performMove(const Move& move,ChessPieceBase*** board);
    void printBoard();
    
    ChessPieceBase*** getBoard() {return board;}
    void debugPrintDanger();
    ChessBoard(Logger* log,int difficulty);
    static std::set<std::pair<int,int>> getDangerousPoints(ChessPieceBase*** board, bool white);
    void clear();
    void cycleFigure(std::pair<int,int> pos, bool color,ChessPieceCode code);
    ~ ChessBoard();
};
