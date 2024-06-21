#include"chess-board.h"

ChessPieceBase* ChessBoard::createPeice(int x, int y,bool color, ChessPieceCode code, Logger* log, ChessPieceBase*** board)
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
ChessBoard:: ChessBoard(Logger* log,int difficulty)
:log(log),difficulty(difficulty)
{
    int i;
    board = new ChessPieceBase**[8];
    for(i=0;i<2;i++)
    {
        board[7-i*7] = createFirstRow(i);
        board[6-i*5] = createPawnRow(i);
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
    int y = white? 0 : 7;
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
    row = white? 1:6;

    for (i = 0; i < 8; i++)
    {
        out[i] = new ChessPiecePawn(i,row,white,log,board);
    }
    return out;
}

void ChessBoard::printImaginaryBoard(ChessPieceBase*** board,std::ostream* out = &std::cout)
{
    int i,j;
    for(i=7; i>=0; --i)
    {
        for(j=0; j<8; j++)
        {
            *out<<board[i][j]->print()<<' ';
        }
        *out<<std::endl;
    }
}
void ChessBoard::printBoard(std::ostream* out = &std::cout)
{
    int i,j;
    for(i=7; i>=0; --i)
    {
        for(j=0; j<8; j++)
        {
            *out<<board[i][j]->print()<<' ';
        }
        *out<<std::endl;
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

Move ChessBoard::getBestMove(bool white)
{
    int i,j,counter;
    int dScore;
    int maxScore;
    std::vector<Move_Candidate> out;
    std::map<float,std::pair<int,int>> scoreTable;
    ChessPieceBase*** imaginaryBoard = copyBoard(board);
    std::vector<std::pair<int,int>> buf;
    if(imaginaryBoard)
    {
        for(i=0;i<BOARDSIZE;++i)
        {
            for(j=0;j<BOARDSIZE;++j)
            {
                if(board[i][j]->isWhite()==white)
                {
                    buf = board[i][j]->getAttackCandidates(false);
                    for(std::pair<int,int> el : board[i][j]->getMoveCandidates())
                    {
                        buf.push_back(el);
                    }
                    for(std::pair<int,int> el : buf)
                    {
                        revertBoard(imaginaryBoard,board);
                        dScore = performMove(Move{{i,j},el},imaginaryBoard);
                        if(out.size()==0)
                        {
                            out.push_back({Move{{i,j},el},dScore});
                        }
                        else
                        {
                            for(counter=0;counter<out.size();++counter)
                            {
                                if(out.at(counter).dScore<dScore)
                                {
                                    out.insert(out.begin()+counter,{Move{{i,j},el},dScore});
                                    if(out.size()>difficulty)
                                    {
                                        out.pop_back();
                                    }
                                    break;
                                }
                                else if(counter==out.size()-1&&out.at(counter).dScore>=dScore&&out.size()<difficulty)
                                {
                                    out.push_back({Move{{i,j},el},dScore});
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        throw std::runtime_error("NOT ENOUGHT MEMORY");
    }
    std::cout<<"SIZE: "<<out.size()<<std::endl;
    for(i=0;i<out.size();++i)
    {
        revertBoard(imaginaryBoard,board);
        performMove(out.at(i).move,imaginaryBoard);
        if(i==0)
        {
            maxScore=out.at(i).dScore-recursiveSubroutine(imaginaryBoard,!white,difficulty,1);
            j=0;
        }
        else
        {
            dScore=out.at(i).dScore-recursiveSubroutine(imaginaryBoard,!white,difficulty,1);
            if(dScore>maxScore)
            {
                j=i;
            }
        }

    }
    deleteBoard(imaginaryBoard);
    try
    {
        return out.at(j).move;
    }
    catch(std::exception& ex)
    {
        //std::cout<<ex.what()<<std::endl;
        throw std::runtime_error("NO MOVE IS POSSIBLE" + out.size());
        return {{-1,-1},{-1,-1}};
    }


}
ChessPieceCode ChessBoard::askReplacement()
{
    return QUEEN;
}
const int ChessBoard::recursiveSubroutine(ChessPieceBase*** board, bool white, int difficulty, int depth)
{
    int i,j,counter;
    int dScore;
    int maxScore;
    std::vector<Move_Candidate> out;
    std::map<float,std::pair<int,int>> scoreTable;
    ChessPieceBase*** imaginaryBoard = copyBoard(board);
    std::vector<std::pair<int,int>> buf;
    if(imaginaryBoard)
    {
        for(i=0;i<BOARDSIZE;++i)
        {
            for(j=0;j<BOARDSIZE;++j)
            {
                if(board[i][j]->isWhite()==white)
                {
                    buf = board[i][j]->getAttackCandidates(false);
                    for(std::pair<int,int> el : board[i][j]->getMoveCandidates())
                    {
                        buf.push_back(el);
                    }
                    for(std::pair<int,int> el : buf)
                    {
                        revertBoard(imaginaryBoard,board);
                        dScore = performMove(Move{{i,j},el},imaginaryBoard);
                        if(out.size()==0)
                        {
                            out.push_back({Move{{i,j},el},dScore});
                        }
                        else
                        {
                            for(counter=0;counter<out.size();++counter)
                            {
                                if(out.at(counter).dScore<dScore)
                                {
                                    out.insert(out.begin()+counter,{Move{{i,j},el},dScore});
                                    if(out.size()>difficulty)
                                    {
                                        out.pop_back();
                                    }
                                    break;
                                }
                                else if(counter==out.size()-1&&out.at(counter).dScore>=dScore&&out.size()<difficulty)
                                {
                                    out.push_back({Move{{i,j},el},dScore});
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        throw std::runtime_error("NOT ENOUGHT MEMORY");
    }
    //std::cout<<"SIZE: "<<out.size()<<std::endl;
    if(depth==difficulty)
    {
        deleteBoard(imaginaryBoard);
        if(out.size()>=1)
        {
            return out.at(0).dScore;
        }
        return -999999;
    }
    else
    {
        for(i=0;i<out.size();++i)
        {
            revertBoard(imaginaryBoard,board);
            performMove(out.at(i).move,imaginaryBoard);
            if(i==0)
            {
                maxScore=out.at(i).dScore-recursiveSubroutine(imaginaryBoard,!white,difficulty,depth+1);
            }
            else
            {
                dScore=out.at(i).dScore-recursiveSubroutine(imaginaryBoard,!white,difficulty,depth+1);
                if(dScore>maxScore)
                {
                    maxScore=dScore;
                }
            }

        }
        deleteBoard(imaginaryBoard);
        if(out.size()==0)
        {
            return -999999;
        }
        return maxScore;
    }
}
float ChessBoard::performMove(const Move& move,ChessPieceBase*** board)
{
    ChessPieceBase* buf;
    float score;
    ChessPieceCode code;
    if(board!=nullptr)
    {
        if(board[move.start.first][move.start.second]->canAttack(move.end))
        {
            score = board[move.end.first][move.end.second]->getCode();
            delete board[move.end.first][move.end.second];
            board[move.end.first][move.end.second] = board[move.start.first][move.start.second];
            board[move.start.first][move.start.second]->move(move.end);
            board[move.start.first][move.start.second] = new ChessPieceEmpty(move.start.second,move.start.first,board[move.end.first][move.end.second]->getLogger(),board);
            return score;
        }
        else if(board[move.start.first][move.start.second]->canMoveTo(move.end))
        {
            
            if(board[move.end.first][move.end.second]->getCode()!=ROOK)
            {
                score = 0;
                for(std::pair<int,int> coord : board[move.start.first][move.start.second]->getMoveCandidates())
                {
                    score -= board[coord.first][coord.second]->getCode()/2.f;
                } 
                board[move.end.first][move.end.second]->move(move.start);
                if(board[move.start.first][move.start.second]->getCode()==PAWN&&move.end.first==7*board[move.end.first][move.end.second]->isWhite())
                {   code=askReplacement();
                    buf = createPeice(move.end.second,move.end.first,board[move.end.first][move.end.second]->isWhite(),
                    code,board[move.end.first][move.end.second]->getLogger(),board);
                    delete board[move.start.first][move.start.second];
                    board[move.start.first][move.start.second] = board[move.end.first][move.end.second];
                    board[move.end.first][move.end.second] = buf;
                    return code;
                }
                buf = board[move.end.first][move.end.second];
                board[move.end.first][move.end.second] = board[move.start.first][move.start.second];
                board[move.end.first][move.end.second]->move(move.end);
                board[move.start.first][move.start.second] = buf;
                for(std::pair<int,int> coord : board[move.end.first][move.end.second]->getMoveCandidates())
                {
                    score += board[coord.first][coord.second]->getCode()/2.f;
                } 
                return score;
            }
            else
            {
                throw std::logic_error("CASTLING IS NOT IMPLEMENTED (YET)");
            }
        }
    }
    throw std::logic_error("CAN'T MOVE");
}

ChessPieceBase*** ChessBoard::copyBoard(ChessPieceBase*** board, bool notImaginary = false)
{
    ChessPieceBase*** out = new ChessPieceBase** [BOARDSIZE];
    ChessPieceBase* buf;
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
                    {
                        buf=nullptr;
                        buf = ChessBoard::createPeice(board[i][j]->getX(),board[i][j]->getY(),board[i][j]->isWhite(),board[i][j]->getCode(),(Logger*)((size_t)(board[i][j]->getLogger())*notImaginary),out);
                        if(buf)
                        {
                            out[i][j]=buf;
                        }
                        else
                        {
                            return nullptr;
                        }
                    }
                }
            }
            else
            {
                return nullptr;
            }
        }
        return out;
    }
    return nullptr;
}

void ChessBoard::revertBoard(ChessPieceBase*** imaginaryBoard,ChessPieceBase*** board)
{
    ChessPieceBase* buf;
    int i,j;
    if(board && imaginaryBoard)
    {
        for(i=0;i<BOARDSIZE;++i)
        {
            if(board[i]&&imaginaryBoard[i])
            {
                for(j=0;j<BOARDSIZE;++j)
                {
                    if(board[i][j])
                    {
                        buf=nullptr;
                        buf = ChessBoard::createPeice(board[i][j]->getX(),board[i][j]->getY(),board[i][j]->isWhite(),board[i][j]->getCode(),nullptr,imaginaryBoard);
                        if(buf)
                        {
                            delete imaginaryBoard[i][j];
                            imaginaryBoard[i][j]=buf;
                        }
                        else
                        {
                            throw std::runtime_error("CRITICAL ERROR, IMPOSSIBLE TO REVERT BOARD");
                        }
                    }
                    else
                    {
                        throw std::runtime_error("CRITICAL ERROR, IMPOSSIBLE TO REVERT BOARD");
                    }
                }
            }
        }
    }
    else
    {
        throw std::runtime_error("CRITICAL ERROR, IMPOSSIBLE TO REVERT BOARD");
    }
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


void ChessBoard::cycleFigure(std::pair<int,int> pos, bool color,ChessPieceCode code)
{
    ChessPieceCode code_;
    if(pos.first>=0&&pos.second>=0&&pos.first<BOARDSIZE&&pos.second<BOARDSIZE)
    {
        if(board[pos.first][pos.second]==nullptr)
        {
            board[pos.first][pos.second] = createPeice(pos.second,pos.first,color,code,log,board);
            return;
        }
        code_=board[pos.first][pos.second]->getCode();
        delete board[pos.first][pos.second];
        if(code_==EMPTY)
        {
            board[pos.first][pos.second] = createPeice(pos.second,pos.first,color,code,log,board);
            return;
        }
            board[pos.first][pos.second] = createPeice(pos.second,pos.first,color,EMPTY,log,board);
            return;
    }
}



ChessBoard::~ ChessBoard()
{


}

