#pragma once
#include <stdexcept>

enum ChessPieceCode
{
    KING,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT,
    PAWN,
    EMPTY,
    NONE,
};

extern int prices[8];
extern int Mate;
extern int Pate;
extern int FirstMove;
extern int Castling;
extern float ATTACK_COST;
extern float worth;

int getScore(ChessPieceCode code);
