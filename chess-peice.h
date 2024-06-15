#pragma once
#include"chess-peice-codes.h"
#include"logger.h"
#include<memory>
#include<set>
#include<algorithm>
#include<vector>
#define BOARDSIZE 8
#define MAXDEPTH 256
class ChessPieceBase
{
protected:
    ChessPieceCode code = NONE;
    bool white=true;
    bool playable=false;
    Logger* log=nullptr;
    ChessPieceBase*** board=nullptr;
    int x=-1;
    int y=-1;
    static char getSymb(ChessPieceCode code);


public:
    void move(const std::pair<int,int>& dest);
    int getX() {return x;}
    int getY() {return y;}
    Logger* getLogger() {return log;}
    ChessPieceCode getCode();
    bool isWhite();
    ChessPieceBase() = default;
    virtual std::vector<std::pair<int,int>> getMoveCandidates()=0;
    virtual std::vector<std::pair<int,int>> getAttackCandidates(bool all)=0;
    virtual bool canMoveTo(const std::pair<int,int>& pnt);
    virtual bool canAttack(const std::pair<int,int>& pnt);
    std::string print();
    bool isPlayable();
};
class ChessPieceEmpty : public ChessPieceBase
{
public:
    ChessPieceEmpty(int x, int y,Logger* log,ChessPieceBase*** board);
    virtual std::vector<std::pair<int,int>> getMoveCandidates() override;
    virtual std::vector<std::pair<int,int>> getAttackCandidates(bool all) override;
    virtual bool canMoveTo(const std::pair<int,int>& pnt) override;
    virtual bool canAttack(const std::pair<int,int>& pnt) override;
    ~ChessPieceEmpty();
};
class ChessPiecePawn : public ChessPieceBase
{
public:
    ChessPiecePawn(int x, int y,bool color,Logger* log,ChessPieceBase*** board);
    virtual std::vector<std::pair<int,int>> getMoveCandidates() override;
    virtual std::vector<std::pair<int,int>> getAttackCandidates(bool all) override;

    ~ChessPiecePawn();
};
class ChessPieceKnight : public ChessPieceBase
{
public:
    ChessPieceKnight(int x, int y,bool color,Logger* log,ChessPieceBase*** board);
    virtual std::vector<std::pair<int,int>> getMoveCandidates() override;
    virtual std::vector<std::pair<int,int>> getAttackCandidates(bool all) override;

    ~ChessPieceKnight();
};
class ChessPieceRook : public ChessPieceBase
{
public:
    ChessPieceRook(int x, int y,bool color,Logger* log,ChessPieceBase*** board);
    virtual std::vector<std::pair<int,int>> getMoveCandidates() override;
    virtual std::vector<std::pair<int,int>> getAttackCandidates(bool all) override;

    ~ChessPieceRook();
};
class ChessPieceBishop : public ChessPieceBase
{
public:
    ChessPieceBishop(int x, int y,bool color,Logger* log,ChessPieceBase*** board);
    virtual std::vector<std::pair<int,int>> getMoveCandidates() override;
    virtual std::vector<std::pair<int,int>> getAttackCandidates(bool all) override;

    ~ChessPieceBishop();
};
class ChessPieceQueen : public ChessPieceBase
{
public:
    ChessPieceQueen(int x, int y,bool color,Logger* log,ChessPieceBase*** board);
    virtual std::vector<std::pair<int,int>> getMoveCandidates() override;
    virtual std::vector<std::pair<int,int>> getAttackCandidates(bool all) override;

    ~ChessPieceQueen();
};
class ChessPeiceKing : public ChessPieceBase
{
public:
    ChessPeiceKing(int x, int y,bool color,Logger* log,ChessPieceBase*** board);
    virtual std::vector<std::pair<int,int>> getMoveCandidates() override;
    virtual std::vector<std::pair<int,int>> getAttackCandidates(bool all) override;
    ~ChessPeiceKing();
};
