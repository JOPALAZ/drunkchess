#pragma once
#include"chess-peice.h"
class  ChessBoard
{
private:
    Logger* log;
    ChessPieceBase** board;
public:
     ChessBoard(Logger* log);
    ~ ChessBoard();
};
