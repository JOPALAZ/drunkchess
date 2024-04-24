#include"chess-peice.h"
ChessPieceCode ChessPieceBase::getCode()
{
    return code;
}
//todo peredelat vezde attack candidates potomu 4to esli ya natikajus' na empty to ja dolzhen delat 4ek jesli je to vrag, i jesli net to sri sebe v rot.
// mojno o4en mnoho sokratit', sdelaj canmove i canattack metodami classa base i realizuj kak v queen ili bishop


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

ChessPieceEmpty::ChessPieceEmpty(int x, int y,Logger* log,ChessPieceBase*** board)
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




ChessPiecePawn::ChessPiecePawn(int x, int y,bool color,Logger* log,ChessPieceBase*** board)
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
    if(y_>=0&&y_<BOARDSIZE&&board[y_][x]->getCode()==NONE)
    {
        return {{x,y_}};
    }
    return {};
}
std::vector<std::pair<int,int>> ChessPiecePawn::getAttackcCandidates()
{
    std::vector<std::pair<int,int>> out;
    int i,y_;
    y_=y+2*white-1;
    for(i=-1;i<=1;i+=2)
    {
        if(y_>=0&&y_<BOARDSIZE&&x+i>=0&&x+i<BOARDSIZE&&board[y+2*white-1][x+i]->getCode()!=EMPTY&&board[y+2*white-1][x+i]->isWhite()!=white)
        {
            out.push_back({y_,x+i});
        }
    }
    return out;
}
bool ChessPiecePawn::canMoveTo(int x, int y)
{
    if(x>=0&&x<BOARDSIZE&&y>=0&&y<BOARDSIZE&&board[y][x]->getCode()==EMPTY)
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
    if(x>=0&&x<BOARDSIZE&&y>=0&&y<BOARDSIZE&&board[y][x]->getCode()!=EMPTY&&board[y][x]->isWhite()!=this->white)
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


ChessPieceKnight::ChessPieceKnight(int x, int y,bool color,Logger* log,ChessPieceBase*** board)
{
    this->log=log;
    this->x=x;
    this->y=y;
    this->code=KNIGHT;
    this->board=board;
    this->white=color;
    this->playable=true;
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
            if(x>=0&&x<BOARDSIZE&&y>=0&&y<BOARDSIZE&&board[y][x]->getCode()!=EMPTY&&board[y][x]->isWhite()!=this->white)
            {
                out.push_back({y,x});
            }
            x=this->x+i;
            y=this->y+j;
            if(x>=0&&x<BOARDSIZE&&y>=0&&y<BOARDSIZE&&board[y][x]->getCode()!=EMPTY&&board[y][x]->isWhite()!=this->white)
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
            if(x_>=0&&x_<BOARDSIZE&&y_>=0&&y_<BOARDSIZE&&x_==x&&y_==y&&board[y_][x_]->getCode()==EMPTY)
            {
                return true;
            }
            x=this->x+i;
            y=this->y+j;
            if(x_>=0&&x_<BOARDSIZE&&y_>=0&&y_<BOARDSIZE&&x_==x&&y_==y&&board[y_][x_]->getCode()==EMPTY)
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
            if(x_>=0&&x_<BOARDSIZE&&y_>=0&&y_<BOARDSIZE&&x_==x&&y_==y&&board[y_][x_]->getCode()!=EMPTY&&board[y_][x_]->isWhite()!=this->white)
            {
                return true;
            }
            x=this->x+i;
            y=this->y+j;
            if(x_>=0&&x_<BOARDSIZE&&y_>=0&&y_<BOARDSIZE&&x_==x&&y_==y&&board[y_][x_]->getCode()!=EMPTY&&board[y_][x_]->isWhite()!=this->white)
            {
                return true;
            }
        }
    }
    return false;    
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
std::vector<std::pair<int,int>> ChessPieceRook::getAttackcCandidates()
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
        }
    }
    return out; 
}
bool ChessPieceRook::canMoveTo(int x, int y)
{
    int i,j,k;
    bool modeX=0;
    if(x!=this->x&&y!=this->y)
        return false;
    if(x!=this->x)
    {
        modeX=1;
    }
    for(j=-1;j<2;j+=2)
    {
        if(modeX)
        {
            for(i=this->y;i>=0&&i<BOARDSIZE;i+=j)
            {
                if(board[i][this->x]->getCode()!=EMPTY&&i!=this->y)
                {
                    break;
                }
                if(i==y)
                {
                    return true;
                }
            }
        }
        else
        {
            for(k=this->x;k>=0&&k<BOARDSIZE;k+=j)
            {
                if(board[this->y][k]->getCode()!=EMPTY)
                {
                    if(board[this->y][k]->isWhite()!=white&&k!=this->y)
                    {
                        break;
                    }    
                    if (k==x)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false; 
}
bool ChessPieceRook::canAttack(int x, int y)
{
    int i,j,k;
    bool modeX=0;
    if(x!=this->x&&y!=this->y)
        return false;
    if(x!=this->x)
    {
        modeX=1;
    }
    for(j=-1;j<2;j+=2)
    {
        if(!modeX)
        {
            for(i=this->y;i>=0&&i<BOARDSIZE;i+=j)
            {
                if(board[i][this->x]->getCode()!=EMPTY&&i!=this->y)
                {
                    if(i==y&&board[i][this->x]->isWhite()!=white)
                    {
                        return true;
                    }
                    break;
                }
            }
        }
        else
        {
            for(k=this->x;k>=0&&k<BOARDSIZE;k+=j)
            {
                if(board[this->y][k]->getCode()!=EMPTY&&k!=this->x)
                {
                    if(k==x&&board[this->y][k]->isWhite()!=white)
                    {
                        return true;
                    }
                    break;
                }
            }
        }
    }
    return false; 
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
        if(i<BOARDSIZE&&i>=0&&j<BOARDSIZE&&j>=0&&this->board[i][j]->getCode()!=EMPTY)
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
std::vector<std::pair<int,int>> ChessPieceBishop::getAttackcCandidates()
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
        if(i<BOARDSIZE&&i>=0&&j<BOARDSIZE&&j>=0&&this->board[i][j]->getCode()!=EMPTY)
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
    }
    throw std::runtime_error("COULDNT CALCULATE MOVES");
    return out;
}
bool ChessPieceBishop::canMoveTo(int x, int y)
{
    std::pair<int,int> pnt = {y,x};
    std::vector<std::pair<int,int>> candidates = this->getMoveCandidates();
    for(std::pair<int,int> cnd : candidates)
    {
        if(cnd == pnt)
        {
            return true;
        }
    }
    return false;
}
bool ChessPieceBishop::canAttack(int x, int y)
{
    std::pair<int,int> pnt = {y,x};
    std::vector<std::pair<int,int>> candidates = this->getAttackcCandidates();
    for(std::pair<int,int> cnd : candidates)
    {
        if(cnd == pnt)
        {
            return true;
        }
    }
    return false;
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
    void* buf = (void*)(this);
    std::vector<std::pair<int,int>> out = static_cast<ChessPieceBishop*>(buf)->getMoveCandidates();
    std::vector<std::pair<int,int>> out1 = static_cast<ChessPieceRook*>(buf)->getMoveCandidates();
    for(std::pair<int,int> pair : out1)
    {
        if(std::find(out.begin(),out.end(),pair)==out.end())
        {
            out.push_back(pair);
        }
    }
    return out;
}
std::vector<std::pair<int,int>> ChessPieceQueen::getAttackcCandidates()
{
    void* buf = (void*)(this);
    std::vector<std::pair<int,int>> out = static_cast<ChessPieceBishop*>(buf)->getAttackcCandidates();
    std::vector<std::pair<int,int>> out1 = static_cast<ChessPieceRook*>(buf)->getAttackcCandidates();
    for(std::pair<int,int> pair : out1)
    {
        if(std::find(out.begin(),out.end(),pair)==out.end())
        {
            out.push_back(pair);
        }
    }
    return out;
}
bool ChessPieceQueen::canMoveTo(int x, int y)
{
    std::pair<int,int> pnt = {y,x};
    std::vector<std::pair<int,int>> candidates = this->getMoveCandidates();
    for(std::pair<int,int> cnd : candidates)
    {
        if(cnd == pnt)
        {
            return true;
        }
    }
    return false;
}
bool ChessPieceQueen::canAttack(int x, int y)
{
    std::pair<int,int> pnt = {y,x};
    std::vector<std::pair<int,int>> candidates = this->getAttackcCandidates();
    for(std::pair<int,int> cnd : candidates)
    {
        if(cnd == pnt)
        {
            return true;
        }
    }
    return false;
}