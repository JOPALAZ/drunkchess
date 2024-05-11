#pragma once
#include"chess-peice.h"
#include <set>
class  ChessBoard
{
private:
    Logger* log;
    ChessPieceBase*** board;
    ChessPieceBase** createPawnRow(bool white);
    ChessPieceBase** createEmptyRow(int row);
    ChessPieceBase** createFirstRow(bool white);
    ChessPieceBase* createPeice(int x, int y,bool color, ChessPieceCode code);
public:
    void printBoard();
    ChessPieceBase*** getBoard() {return board;}
    void debugPrintDanger();
    ChessBoard(Logger* log);
    static std::set<std::pair<int,int>> getDangerousPoints(ChessPieceBase*** board, bool white);
    void clear();
    void cycleFigure(std::pair<int,int> pos, bool color,ChessPieceCode code)
    {
        ChessPieceCode code_;
        if(pos.first>=0&&pos.second>=0&&pos.first<BOARDSIZE&&pos.second<BOARDSIZE)
        {
            if(board[pos.first][pos.second]==nullptr)
            {
                board[pos.first][pos.second] = createPeice(pos.second,pos.first,color,code);
                return;
            }
            code_=board[pos.first][pos.second]->getCode();
            delete board[pos.first][pos.second];
            if(code_==EMPTY)
            {
                board[pos.first][pos.second] = createPeice(pos.second,pos.first,color,code);
                return;
            }
                board[pos.first][pos.second] = createPeice(pos.second,pos.first,color,EMPTY);
                return;
        }
    }
    ~ ChessBoard();
};
