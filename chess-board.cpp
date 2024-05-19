#include"chess-board.h"

ChessPieceBase* ChessBoard::createPeice(int x, int y,bool color, ChessPieceCode code)
{
    switch (code)
    {
    case KING:
        return new ChessPeiceKing(x,y,color,log,board);
        break;
    case QUEEN:
        return new ChessPieceQueen(x,y,color,log,board);
        break;
    case BISHOP:
        return new ChessPieceBishop(x,y,color,log,board);
        break;
    case KNIGHT:
        return new ChessPieceKnight(x,y,color,log,board);
        break;
    case ROOK:
        return new ChessPieceRook(x,y,color,log,board);
        break;
    case PAWN:
        return new ChessPiecePawn(x,y,color,log,board);
        break;
    case EMPTY:
        return new ChessPieceEmpty(x,y,log,board);
        break;
    default:
        throw std::runtime_error("UNKNOWN OPTION");
        break;
    }
}
std::set<std::pair<int,int>> ChessBoard::getDangerousPoints(ChessPieceBase*** board, bool white)
{
    std::set<std::pair<int,int>> out;
    std::vector<std::pair<int,int>> buf;
    int i,j;
    //std::cout<<std::endl<<std::endl<<std::endl<<std::endl;
    for(i=0;i<BOARDSIZE;++i)
    {
        for(j=0;j<BOARDSIZE;++j)
        {
            //if(i==j&&j==7)
            //{
            //    std::cout<<"4to za huita?"<<std::endl;
            //}
            //std::cout<<"i: "<<i<<"j: "<<j<<'\t';
            if(board[i][j]->isPlayable()&&board[i][j]->isWhite()!=white)
            {
                //std::cout<<"+";
                buf = board[i][j]->getAttackCandidates(true);
                for(std::pair<int,int>& el : buf)
                {
                    out.insert(el);
                }
            }
            //std::cout<<std::endl;
        }
    }
    return out;
}


void ChessBoard::debugPrintDanger()
{
    int i,j,k;
    std::set<std::pair<int,int>> danger;
    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<std::endl;
    for(k=0;k<2;k++)
    {    
        danger = ChessBoard::getDangerousPoints(board,k);
        for(i=0; i<8; i++)
        {
            for(j=0; j<8; j++)
            {
                if(danger.find({i,j})!=danger.end())
                {
                    std::cout<<"D"<<k<<" ";
                }
                else
                {
                    std::cout<<"S"<<k<<" ";
                }
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
        std::cout<<std::endl;
        std::cout<<std::endl;
    }
}
ChessBoard:: ChessBoard(Logger* log)
:log(log)
{
    int i;
    board = new ChessPieceBase**[8];
    for(i=0;i<2;i++)
    {
        board[0+i*7] = createFirstRow(i);
        board[1+i*5] = createPawnRow(i);
    }
    for (i = 2; i < 6; i++)
    {
        board[i] = createEmptyRow(i);
    }
    log->log("ДОСКА СОЗДАЛАСЬ ЕПТА!!!");
    auto test = getDangerousPoints(board, false);
}
ChessPieceBase** ChessBoard::createEmptyRow(int row)
{
    ChessPieceBase** out = new ChessPieceBase*[8];
    int i;

    for (i = 0; i < 8; i++)
    {
        out[i] = new ChessPieceEmpty(i,row,log,board);
    }
    return out;
}
ChessPieceBase** ChessBoard::createFirstRow(bool white)
{
    ChessPieceBase** out = new ChessPieceBase*[8];
    int y = white? 7 : 0;
    out[0] = new ChessPieceRook(0,y,white,log,board);
    out[1] = new ChessPieceKnight(1,y,white,log,board);
    out[2] = new ChessPieceBishop(2,y,white,log,board);
    out[3] = new ChessPieceQueen(3,y,white,log,board);
    out[4] = new ChessPeiceKing(4,y,white,log,board); // tut dolzhen byt korol
    out[5] = new ChessPieceBishop(5,y,white,log,board);
    out[6] = new ChessPieceKnight(6,y,white,log,board);
    out[7] = new ChessPieceRook(7,y,white,log,board);
    return out;
}
ChessPieceBase** ChessBoard::createPawnRow(bool white)
{
    ChessPieceBase** out = new ChessPieceBase*[8];
    int i, row;
    row = white? 6:1;

    for (i = 0; i < 8; i++)
    {
        out[i] = new ChessPiecePawn(i,row,white,log,board);
    }
    return out;
}

void ChessBoard::printBoard()
{
    int i,j;
    for(i=0; i<8; i++)
    {
        for(j=0; j<8; j++)
        {
            std::cout<<board[i][j]->print()<<' ';
        }
        std::cout<<std::endl;
    }
}
void ChessBoard::clear()
{
    int i,j;
    for(i=0;i<BOARDSIZE;++i)
    {
        for(j=0;j<BOARDSIZE;++j)
        {
            if(board[i][j]!=nullptr)
            {
                delete board[i][j];
            }
        }
        delete[] board[i];
        board[i] = createEmptyRow(i);
    }
}


const std::pair<std::pair<int,int>,std::pair<int,int>> ChessBoard::computeMove(bool white)
{

}
bool ChessBoard::performMove(const std::pair<std::pair<int,int>,std::pair<int,int>>& move)
{
    ChessPieceBase* buf;
    if(this->board!=nullptr)
    {
        if(this->board[move.first.first][move.first.second]->canAttack(move.second))
        {
            this->score[this->board[move.second.first][move.second.second]->isWhite()] += this->board[move.second.first][move.second.second]->getCode();
            delete this->board[move.second.first][move.second.second];
            this->board[move.second.first][move.second.second] = this->board[move.first.first][move.first.second];
            this->board[move.first.first][move.first.second]->move(move.second);
            this->board[move.first.first][move.first.second] = new ChessPieceEmpty(move.first.second,move.first.first,this->log,this->board);
            return true;
        }
        else if(this->board[move.first.first][move.first.second]->canMoveTo(move.second))
        {
            if(this->board[move.second.first][move.second.second]->getCode()!=ROOK)
            {
                this->board[move.second.first][move.second.second]->move(move.first);
                buf = this->board[move.second.first][move.second.second];
                this->board[move.second.first][move.second.second] = this->board[move.first.first][move.first.second];
                this->board[move.second.first][move.second.second]->move(move.second);
                this->board[move.first.first][move.first.second] = buf;
                return true;
            }
            else
            {
                throw std::logic_error("CASTLING IS NOT IMPLEMENTED (YET)");
            }
        }
    }
    return false;
}

ChessPieceBase*** ChessBoard::deleteBoard(ChessPieceBase*** board)
{
    ChessPieceBase*** out = new ChessPieceBase** [BOARDSIZE];
    int i,j;
    if(board && out)
    {
        for(i=0;i<BOARDSIZE;++i)
        {
            out[i] = new ChessPieceBase* [BOARDSIZE];
            if(board[i]&&out[i])
            {
                for(j=0;j<BOARDSIZE;++j)
                {
                    if(board[i][j])
                        out[i][j] = ChessBoard::createPeice()
                }
            }
            delete board[i];
        }
        delete board;
    }
    return nullptr;
}


ChessPieceBase*** ChessBoard::deleteBoard(ChessPieceBase*** board)
{
    int i,j;
    if(board)
    {
        for(i=0;i<BOARDSIZE;++i)
        {
            if(board[i])
            {
                for(j=0;j<BOARDSIZE;++j)
                {
                    if(board[i][j])
                        delete board[i][j];
                }
            }
            delete board[i];
        }
        delete board;
    }
    return nullptr;
}






ChessBoard::~ ChessBoard()
{


}

