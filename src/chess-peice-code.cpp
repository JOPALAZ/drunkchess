#include "chess-peice-codes.h"

int Mate = -999999;
int Pate = 0;
int FirstMove = 1;
int Castling = 50;
int prices[8] = {1100, 900, 500, 330, 320, 100, 0, -1};
float ATTACK_COST = 0.05;
float worth = 0.9;

int getScore(ChessPieceCode code) {
  switch (code) {
  case KING:
    return prices[0];
  case QUEEN:
    return prices[1];
  case ROOK:
    return prices[2];
  case BISHOP:
    return prices[3];
  case KNIGHT:
    return prices[4];
  case PAWN:
    return prices[5];
  case EMPTY:
    return prices[6];
  case NONE:
    return prices[7];
  default:
    throw std::invalid_argument("UNKNOWN FIGURE");
    return -1;
  }
}
