#include"chess-peice.h"
#include<set>
#include"chess-board.h"
ChessPieceCode ChessPieceBase::getCode()
{
    return code;
}
//todo peredelat vezde attack candidates potomu 4to esli ya natikajus' na empty to ja dolzhen delat 4ek jesli je to vrag, i jesli net to sri sebe v rot.
// mojno o4en mnoho sokratit', sdelaj canmove i canattack metodami classa base i realizuj kak v queen ili bishop
static bool kingNear(ChessPieceBase*** board, std::pair<int,int> pos)
{
    int i,j;
    for(i=pos.first-1;i<pos.first+2;++i)
    {
        for(j=pos.second-1;j<pos.second+2;++j)
        {
            if(i>=0&&j>=0&&i<BOARDSIZE&&j<BOARDSIZE&&(i!=pos.first||j!=pos.second))
            {
                if(board[i][j]->getCode()==KING)
                {
                    return true;
                }
            }
        }
    }
    return false;
}
bool ChessPieceBase::isPlayable()
{
    return playable;
}
bool ChessPieceBase::isWhite()
{  
    return white;
}
char ChessPieceBase::getSymb(ChessPieceCode code)
{
    switch (code)
    {
    case EMPTY:
        return '0';
        break;
    case PAWN:
        return 'P';
        break;
    case ROOK:
        return 'R';
        break;
    case KNIGHT:
        return 'H';
        break;
    case BISHOP:
        return 'B';
        break;
    case QUEEN:
        return 'Q';
        break;
    case KING:
        return 'K';
        break;
    default:
        return '?';
        break;
    }
}
std::string ChessPieceBase::print()
{
    std::string out;
    out += white? "W" : "B";
    out += ChessPieceBase::getSymb(this->code);
    return out;
}
bool ChessPieceBase::canMoveTo(const std::pair<int,int>& pnt)
{
    
    std::vector<std::pair<int,int>> candidates = this->getMoveCandidates();
    for(const std::pair<int,int>& cnd : candidates)
    {
        if(cnd == pnt)
        {
            return true;
        }
    }
    return false;
}
bool ChessPieceBase::canAttack(const std::pair<int,int>& pnt)
{
    std::vector<std::pair<int,int>> candidates = this->getAttackCandidates(false);
    for(const std::pair<int,int>& cnd : candidates)
    {
        if(cnd == pnt)
        {
            return true;
        }
    }
    return false;
}
ChessPieceEmpty::ChessPieceEmpty(int x, int y,Logger* log,ChessPieceBase*** board)
{
    this->log=log;
    this->x=x;
    this->y=y;
    this->code=EMPTY;
    this->board=board;
    if(log)
    log->log("ПУСТОЙ КУСОК БЫЛ ВУПУКАН!!!!");
}
std::vector<std::pair<int,int>> ChessPieceEmpty::getMoveCandidates()
{
    return {};
}
std::vector<std::pair<int,int>> ChessPieceEmpty::getAttackCandidates(bool all)
{
    return {};
}
bool ChessPieceEmpty::canMoveTo(const std::pair<int,int>& pnt)
{
    return false;
}
bool ChessPieceEmpty::canAttack(const std::pair<int,int>& pnt)
{
    return false;
}
void ChessPieceBase::move(const std::pair<int,int>& dest)
{
    this->x=dest.second;
    this->y=dest.first;
}
ChessPieceEmpty::~ChessPieceEmpty()
{
    if(log)
    this->log->log("ПУСТОЙ ПУКАН БЫЛ ПРОПУКАН");
}




ChessPiecePawn::ChessPiecePawn(int x, int y,bool color,Logger* log,ChessPieceBase*** board)
{
    this->log=log;
    this->x=x;
    this->y=y;
    this->code=PAWN;
    this->board=board;
    this->white=color;
    this->playable=true;
    if(log)
    log->log("ГВИТНИК НАРОДИВСЯ!!!!!!!");
}
std::vector<std::pair<int,int>> ChessPiecePawn::getMoveCandidates()
{
    int y_ = y-2*white+1;
    if(y_>=0&&y_<BOARDSIZE&&board[y_][x]->getCode()==EMPTY)
    {
        return {{y_,x}};
    }
    return {};
}
std::vector<std::pair<int,int>> ChessPiecePawn::getAttackCandidates(bool all)
{
    std::vector<std::pair<int,int>> out;
    int i,y_;
    y_=y-2*white+1;
    for(i=-1;i<=1;i+=2)
    {
        if(y_>=0&&y_<BOARDSIZE&&x+i>=0&&x+i<BOARDSIZE)
        {
            if(board[y_][x+i]->getCode()==EMPTY&&all)
            {
                out.push_back({y_,x+i});
            }
            if(board[y_][x+i]->getCode()!=EMPTY&&board[y_][x+i]->isWhite()!=white)
                out.push_back({y_,x+i});
        }
    }
    return out;
}


ChessPieceKnight::ChessPieceKnight(int x, int y,bool color,Logger* log,ChessPieceBase*** board)
{
    this->log=log;
    this->x=x;
    this->y=y;
    this->code=KNIGHT;
    this->board=board;
    this->white=color;
    this->playable=true;
    if(log)
    log->log("КОНЬ ВЫСРАЛСЯ!!!");
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
            if(x>=0&&x<BOARDSIZE&&y>=0&&y<BOARDSIZE&&board[y][x]->getCode()==EMPTY)
            {
                out.push_back({y,x});
            }
            x=this->x+i;
            y=this->y+j;
            if(x>=0&&x<BOARDSIZE&&y>=0&&y<BOARDSIZE&&board[y][x]->getCode()==EMPTY)
            {
                out.push_back({y,x});
            }
        }
    }
    return out;
}
std::vector<std::pair<int,int>> ChessPieceKnight::getAttackCandidates(bool all)
{
    std::vector<std::pair<int,int>> out;
    int i,j,x,y;
    for(j=-2;j<=2;j+=4)
    {
        for(i=-1;i<=1;i+=2)
        {
            x=this->x+j;
            y=this->y+i;
            if(x>=0&&x<BOARDSIZE&&y>=0&&y<BOARDSIZE&&
            (board[y][x]->getCode()!=EMPTY&&board[y][x]->isWhite()!=this->white||(all&&board[y][x]->getCode()==EMPTY)))
            {
                out.push_back({y,x});
            }
            x=this->x+i;
            y=this->y+j;
            if(x>=0&&x<BOARDSIZE&&y>=0&&y<BOARDSIZE&&
            (board[y][x]->getCode()!=EMPTY&&board[y][x]->isWhite()!=this->white||(all&&board[y][x]->getCode()==EMPTY)))
            {
                out.push_back({y,x});
            }
        }
    }
    return out;    
}

ChessPieceRook::ChessPieceRook(int x, int y,bool color,Logger* log,ChessPieceBase*** board)
{
    this->log=log;
    this->x=x;
    this->y=y;
    this->code=ROOK;
    this->board=board;
    this->white=color;
    this->playable=true;
}
std::vector<std::pair<int,int>> ChessPieceRook::getMoveCandidates()
{
    int i,j,k;
    std::vector<std::pair<int,int>> out;
    for(j=-1;j<2;j+=2)
    {
        for(i=this->y;i>=0&&i<BOARDSIZE;i+=j)
        {
            if(board[i][this->x]->getCode()==EMPTY)
            {
                out.push_back({i,this->x});
            }
            else if (i!=this->y)
            {
                break;
            }
        }
        for(k=this->x;k>=0&&k<BOARDSIZE;k+=j)
        {
            if(board[this->y][k]->getCode()==EMPTY)
            {
                out.push_back({this->y,k});
            }
            else if (k!=this->x)
            {
                break;
            }
        }
    }
    return out; 
}
std::vector<std::pair<int,int>> ChessPieceRook::getAttackCandidates(bool all)
{
    int i,j,k;
    std::vector<std::pair<int,int>> out;
    for(j=-1;j<2;j+=2)
    {
        for(i=this->y;i>=0&&i<BOARDSIZE;i+=j)
        {
            if(board[i][this->x]->getCode()!=EMPTY)
            {
                if(board[i][this->x]->isWhite()!=white)
                {
                    out.push_back({i,this->x});
                    break;
                }
                else if (i!=this->y)
                {
                    break;
                }
            }
            else if(all)
            {
                out.push_back({i,this->x});
            }
        }
        for(k=this->x;k>=0&&k<BOARDSIZE;k+=j)
        {
            if(board[this->y][k]->getCode()!=EMPTY)
            {
                if(board[this->y][k]->isWhite()!=white)
                {
                    out.push_back({this->y,k});
                    break;
                }    
                else if (k!=this->x)
                {
                    break;
                }
            }
            else if(all)
            {
                out.push_back({this->y,k});
            }
        }
    }
    return out; 
}


ChessPieceBishop::ChessPieceBishop(int x, int y,bool color,Logger* log,ChessPieceBase*** board)
{
    this->log=log;
    this->x=x;
    this->y=y;
    this->code=BISHOP;
    this->board=board;
    this->white=color;
    this->playable=true;
}
std::vector<std::pair<int,int>> ChessPieceBishop::getMoveCandidates()
{
    std::vector<std::pair<int,int>> out;
    bool modX = false, modY = false;
    int i,j,counter;
    counter=0;
    i = this->y;
    j = this->x;
    while(counter<=MAXDEPTH)
    {
        i+=1-modY*2;
        j+=1-modX*2;
        if(i<BOARDSIZE&&i>=0&&j<BOARDSIZE&&j>=0&&this->board[i][j]->getCode()==EMPTY)
        {
            out.push_back({i,j});
        }
        else
        {
            if(!modX&&!modY)
            {
                modY=!modY;
                i=y;
                j=x;
            }
            else if(!modX&&modY)
            {
                modX=!modX;
                i=y;
                j=x;
            }
            else if(modX&&modY)
            {
                modY=!modY;
                i=y;
                j=x;                   
            }
            else
            {
                return out;
            }
        }
    }
    throw std::runtime_error("COULDNT CALCULATE MOVES");
    return out;
}
std::vector<std::pair<int,int>> ChessPieceBishop::getAttackCandidates(bool all)
{
    std::vector<std::pair<int,int>> out;
    bool modX = false, modY = false;
    int i,j,counter;
    counter=0;
    i = this->y;
    j = this->x;
    while(counter<=MAXDEPTH)
    {
        i+=1-modY*2;
        j+=1-modX*2;
        if(i<BOARDSIZE&&i>=0&&j<BOARDSIZE&&j>=0)
        {
            if(this->board[i][j]->getCode()!=EMPTY)
            {
                if(this->board[i][j]->isWhite()!=this->white)
                    out.push_back({i,j});
                if(!modX&&!modY)
                {
                    modY=!modY;
                    i=y;
                    j=x;
                }
                else if(!modX&&modY)
                {
                    modX=!modX;
                    i=y;
                    j=x;
                }
                else if(modX&&modY)
                {
                    modY=!modY;
                    i=y;
                    j=x;                   
                }
                else
                {
                    return out;
                }
            }
            else if(all)
            {
                out.push_back({i,j});
            }
        }
        else
        {
            if(!modX&&!modY)
            {
                modY=!modY;
                i=y;
                j=x;
            }
            else if(!modX&&modY)
            {
                modX=!modX;
                i=y;
                j=x;
            }
            else if(modX&&modY)
            {
                modY=!modY;
                i=y;
                j=x;                   
            }
            else
            {
                return out;
            }
        }
    }
    throw std::runtime_error("COULDNT CALCULATE MOVES");
    return out;
}


ChessPieceQueen::ChessPieceQueen(int x, int y,bool color,Logger* log,ChessPieceBase*** board)
{
    this->log=log;
    this->x=x;
    this->y=y;
    this->code=QUEEN;
    this->board=board;
    this->white=color;
    this->playable=true;
}
std::vector<std::pair<int,int>> ChessPieceQueen::getMoveCandidates()
{
    std::vector<std::pair<int,int>> out;
    bool modX = false, modY = false;
    int i,j,k,counter;
    for(j=-1;j<2;j+=2)
    {
        for(i=this->y;i>=0&&i<BOARDSIZE;i+=j)
        {
            if(board[i][this->x]->getCode()==EMPTY)
            {
                out.push_back({i,this->x});
            }
            else if (i!=this->y)
            {
                break;
            }
        }
        for(k=this->x;k>=0&&k<BOARDSIZE;k+=j)
        {
            if(board[this->y][k]->getCode()==EMPTY)
            {
                out.push_back({this->y,k});
            }
            else if (k!=this->x)
            {
                break;
            }
        }
    }
    counter=0;
    i = this->y;
    j = this->x;
    while(counter<=MAXDEPTH)
    {
        i+=1-modY*2;
        j+=1-modX*2;
        if(i<BOARDSIZE&&i>=0&&j<BOARDSIZE&&j>=0&&this->board[i][j]->getCode()==EMPTY)
        {
            out.push_back({i,j});
        }
        else
        {
            if(!modX&&!modY)
            {
                modY=!modY;
                i=y;
                j=x;
            }
            else if(!modX&&modY)
            {
                modX=!modX;
                i=y;
                j=x;
            }
            else if(modX&&modY)
            {
                modY=!modY;
                i=y;
                j=x;                   
            }
            else
            {
                return out;
            }
        }
    }
    throw std::runtime_error("COULDNT CALCULATE MOVES");
    return out;
}
std::vector<std::pair<int,int>> ChessPieceQueen::getAttackCandidates(bool all)
{
    std::vector<std::pair<int,int>> out;
    bool modX = false, modY = false;
    int i,j,k,counter;
    for(j=-1;j<2;j+=2)
    {
        for(i=this->y;i>=0&&i<BOARDSIZE;i+=j)
        {
            if(board[i][this->x]->getCode()!=EMPTY)
            {
                if(board[i][this->x]->isWhite()!=white)
                {
                    out.push_back({i,this->x});
                    break;
                }
                else if (i!=this->y)
                {
                    break;
                }
            }
            else if(all)
            {
                out.push_back({i,this->x});
            }
        }
        for(k=this->x;k>=0&&k<BOARDSIZE;k+=j)
        {
            if(board[this->y][k]->getCode()!=EMPTY)
            {
                if(board[this->y][k]->isWhite()!=white)
                {
                    out.push_back({this->y,k});
                    break;
                }    
                else if (k!=this->x)
                {
                    break;
                }
            }
            else if(all)
            {
                out.push_back({this->y,k});
            }
        }
    }
    counter=0;
    i = this->y;
    j = this->x;
    while(counter<=MAXDEPTH)
    {
        i+=1-modY*2;
        j+=1-modX*2;
        if(i<BOARDSIZE&&i>=0&&j<BOARDSIZE&&j>=0)
        {
            if(this->board[i][j]->getCode()!=EMPTY)
            {
                if(this->board[i][j]->isWhite()!=this->white)
                    out.push_back({i,j});
                if(!modX&&!modY)
                {
                    modY=!modY;
                    i=y;
                    j=x;
                }
                else if(!modX&&modY)
                {
                    modX=!modX;
                    i=y;
                    j=x;
                }
                else if(modX&&modY)
                {
                    modY=!modY;
                    i=y;
                    j=x;                   
                }
                else
                {
                    return out;
                }
            }
            else if(all)
            {
                out.push_back({i,j});
            }
        }
        else
        {
            if(!modX&&!modY)
            {
                modY=!modY;
                i=y;
                j=x;
            }
            else if(!modX&&modY)
            {
                modX=!modX;
                i=y;
                j=x;
            }
            else if(modX&&modY)
            {
                modY=!modY;
                i=y;
                j=x;                   
            }
            else
            {
                return out;
            }
        }
    }
    throw std::runtime_error("COULDNT CALCULATE MOVES");
    return out;
}

ChessPeiceKing::ChessPeiceKing(int x, int y,bool color,Logger* log,ChessPieceBase*** board)
{
    this->log=log;
    this->x=x;
    this->y=y;
    this->code=KING;
    this->board=board;
    this->white=color;
    this->playable=true;
}
std::vector<std::pair<int, int>> ChessPeiceKing::getMoveCandidates()
{
    std::vector<std::pair<int, int>> out;
    std::set<std::pair<int,int>> danger = ChessBoard::getDangerousPoints(board,white);
    int i,j;
    for(i=this->y-1;i<this->y+2;++i)
    {
        for(j=this->x-1;j<this->x+2;++j)
        {
            if(i>=0&&j>=0&&i<BOARDSIZE&&j<BOARDSIZE&&(i!=this->y||j!=this->x))
            {
                if(board[i][j]->getCode()==EMPTY&&danger.find({i,j})==danger.end()&&kingNear(this->board,{i,j}))
                {
                    out.push_back({i,j});
                }
            }
        }
    }
    return out;
}
std::vector<std::pair<int, int>> ChessPeiceKing::getAttackCandidates(bool all)
{
    std::vector<std::pair<int, int>> out;
    std::set<std::pair<int,int>> danger;
    if(!all)
    {
        danger = ChessBoard::getDangerousPoints(board,white);
    }
    else
    {
        danger={};
    }
    int i,j;
    for(i=this->y-1;i<this->y+2;++i)
    {
        for(j=this->x-1;j<this->x+2;++j)
        {
            if(i>=0&&j>=0&&i<BOARDSIZE&&j<BOARDSIZE&&(i!=y||j!=x))
            {
                if(board[i][j]->getCode()!=EMPTY)
                {
                    if(danger.find({i,j})==danger.end()&&board[i][j]->isWhite()!=white)
                        out.push_back({i,j});
                }
                else if(all)
                {
                    out.push_back({i,j});
                }
            }
        }
    }
    return out;
}

