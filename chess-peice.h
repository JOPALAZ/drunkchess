#pragma once
#include"chess-peice-codes.h"
#include"logger.h"
#include<memory>
#include<set>
#include<vector>
#define BOARDSIZE 8
class ChessPieceBase
{
protected:
    ChessPieceCode code = NONE;
    bool white=true;
    bool playable=false;
    Logger* log=nullptr;
    ChessPieceBase** board=nullptr;
    int x=-1;
    int y=-1;
public:
    ChessPieceCode getCode();
    ChessPieceBase() = default;
    virtual std::vector<std::pair<int,int>> getMoveCandidates()=0;
    virtual std::vector<std::pair<int,int>> getAttackcCandidates()=0;
    virtual bool canMoveTo(int x,int y) = 0;
    virtual bool canAttack(int x,int y) = 0;
};
class ChessPieceEmpty : public ChessPieceBase
{
public:
    ChessPieceEmpty(int x, int y,Logger* log,ChessPieceBase** board);
    virtual std::vector<std::pair<int,int>> getMoveCandidates() override;
    virtual std::vector<std::pair<int,int>> getAttackcCandidates() override;
    virtual bool canMoveTo(int x,int y) override;
    virtual bool canAttack(int x,int y) override;
    ~ChessPieceEmpty();
};
class ChessPiecePawn : public ChessPieceBase
{
public:
    ChessPiecePawn(int x, int y,bool color,Logger* log,ChessPieceBase** board);
    virtual std::vector<std::pair<int,int>> getMoveCandidates() override;
    virtual std::vector<std::pair<int,int>> getAttackcCandidates() override;
    virtual bool canMoveTo(int x,int y) override;
    virtual bool canAttack(int x,int y) override;
    ~ChessPiecePawn();
};
class ChessPieceKnight : public ChessPieceBase
{
public:
    ChessPieceKnight(int x, int y,bool color,Logger* log,ChessPieceBase** board);
    virtual std::vector<std::pair<int,int>> getMoveCandidates() override;
    virtual std::vector<std::pair<int,int>> getAttackcCandidates() override;
    virtual bool canMoveTo(int x,int y) override;
    virtual bool canAttack(int x,int y) override;
    ~ChessPieceKnight();
};
class ChessPieceRook : public ChessPieceBase
{
public:
    ChessPieceRook(int x, int y,bool color,Logger* log,ChessPieceBase** board);
    virtual std::vector<std::pair<int,int>> getMoveCandidates() override;
    virtual std::vector<std::pair<int,int>> getAttackcCandidates() override;
    virtual bool canMoveTo(int x,int y) override;
    virtual bool canAttack(int x,int y) override;
    ~ChessPieceRook();
};
