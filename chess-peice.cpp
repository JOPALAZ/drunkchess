#include"chess-peice.h"
ChessPieceCode ChessPieceBase::getCode()
{
    return code;
}


ChessPieceEmpty::ChessPieceEmpty(int x, int y,Logger* log,ChessPieceBase** board)
{
    this->log=log;
    this->x=x;
    this->y=y;
    this->code=EMPTY;
    this->board=board;
    log->log("ПУСТОЙ КУСОК БЫЛ ВУПУКАН!!!!");
}
std::vector<std::pair<int,int>> ChessPieceEmpty::getMoveCandidates()
{
    return {};
}
std::vector<std::pair<int,int>> ChessPieceEmpty::getAttackcCandidates()
{
    return {};
}
bool ChessPieceEmpty::canMoveTo(int x, int y)
{
    return false;
}
bool ChessPieceEmpty::canAttack(int x, int y)
{
    return false;
}
ChessPieceEmpty::~ChessPieceEmpty()
{
    this->log->log("ПУСТОЙ ПУКАН БЫЛ ПРОПУКАН");
}




ChessPiecePawn::ChessPiecePawn(int x, int y,bool color,Logger* log,ChessPieceBase** board)
{
    this->log=log;
    this->x=x;
    this->y=y;
    this->code=PAWN;
    this->board=board;
    this->white=color;
    this->playable=true;
    log->log("ГВИТНИК НАРОДИВСЯ!!!!!!!");
}
std::vector<std::pair<int,int>> ChessPiecePawn::getMoveCandidates()
{
    int y_ = y+2*white-1;
    if(y_>=0&&y_<BOARDSIZE&&board[y_][x].getCode()==NONE)
    {
        return {{x,y_}};
    }
    return {};
}
std::vector<std::pair<int,int>> ChessPiecePawn::getAttackcCandidates()
{
    std::vector<std::pair<int,int>> out;
    int i;
    for(i=-1;i<=1;i+=2)
    {
        if(board[y+2*white-1][x+1].getCode() == EMPTY)
        {
            out.push_back({y+2*white-1,x+i});
        }
    }
    return out;
}
bool ChessPiecePawn::canMoveTo(int x, int y)
{
    if(x>=0&&x<8&&y>=0&&y<8&&board[y][x].getCode()!=EMPTY)
        return false;
    if(white)
    {
        return this->y==y+1;
    }
    else
    {
        return this->y==y-1;
    }
}
bool ChessPiecePawn::canAttack(int x, int y)
{
    if(x>=0&&x<8&&y>=0&&y<8&&board[y][x].getCode()==EMPTY)
        return false;
    if(white)
    {
        return this->y==y+1&&(this->x==x-1||this->x==x+1);
    }
    else
    {
        return this->y==y-1&&(this->x==x-1||this->x==x+1);
    }
}
ChessPiecePawn::~ChessPiecePawn()
{
    this->log->log("АГЄНТ ГВИНТИК СДОХ");
}   


ChessPieceKnight::ChessPieceKnight(int x, int y,bool color,Logger* log,ChessPieceBase** board)
{
    this->log=log;
    this->x=x;
    this->y=y;
    this->code=KNIGHT;
    this->board=board;
    this->white=color;
    this->playable=true;
}
std::vector<std::pair<int,int>> ChessPieceKnight::getMoveCandidates()
{
    std::vector<std::pair<int,int>> out;
    int i,j,x,y;
    for(j=-2;j<=2;j+=4)
    {
        for(i=-1;i<=1;i+=2)
        {
            x=this->x+j;
            y=this->y+i;
            if(x>=0&&x<8&&y>=0&&y<8&&board[y][x].getCode()==EMPTY)
            {
                out.push_back({y,x});
            }
            x=this->x+i;
            y=this->y+j;
            if(x>=0&&x<8&&y>=0&&y<8&&board[y][x].getCode()==EMPTY)
            {
                out.push_back({y,x});
            }
        }
    }
    return out;
}
std::vector<std::pair<int,int>> ChessPieceKnight::getAttackcCandidates()
{
    std::vector<std::pair<int,int>> out;
    int i,j,x,y;
    for(j=-2;j<=2;j+=4)
    {
        for(i=-1;i<=1;i+=2)
        {
            x=this->x+j;
            y=this->y+i;
            if(x>=0&&x<8&&y>=0&&y<8&&board[y][x].getCode()!=EMPTY)
            {
                out.push_back({y,x});
            }
            x=this->x+i;
            y=this->y+j;
            if(x>=0&&x<8&&y>=0&&y<8&&board[y][x].getCode()!=EMPTY)
            {
                out.push_back({y,x});
            }
        }
    }
    return out;    
}
bool ChessPieceKnight::canMoveTo(int x, int y)
{
    int i,j,x_,y_;
    for(j=-2;j<=2;j+=4)
    {
        for(i=-1;i<=1;i+=2)
        {
            x_=this->x+j;
            y_=this->y+i;
            if(x_>=0&&x_<8&&y_>=0&&y_<8&&x_==x&&y_==y&&board[y_][x_].getCode()==EMPTY)
            {
                return true;
            }
            x=this->x+i;
            y=this->y+j;
            if(x_>=0&&x_<8&&y_>=0&&y_<8&&x_==x&&y_==y&&board[y_][x_].getCode()==EMPTY)
            {
                return true;
            }
        }
    }
    return false;    
}
bool ChessPieceKnight::canAttack(int x, int y)
{
    int i,j,x_,y_;
    for(j=-2;j<=2;j+=4)
    {
        for(i=-1;i<=1;i+=2)
        {
            x_=this->x+j;
            y_=this->y+i;
            if(x_>=0&&x_<8&&y_>=0&&y_<8&&x_==x&&y_==y&&board[y_][x_].getCode()!=EMPTY)
            {
                return true;
            }
            x=this->x+i;
            y=this->y+j;
            if(x_>=0&&x_<8&&y_>=0&&y_<8&&x_==x&&y_==y&&board[y_][x_].getCode()!=EMPTY)
            {
                return true;
            }
        }
    }
    return false;    
}