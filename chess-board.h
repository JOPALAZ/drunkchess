#pragma once
#include"chess-peice.h"
#include <set>
#include <map>
class  ChessBoard
{
private:

    ChessPieceBase** createPawnRow(bool white);
    ChessPieceBase** createEmptyRow(int row);
    ChessPieceBase** createFirstRow(bool white);
protected:
    unsigned long long score[2] = {0,0};
    Logger* log;
    ChessPieceBase*** board;
public:
    static ChessPieceBase* createPeice(int x, int y,bool color, ChessPieceCode code, Logger* log, ChessPieceBase*** board);
    static void revertBoard(ChessPieceBase*** imgainaryBoard,ChessPieceBase*** board);
    static ChessPieceBase*** deleteBoard(ChessPieceBase*** board);
    static ChessPieceBase*** copyBoard(ChessPieceBase*** board, bool notImaginary = false);
    void printBoard();
    const std::pair<std::pair<int,int>,std::pair<int,int>> computeMove(bool white);
    bool performMove(const std::pair<std::pair<int,int>,std::pair<int,int>>& move);
    ChessPieceBase*** getBoard() {return board;}
    void debugPrintDanger();
    ChessBoard(Logger* log);
    static std::set<std::pair<int,int>> getDangerousPoints(ChessPieceBase*** board, bool white);
    void clear();
    void cycleFigure(std::pair<int,int> pos, bool color,ChessPieceCode code);
    ~ ChessBoard();
};
