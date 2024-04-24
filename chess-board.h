#pragma once
#include"chess-peice.h"
class  ChessBoard
{
private:
    Logger* log;
    ChessPieceBase*** board;
    ChessPieceBase** createPawnRow(bool white);
    ChessPieceBase** createEmptyRow(int row);
    ChessPieceBase** createFirstRow(bool white);
public:
    void printBoard();
    ChessBoard(Logger* log);
    ~ ChessBoard();
};
