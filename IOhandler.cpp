#include"IOhandler.h"

int patOrMate(bool side,ChessPieceBase*** board)
{
    int i,j,id;
    Special_Parameter checkMate=ChessBoard::evaluateCheckMate(side,board);
    std::vector<std::pair<int,int>> buf;
    for(i=0;i<BOARDSIZE;++i)
    {
        for(j=0;j<BOARDSIZE;++j)
        {
            if(board[i][j]->isWhite()==side)
            {
                buf = board[i][j]->getAttackCandidates(false);
                for(std::pair<int,int> el : board[i][j]->getMoveCandidates())
                {
                    buf.push_back(el);
                }
                id=ChessBoard::findFigureIndex(checkMate.restrictions,{i,j});
                if((checkMate.kingAttacked||id!=-1)&&board[i][j]->getCode()!=KING)
                {
                    buf=ChessBoard::filterMoves(buf,checkMate,id);
                }
                if(!buf.empty())
                {
                    return 1;
                }
            }
        }
    }
    return -1*checkMate.kingAttacked;
}

void IOhandler::toLowercase(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(),[](unsigned char c){ return std::tolower(c); });
}
IOhandler::IOhandler(std::ostream* output,std::istream* input)
:output(output),input(input)
{
    std::string response;
    std::ostream* out;
    bool silent;
    *output << "Do you want to log? [y/N] "<<std::flush;
    std::getline(*input, response);
    silent = response.empty() || response[0] == 'N' || response[0] == 'n';
    if(!silent&&response[0]=='S')
    {
        server=true;
    }
    else if(!silent)
    {
        *output <<"Where do you want to log? [standart/file] "<<std::flush;
        std::getline(*input, response);
        toLowercase(response);
        if(response == "standart")
        {
            out = &std::cout;
        }
        else if (response == "file")
        {
            *output <<"Enter filename (will be overwritten) "<<std::flush;
            std::getline(*input, response);
            toLowercase(response);
            out = new std::ofstream(response,std::ofstream::out | std::ofstream::app);
        }
        else
        {
            *output <<"Unknown input, assuming - standart"<<std::flush;
            out = &std::cout;
        }
    }
    log = new Logger(silent,out);
}
void IOhandler::mainLoop()
{
    loop=true;
    bool ok = true;
    std::string response;
    while(loop)
    {
        try
        {
            response = server? "OK":"~ (help for help): ";
            if(!server||ok)
                *output<<response<<std::flush;
            std::getline(*input, response);
            toLowercase(response);
            processInput(response);
            ok=true;
        }
        catch(std::out_of_range& range)
        {
            if(!server)
                *output<<"YOUR COMMAND '"<<response<<"' WAS GIVEN WRONG: "<<range.what()<<std::flush;
            else
            {
                *output<<"NOT OK"<<std::flush;
                std::cerr<<range.what()<<std::endl;
            }
            ok=false;
        }
        catch(std::invalid_argument& arg)
        {
            if(!server)
                *output<<"YOUR COMMAND '"<<response<<"' WAS GIVEN WRONG: "<<arg.what()<<std::flush;
            else
            {
                *output<<"NOT OK"<<std::flush;
                std::cerr<<arg.what()<<std::endl;
            }
            ok=false;
        }
        catch(std::logic_error& logic)
        {
            if(!server)
                *output<<"YOUR COMMAND '"<<response<<"' WAS GIVEN WRONG: "<<logic.what()<<std::flush;
            else
            {
                *output<<"NOT OK"<<std::flush;
                std::cerr<<logic.what()<<std::endl;
            }
            ok=false;
        }
        catch (std::exception& ex)
        {
            std::cerr<<ex.what()<<"UNABLE TO PROCEED"<<std::endl;
            loop=false;
        }
    }
}
std::vector<std::string> IOhandler::getPossibleOptions()
{
    std::vector<std::string> out;
    out.push_back("exit\t\t\texits the game");
    out.push_back("help\t\t\tdisplays this help");
    if(!gameIsOn)
    {
        out.push_back("start\t\t\tstarts a game");
    }
    else
    {
        out.push_back("move <start:end>\tperforms specified move");
        out.push_back("surrender\t\tyou instantly lose");
        out.push_back("print\t\t\tprints a board");
    }
    return out;
}
void IOhandler::processInput(const std::string& response)
{
    std::string response_;
    if(response=="help")
    {
        for(const std::string& el : getPossibleOptions())
        {
            *output<<el<<std::endl;
        }
    }
    else if(response=="exit")
    {
        loop=false;
        if(ch)
        {
            delete ch;
            ch=nullptr;
        }
    }
    else if(!gameIsOn&&response=="start")
    {
        response_ = server? "OK":"Chose a side [w/b]";
        *output<<response_<<std::flush;
        std::getline(*input, response_);
        toLowercase(response_);
        if(response_[0]=='w')
        {
            side = true;
            gameIsOn=startGame();
        }
        else if(response_[0]=='b')
        {
            side = false;
            gameIsOn=startGame();
        }
        else
        {
            if(!server)
                *output<<"Unknown input "<<std::flush;
        }
    }
    else if(gameIsOn&&response.size()==10&&response.substr(0,4)=="move")
    {
        move(response.substr(5,10));
    }
    else if(gameIsOn&&response.size()==8&&response.substr(0,5)=="moves")
    {
        printMoveCandidates(response.substr(6,8));
    }
    else if(gameIsOn&&response=="surrender")
    {
        if(ch)
        {
            delete ch;
            ch=nullptr;
        }
        *output<<"You lost!!!"<<std::flush;
        gameIsOn=false;
    }
    else if(gameIsOn&&response=="print")
    {
        if(ch)
        {
            ch->printBoard(output);
        }
    }
    else if(response=="prestart")
    {
        if(!server) 
            *output<<"Chose a side [w/b]"<<std::flush;
        std::getline(*input, response_);
        toLowercase(response_);
        if(response_[0]=='w')
        {
            side = true;
            gameIsOn=startPreDefinedGame();
        }
        else if(response_[0]=='b')
        {
            side = false;
            gameIsOn=startPreDefinedGame();
        }
        else
        {
            if(!server)
                *output<<"Unknown input "<<std::flush;
        }
    }
    else
    {
        throw std::invalid_argument("Unknown input ");
    }
}

bool IOhandler::startPreDefinedGame()
{
    if(ch)
    {
        delete ch;
        ch=nullptr;
    }
    std::string response_;
    int difficulty;
    if(!server)
        *output<<"Chose a difficulty [1-5]"<<std::flush;
    std::getline(*input, response_);
    toLowercase(response_);
    difficulty = std::stoi(response_);
    if(difficulty>=1&&difficulty<=10)
    {
        ch = new ChessBoard(log,difficulty);
        *output<<"Give board: "<<std::flush;
        std::getline(*input, response_);
        ch->makeBoardFromString(response_);
        if(!this->side)
        {
            ch->performMove(ch->getBestMove(!this->side),ch->getBoard());
        }
        ch->printBoard(output);
    }
    return ch!=nullptr;
}

bool IOhandler::startGame()
{
    if(ch)
    {
        delete ch;
        ch=nullptr;
    }
    std::string response_= server? "OK":"Chose a difficulty [1-5]";
    int difficulty;
    *output<<response_<<std::flush;
    std::getline(*input, response_);
    toLowercase(response_);
    difficulty = std::stoi(response_);
    if(difficulty>=1&&difficulty<=10)
    {
        ch = new ChessBoard(log,difficulty);
        if(ch)
        {
            if(!this->side)
            {
                ch->performMove(ch->getBestMove(!this->side),ch->getBoard());
            }
            ch->printBoard(output);
        }
    }
    return ch!=nullptr;
}
void IOhandler::move(const std::string& move)
{
    Move mv = {{move[1]-48,move[0]-48},{move[4]-48,move[3]-48}};
    Move bestMove;
    checkMate = {false,{},{}};
    bool isGood = true;
    int id;
    if (ch->getBoard()[mv.start.first][mv.start.second]->isWhite()==this->side&&ch->getBoard()[mv.start.first][mv.start.second]->getCode()!=EMPTY)
    {
        if(mv.start==mv.end)
        {
            throw std::logic_error("INVALID MOVE");
        }
        checkMate = ChessBoard::evaluateCheckMate(this->side,ch->getBoard());
        id =ChessBoard::findFigureIndex(checkMate.restrictions,mv.start);
        if(id!=-1)
        {
            isGood=std::find(checkMate.restrictions.at(id).unrestrictedPositions.begin(),checkMate.restrictions.at(id).unrestrictedPositions.end(),mv.end)!=checkMate.restrictions.at(id).unrestrictedPositions.end();
        }
        if(checkMate.kingAttacked&&isGood)
        {
            isGood=std::find(checkMate.saveKingPath.begin(),checkMate.saveKingPath.end(),mv.end)!=checkMate.saveKingPath.end();
        }
        if(isGood||ch->getBoard()[mv.start.first][mv.start.second]->getCode()==KING)
            ch->performMove(mv,ch->getBoard());
        else
            throw std::logic_error("CANT MOVE THERE, KING IS ATTACKED");
        try
        {
            bestMove = ch->getBestMove(!this->side);
            if(bestMove.start.first==-1)
            {
                *output<<"YOU WON!!!"<<std::flush;
                if(ch)
                {
                    delete ch;
                    ch=nullptr;
                }
                gameIsOn=false;
                return;
            }
            ch->performMove(bestMove,ch->getBoard());
            id=patOrMate(side,ch->getBoard());
            if(id==-1)
            {
                *output<<"YOU LOST!!!"<<std::flush;
            }
            else if(id==0)
            {
                *output<<"TIE!!!"<<std::flush;
            }
            ch->printBoard(output);
            if(id!=1)
            {
                if(ch)
                {
                    delete ch;
                    ch=nullptr;
                }
                gameIsOn=false;
                return;
            }

        }
        catch(...)
        {
            *output<<"ENEMY DECIDED TO SURRENDER"<<std::endl;
            *output<<"YOU WON!!!"<<std::flush;
            if(ch)
            {
                delete ch;
                ch=nullptr;
            }
            gameIsOn=false;
            return;
        }
        
    }
    else
    {
        throw std::logic_error("YOU CAN'T MOVE THAT FIGURE");
    }
    
}
void IOhandler::printMoveCandidates(std::string start)
{
    std::vector<std::pair<int,int>> buf = ch&&ch->getBoard()[start[1]-48][start[0]-48]->isWhite()==side? getMoveCandidates({start[0]-48,start[1]-48}):std::vector<std::pair<int,int>>{};
    int counter=0;
    if(buf.empty())
    {
        *output<<"NONE"<<std::endl;
    }
    else
    {
        for(std::pair<int,int> el : buf)
        {
            ++counter;
            *output<<el.first<<el.second;
            if(counter!=buf.size())
            {
                *output<<',';
            }
        }
        *output<<std::endl;
    }
}
std::vector<std::pair<int,int>> IOhandler::getMoveCandidates(std::pair<int,int> start)
{
    std::vector<std::pair<int,int>> out;
    int id = ChessBoard::findFigureIndex(checkMate.restrictions,start);
    out=ch->getBoard()[start.second][start.first]->getAttackCandidates(false);
    for(std::pair<int,int> el : ch->getBoard()[start.second][start.first]->getMoveCandidates())
    {
        out.push_back(el);
    }
    if(checkMate.kingAttacked)
    {
        out = ChessBoard::getOverlap(out,checkMate.saveKingPath);
    }
    if(id!=-1)
    {
        out = ChessBoard::getOverlap(out,checkMate.restrictions.at(id).unrestrictedPositions);
    }
    return out;
}

IOhandler::IOhandler()
:output(&std::cout),input(&std::cin)
{
    log = new Logger(true,nullptr);

}
IOhandler::~IOhandler()
{
}
// todo backtrack king being attacked and based on that get what cells cannot be moved and where they cant go
// nuzno peredavat v recursive i v obicny specialny parameter bana hotby dlya figur i sostojanije shaha i scitat kajdy raz kogda idem dalse v recurcive.