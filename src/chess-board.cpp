#include"chess-board.h"
static int debugCounter=0;

std::vector<std::pair<int,int>> ChessBoard::getOverlap(const std::vector<std::pair<int,int>>& el1,const std::vector<std::pair<int,int>>& el2)
{
    std::vector<std::pair<int,int>> out;
    for(std::pair<int,int> pr : el1)
    {
        if(std::find(el2.begin(),el2.end(),pr)!=el2.end())
        {
            out.push_back(pr);
        }
    }
    return out;
}

ChessPieceBase* ChessBoard::createPeice(int x, int y,bool color, ChessPieceCode code, Logger* log, ChessPieceBase*** board,bool moved_=false)
{
    switch (code)
    {
    case KING:
        return new ChessPeiceKing(x,y,color,log,board,moved_);
        break;
    case QUEEN:
        return new ChessPieceQueen(x,y,color,log,board,moved_);
        break;
    case BISHOP:
        return new ChessPieceBishop(x,y,color,log,board,moved_);
        break;
    case KNIGHT:
        return new ChessPieceKnight(x,y,color,log,board,moved_);
        break;
    case ROOK:
        return new ChessPieceRook(x,y,color,log,board,moved_);
        break;
    case PAWN:
        return new ChessPiecePawn(x,y,color,log,board,moved_);
        break;
    case EMPTY:
        return new ChessPieceEmpty(x,y,log,board);
        break;
    default:
        throw std::runtime_error("UNKNOWN OPTION");
        break;
    }
}

ChessPieceBase* ChessBoard::createPeiceFromString(int x, int y,bool color, char code, Logger* log, ChessPieceBase*** board,bool moved_=false)
{
    switch (code)
    {
    case 'K':
        return new ChessPeiceKing(x,y,color,log,board,moved_);
        break;
    case 'Q':
        return new ChessPieceQueen(x,y,color,log,board,moved_);
        break;
    case 'B':
        return new ChessPieceBishop(x,y,color,log,board,moved_);
        break;
    case 'H':
        return new ChessPieceKnight(x,y,color,log,board,moved_);
        break;
    case 'R':
        return new ChessPieceRook(x,y,color,log,board,moved_);
        break;
    case 'P':
        return new ChessPiecePawn(x,y,color,log,board,moved_);
        break;
    case '0':
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
                for(std::pair<int,int> el : buf)
                {

                        out.insert(el);

                }
            }
            //std::cout<<std::endl;
        }
    }
    return out;
}

int ChessBoard::findFigureIndex(const std::vector<Figure_Move_Restriction> restrictions,std::pair<int,int> pos)
{
    int counter = 0;
    for(const Figure_Move_Restriction& el : restrictions)
    {
        if(el.position==pos)
        {
            return counter;
        }
        counter++;
    }
    return -1;
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
    board = new ChessPieceBase**[BOARDSIZE];
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
    //auto test = getDangerousPoints(board, false);
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
void ChessBoard::threadFunc(Thread_Parameter* param)
{
    param->score=recursiveSubroutine(param->board,!param->white,param->difficulty,1,param->maxDepth);
    deleteBoard(param->board);
    param->ready=true;
}
void ChessBoard::makeBoardFromString(const std::string& str)
{
    std::istringstream iss(str);
    int counter=0;
    int i,j;
    std::string buf;
    deleteBoard(this->board);
    board = new ChessPieceBase**[BOARDSIZE];
    if(!board)
    {
        throw std::runtime_error("NOT ENOUGHT MEMORY");
    }
    for(i=0;i<BOARDSIZE;++i)
    {
        board[i]=nullptr;
        board[i]=new ChessPieceBase*[BOARDSIZE];
        if(!board[i])
        {
            throw std::runtime_error("NOT ENOUGHT MEMORY");
        }
    }
    i=0;
    j=0;
    while (iss>>buf&&counter!=64)
    {
        board[7-i][j]=createPeiceFromString(j,7-i,buf[0]=='W',buf[1],this->log,this->board);
        j++;
        if(j==BOARDSIZE)
        {
            ++i;
            j=0;
        }
    }
    


}
Move ChessBoard::getBestMove(bool white)
{
    int i,j,counter;
    int dScore;
    int maxScore;
    std::vector<std::thread> threads;
    Thread_Parameter* param;
    std::vector<Thread_Parameter*> params;
    std::vector<Move_Candidate> out;
    std::map<float,std::pair<int,int>> scoreTable;
    ChessPieceBase*** imaginaryBoard = copyBoard(board);
    std::vector<std::pair<int,int>> buf;
    Special_Parameter checkMate;
    int id;
    if(imaginaryBoard)
    {
        checkMate=evaluateCheckMate(white,imaginaryBoard);
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
                    id=findFigureIndex(checkMate.restrictions,{i,j});
                    if((checkMate.kingAttacked||id!=-1)&&board[i][j]->getCode()!=KING)
                    {
                        buf=filterMoves(buf,checkMate,id);
                    }
                    for(std::pair<int,int> el : buf)
                    {
                        revertBoard(imaginaryBoard,board);
                        dScore = performMove(Move{{i,j},el},imaginaryBoard,true);
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
   // std::cout<<"SIZE: "<<out.size()<<std::endl;
    for(i=0;i<out.size();++i)
    {
        param = new Thread_Parameter;
        if(param)
        {
            param->board=copyBoard(board);
            performMove(out.at(i).move,param->board,true);
            param->difficulty=difficulty-1;
            param->maxDepth=difficulty-1;
            param->white=white;
            param->ready=false;
        }
        else
        {
            throw std::runtime_error("NOT ENOUGHT MEMORY");
        }
        params.push_back(param);
        threads.push_back(std::thread{threadFunc,param});
        threads.back().detach();

    }
    for(i=0;i<out.size();++i)
    {
        if(params[i]->ready)
        {
            if(i==0)
            {
                maxScore=out.at(i).dScore-params[i]->score;
                j=0;
            }
            else
            {
                dScore=out.at(i).dScore-params[i]->score;
                if(dScore>maxScore)
                {
                    j=i;
                }
            }
        }
        else
        {
            --i;
        }
    }
    for(i=0;i<out.size();++i)
    {
        delete params[i];        
    }
    params.clear();
    threads.clear();
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
std::vector<std::pair<int,int>> ChessBoard::filterMoves(const std::vector<std::pair<int,int>>& input,Special_Parameter checkMate,int usedIndex)
{
    std::vector<std::pair<int,int>> out;
    std::vector<std::pair<int,int>> restrictions;

    if(usedIndex<checkMate.restrictions.size()&&usedIndex!=-1)
    {
        if(checkMate.kingAttacked)
        {
            for(std::pair<int,int> el : checkMate.restrictions.at(usedIndex).unrestrictedPositions)
            {
                if(std::find(checkMate.saveKingPath.begin(),checkMate.saveKingPath.end(),el)!=checkMate.saveKingPath.end())
                {
                    restrictions.push_back(el);
                }
            }
        }
        else
        {
            restrictions=checkMate.restrictions.at(usedIndex).unrestrictedPositions;
        }
    }
    else
    {
        restrictions=checkMate.saveKingPath;
    }
    for(std::pair<int,int> el : input)
    {
        if(std::find(restrictions.begin(),restrictions.end(),el)!=restrictions.end())
        {
            out.push_back(el);
        }
    }
    return out;
}
const int ChessBoard::recursiveSubroutine(ChessPieceBase*** board, bool white, int difficulty, int depth,int maxDepth)
{
    int i,j,counter;
    int dScore;
    int maxScore;
    std::vector<Move_Candidate> out;
    std::map<float,std::pair<int,int>> scoreTable;
    ChessPieceBase*** imaginaryBoard = copyBoard(board);
    std::vector<std::pair<int,int>> buf;
    Special_Parameter checkMate;
    int id;
    if(imaginaryBoard)
    {
        checkMate = evaluateCheckMate(white,imaginaryBoard);
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
                    id=findFigureIndex(checkMate.restrictions,{i,j});
                    if((checkMate.kingAttacked||id!=-1)&&board[i][j]->getCode()!=KING)
                    {
                        buf=filterMoves(buf,checkMate,id);
                    }
                    for(std::pair<int,int> el : buf)
                    {
                        revertBoard(imaginaryBoard,board);
                        dScore = performMove(Move{{i,j},el},imaginaryBoard,true);

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
    if(depth==maxDepth)
    {
        deleteBoard(imaginaryBoard);
        if(out.size()>=1)
        {
            return out.at(0).dScore;
        }
        else if(checkMate.kingAttacked)
            return Mate;
        else
            return Pate;
    }
    else
    {
        for(i=0;i<out.size();++i)
        {
            revertBoard(imaginaryBoard,board);
            performMove(out.at(i).move,imaginaryBoard,true);
            if(i==0)
            {
                maxScore=out.at(i).dScore-recursiveSubroutine(imaginaryBoard,!white,difficulty-1,depth+1,maxDepth);
            }
            else
            {
                dScore=out.at(i).dScore-recursiveSubroutine(imaginaryBoard,!white,difficulty-1,depth+1,maxDepth);
                if(dScore>maxScore)
                {
                    maxScore=dScore;
                }
            }

        }
        deleteBoard(imaginaryBoard);
        if(out.size()==0)
        {
            if(checkMate.kingAttacked)
                return Mate;
            else
                return Pate;
        }
        return maxScore;
    }
}
float ChessBoard::performMove(const Move& move,ChessPieceBase*** board,bool overrideRightess=false)
{
    ChessPieceBase* buf;
    float score;
    std::pair<int,int> bufMoveKing;
    std::pair<int,int> bufMoveRook; 
    ChessPieceCode code;
    if(board!=nullptr)
    {
        if(board[move.end.first][move.end.second]->getCode()==KING)
        {
            printImaginaryBoard(board);
            std::cout<<move.start.second<<"\t"<<move.start.first<<std::endl;
            throw std::runtime_error("CANNOT ATTACK KING");

        }
        if(overrideRightess)
        {
            if(board[move.start.first][move.start.second]->isWhite()!=board[move.end.first][move.end.second]->isWhite()&&board[move.end.first][move.end.second]->getCode()!=EMPTY)
            {
                score = board[move.end.first][move.end.second]->getCode();
                if(!board[move.start.first][move.start.second]->hasMoved()&&board[move.start.first][move.start.second]->getCode()!=KING&&board[move.start.first][move.start.second]->getCode()!=ROOK)
                {
                    score += FirstMove; 
                }
                delete board[move.end.first][move.end.second];
                board[move.end.first][move.end.second] = board[move.start.first][move.start.second];
                board[move.start.first][move.start.second]->move(move.end);
                board[move.start.first][move.start.second] = new ChessPieceEmpty(move.start.second,move.start.first,board[move.end.first][move.end.second]->getLogger(),board);
                return score;
            }
            else
            {
                if(board[move.end.first][move.end.second]->getCode()!=ROOK)
                {
                    score = 0;
                    for(std::pair<int,int> coord : board[move.start.first][move.start.second]->getAttackCandidates(true))
                    {
                        score -= board[coord.first][coord.second]->getCode()/2.f;
                    }
                    if(!board[move.start.first][move.start.second]->hasMoved()&&board[move.start.first][move.start.second]->getCode()!=KING&&board[move.start.first][move.start.second]->getCode()!=ROOK)
                    {
                        score += FirstMove; 
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
                    for(std::pair<int,int> coord : board[move.end.first][move.end.second]->getAttackCandidates(true))
                    {
                        score += board[coord.first][coord.second]->getCode()/2.f;
                    } 
                    return score;
                }
                else
                {                
                    bool white;                                     
                    score = 0;
                    for(std::pair<int,int> coord : board[move.start.first][move.start.second]->getAttackCandidates(true))
                    {
                        score -= board[coord.first][coord.second]->getCode()/2.f;
                    }
                    for(std::pair<int,int> coord : board[move.end.first][move.end.second]->getAttackCandidates(true))
                    {
                        score -= board[coord.first][coord.second]->getCode()/2.f;
                    }
                    if(move.end.second==0)
                    {
                        bufMoveKing={move.start.first,move.start.second-2};
                        bufMoveRook={move.start.first,move.start.second-1};
                    }
                    else if(move.end.second==7)
                    {
                        bufMoveKing={move.start.first,move.start.second+2};
                        bufMoveRook={move.start.first,move.start.second+1};
                    }
                    else
                    {
                        bufMoveKing={move.start.first+2-4*move.start.first==7,move.start.second};
                        bufMoveKing={move.start.first+2-4*move.start.first==7,move.start.second};                            
                    }
                    performMove({move.end,bufMoveRook},board,true);
                    performMove({move.start,bufMoveKing},board,true);
                    for(std::pair<int,int> coord : board[bufMoveKing.first][bufMoveKing.second]->getAttackCandidates(true))
                    {
                        score += board[coord.first][coord.second]->getCode()/2.f;
                    }
                    for(std::pair<int,int> coord : board[bufMoveKing.first][bufMoveRook.second]->getAttackCandidates(true))
                    {
                        score += board[coord.first][coord.second]->getCode()/2.f;
                    }
                    score+=Castling;
                    return score;
                }
            }
        }
        else
        {
            if(board[move.start.first][move.start.second]->canAttack(move.end))
            {
                score = board[move.end.first][move.end.second]->getCode();
                if(!board[move.start.first][move.start.second]->hasMoved()&&board[move.start.first][move.start.second]->getCode()!=KING&&board[move.start.first][move.start.second]->getCode()!=ROOK)
                {
                    score += FirstMove; 
                }
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
                    for(std::pair<int,int> coord : board[move.start.first][move.start.second]->getAttackCandidates(true))
                    {
                        score -= board[coord.first][coord.second]->getCode()/2.f;
                    } 
                    if(!board[move.start.first][move.start.second]->hasMoved()&&board[move.start.first][move.start.second]->getCode()!=KING&&board[move.start.first][move.start.second]->getCode()!=ROOK)
                    {
                        score += FirstMove; 
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
                    for(std::pair<int,int> coord : board[move.end.first][move.end.second]->getAttackCandidates(true))
                    {
                        score += board[coord.first][coord.second]->getCode()/2.f;
                    } 
                    return score;
                }
                else
                {                   
                    bool white;
                    score = 0;
                    for(std::pair<int,int> coord : board[move.start.first][move.start.second]->getAttackCandidates(true))
                    {
                        score -= board[coord.first][coord.second]->getCode()/2.f;
                    }
                    for(std::pair<int,int> coord : board[move.end.first][move.end.second]->getAttackCandidates(true))
                    {
                        score -= board[coord.first][coord.second]->getCode()/2.f;
                    }
                    if(move.end.second==0)
                    {
                        bufMoveKing={move.start.first,move.start.second-2};
                        bufMoveRook={move.start.first,move.start.second-1};
                    }
                    else if(move.end.second==7)
                    {
                        bufMoveKing={move.start.first,move.start.second+2};
                        bufMoveRook={move.start.first,move.start.second+1};
                    }
                    else
                    {
                        bufMoveKing={move.start.first+2-4*move.start.first==7,move.start.second};
                        bufMoveKing={move.start.first+2-4*move.start.first==7,move.start.second};                            
                    }
                    performMove({move.end,bufMoveRook},board,true);
                    performMove({move.start,bufMoveKing},board,true);
                    for(std::pair<int,int> coord : board[bufMoveKing.first][bufMoveKing.second]->getAttackCandidates(true))
                    {
                        score += board[coord.first][coord.second]->getCode()/2.f;
                    }
                    for(std::pair<int,int> coord : board[bufMoveKing.first][bufMoveRook.second]->getAttackCandidates(true))
                    {
                        score += board[coord.first][coord.second]->getCode()/2.f;
                    }
                    score+=Castling;
                    return score;
                }
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
                        buf = ChessBoard::createPeice(board[i][j]->getX(),board[i][j]->getY(),board[i][j]->isWhite(),board[i][j]->getCode(),(Logger*)((size_t)(board[i][j]->getLogger())*notImaginary),out,board[i][j]->hasMoved());
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
                        buf = ChessBoard::createPeice(board[i][j]->getX(),board[i][j]->getY(),board[i][j]->isWhite(),board[i][j]->getCode(),nullptr,imaginaryBoard,board[i][j]->hasMoved());
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

std::pair<int,int> ChessBoard::findKing(bool side, ChessPieceBase*** board)
{
    int i,j;
    for(i=0;i<BOARDSIZE;++i)
    {
        for(j = side? 0:7;j<BOARDSIZE&&j>=0;j+=-1+2*side)
        {
            if(board[j][i]->getCode()==KING&&board[j][i]->isWhite()==side)
                return {j,i};
        }
    }
    throw std::logic_error("NO KING WAS FOUND -> ???");
}

bool ChessBoard::isDangerous(int distance,std::pair<int,int> kingPos,int8_t dX,int8_t dY, ChessPieceBase* suspect)
{
    ChessPieceCode code = suspect->getCode();
    std::vector<std::pair<int,int>> buf;
    if(distance==0)
    {
        if(code==PAWN)
        {
            return dX!=0&&dY==1-2*suspect->isWhite();
        }
    }
    if(dX!=0&&dY!=0)
    {
        return code==QUEEN||code==BISHOP;
    }
    else if(dX==0||dY==0)
    {
        return code==ROOK||code==QUEEN;
    }
    return false;
}

bool ChessBoard::simplifiedEvaluateCheckMate(bool side,std::pair<int,int> kingPosition,ChessPieceBase*** board)
{
    const int8_t rotationWheel[10]={0,1,1,1,0,-1,-1,-1,0,1};
    ChessPieceBase* candidate;
    const int8_t shift=2;
    int i,j,x,y,counter,distance;
    for(j=-2;j<=2;j+=4)
    {
        for(i=-1;i<=1;i+=2)
        {
            x=kingPosition.second+j;
            y=kingPosition.first+i;
            if(x>=0&&x<BOARDSIZE&&y>=0&&y<BOARDSIZE&&board[y][x]->getCode()==KNIGHT&&board[y][x]->isWhite()!=side)
            {
                return true;
            }
            x=kingPosition.second+i;
            y=kingPosition.first+j;
            if(x>=0&&x<BOARDSIZE&&y>=0&&y<BOARDSIZE&&board[y][x]->getCode()==KNIGHT&&board[y][x]->isWhite()!=side)
            {
                return true;
            }
        }           
    }
    for(counter=0;counter<8;++counter)
    {
        distance=0;
        x=kingPosition.second;
        y=kingPosition.first;
        x+=rotationWheel[counter];
        y+=rotationWheel[counter+shift];
        while(x<BOARDSIZE&&y<BOARDSIZE&&x>=0&&y>=0)
        {
            candidate = board[y][x];
            if(candidate->getCode()!=EMPTY&&candidate->getCode()!=KING)
            {
                if(candidate->isWhite()!=side)
                {
                    if(isDangerous(distance,kingPosition,rotationWheel[counter],rotationWheel[counter+shift],candidate))
                    {
                        return true;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            ++distance;
            x+=rotationWheel[counter];
            y+=rotationWheel[counter+shift];
        }
    }
    return false;
}

Special_Parameter ChessBoard::evaluateCheckMate(bool side,ChessPieceBase*** board)
{
    const int8_t rotationWheel[10]={0,1,1,1,0,-1,-1,-1,0,1};
    ChessPieceBase* candidate;
    Special_Parameter out{false,{},{}};
    std::pair kingPosition = findKing(side,board);
    Figure_Move_Restriction buf;
    const int8_t shift=2;
    int i,j,x,y,counter,distance;
    bool obstacle;
    for(j=-2;j<=2;j+=4)
    {
        for(i=-1;i<=1;i+=2)
        {
            x=kingPosition.second+j;
            y=kingPosition.first+i;
            if(x>=0&&x<BOARDSIZE&&y>=0&&y<BOARDSIZE&&board[y][x]->getCode()==KNIGHT&&board[y][x]->isWhite()!=side)
            {
                out.saveKingPath.push_back({y,x});
                out.kingAttacked=true;
            }
            x=kingPosition.second+i;
            y=kingPosition.first+j;
            if(x>=0&&x<BOARDSIZE&&y>=0&&y<BOARDSIZE&&board[y][x]->getCode()==KNIGHT&&board[y][x]->isWhite()!=side)
            {
                out.saveKingPath.push_back({y,x});
                out.kingAttacked=true;
            }
        }           
    }
    for(counter=0;counter<8;++counter)
    {
        buf={{-1,-1},{}};
        obstacle=false;
        distance=0;
        x=kingPosition.second;
        y=kingPosition.first;
        x+=rotationWheel[counter];
        y+=rotationWheel[counter+shift];
        while(x<BOARDSIZE&&y<BOARDSIZE&&x>=0&&y>=0)
        {
            candidate = board[y][x];
            buf.unrestrictedPositions.push_back({y,x});
            if(candidate->getCode()!=EMPTY)
            {
                if(candidate->isWhite()!=side)
                {
                    if(isDangerous(distance,kingPosition,rotationWheel[counter],rotationWheel[counter+shift],candidate))
                    {
                        if(buf.position==std::pair{-1,-1})
                        {
                            out.kingAttacked=true;
                            if(out.kingAttacked)
                            {
                                out.saveKingPath=getOverlap(out.saveKingPath,buf.unrestrictedPositions);
                            }
                            else
                            {
                                out.saveKingPath=buf.unrestrictedPositions;
                            }
                            break;
                        }
                        else
                        {
                            out.restrictions.push_back(buf);
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    if(buf.position==std::pair{-1,-1})
                    {
                        buf.position={y,x};
                    }
                    else
                    {
                        break;
                    }
                }
            }
            ++distance;
            x+=rotationWheel[counter];
            y+=rotationWheel[counter+shift];
        }
    }
    return out;
}

ChessBoard::~ ChessBoard()
{


}
