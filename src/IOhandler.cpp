#include"IOhandler.h"
#include"chess-peice-codes.h"
int patOrMate(bool side,ChessPieceBase*** board,Special_Parameter& checkMate)
{
    int i,j,id;
    checkMate=ChessBoard::evaluateCheckMate(side,board);
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
    std::ostream* out = nullptr;
    bool silent;
    *output << "Do you want to log? [y/N] "<<std::endl;
    std::getline(*input, response);
    silent = response.empty() || response[0] == 'N' || response[0] == 'n';
    if(!silent&&response[0]=='S')
    {
        server=true;
        silent = true;
    }
    else if(!silent)
    {
        *output <<"Where do you want to log? [standart/file] "<<std::endl;
        std::getline(*input, response);
        toLowercase(response);
        if(response == "standart")
        {
            out = &std::cout;
        }
        else if (response == "file")
        {
            *output <<"Enter filename (will be overwritten) "<<std::endl;
            std::getline(*input, response);
            toLowercase(response);
            out = new std::ofstream(response,std::ofstream::out | std::ofstream::app);
        }
        else
        {
            *output <<"Unknown input, assuming - standart"<<std::endl;
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
                *output<<response<<std::endl;
            std::getline(*input, response);
            toLowercase(response);
            processInput(response);
            ok=true;
        }
        catch(std::out_of_range& range)
        {
            if(!server)
                *output<<"YOUR COMMAND '"<<response<<"' WAS GIVEN WRONG: "<<range.what()<<std::endl;
            else
            {
                *output<<"NOT OK"<<std::endl;
                std::cerr<<range.what()<<std::endl;
            }
            ok=false;
        }
        catch(std::invalid_argument& arg)
        {
            if(!server)
                *output<<"YOUR COMMAND '"<<response<<"' WAS GIVEN WRONG: "<<arg.what()<<std::endl;
            else
            {
                *output<<"NOT OK"<<std::endl;
                std::cerr<<arg.what()<<std::endl;
            }
            ok=false;
        }
        catch(std::logic_error& logic)
        {
            if(!server)
                *output<<"YOUR COMMAND '"<<response<<"' WAS GIVEN WRONG: "<<logic.what()<<std::endl;
            else
            {
                *output<<"NOT OK"<<std::endl;
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
        *output<<response_<<std::endl;
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
                *output<<"Unknown input "<<std::endl;
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
        *output<<"You lost!!!"<<std::endl;
        checkMate={false,{},{}};
        gameIsOn=false;
    }
    else if(response=="prestart")
    {
        if(!server) 
            *output<<"Chose a side [w/b]"<<std::endl;
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
                *output<<"Unknown input "<<std::endl;
        }
    }
    else if(response=="set params")
    {
        setParams();
    }
    else if(gameIsOn&&response=="print")
    {
        printBoard();
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
    checkMate={false,{},{}};
    std::string response_;
    int difficulty;
    if(!server)
        *output<<"Chose a difficulty [1-5]"<<std::endl;
    std::getline(*input, response_);
    toLowercase(response_);
    difficulty = std::stoi(response_);
    if(difficulty>=1&&difficulty<=10)
    {
        ch = new ChessBoard(log,difficulty);
        *output<<"Give board: "<<std::endl;
        std::getline(*input, response_);
        ch->makeBoardFromString(response_);
        if(!this->side)
        {
            ch->performMove(ch->getBestMove(!this->side),ch->getBoard());
        }
        printBoard();
    }
    return ch!=nullptr;
}
std::pair<int,int> IOhandler::transcodePosition(std::string str)
{
    std::pair<int,int> out;
    int i,j;
    if(str.size()!=2)
    {
        throw std::out_of_range("SIZE MUST BE 2");
    }
    i=std::stoi(str);
    j=i%10;
    i=i/10;
    if(side==true)
    {
        out={j,i};
    }
    else
    {
        out={7-j,7-i};
    }
    return out;
}
bool IOhandler::startGame()
{
    if(ch)
    {
        delete ch;
        ch=nullptr;
    }
    checkMate={false,{},{}};
    std::string response_= server? "OK":"Chose a difficulty [1-10 (more is not recomended)]";
    int difficulty;
    *output<<response_<<std::endl;
    std::getline(*input, response_);
    toLowercase(response_);
    difficulty = std::stoi(response_);
    if(difficulty>=1)
    {
        ch = new ChessBoard(log,difficulty);
        if(ch)
        {
            if(!this->side)
            {
                ch->performMove(ch->getBestMove(!this->side),ch->getBoard());
            }
            if(!server)
                printBoard();
        }
    }
    return ch!=nullptr;
}
void IOhandler::setParams()
{
    std::string response;
    std::string figureNames[] = {"KING", "QUEEN", "ROOK", "BISHOP", "KNIGHT", "PAWN", "EMPTY"};
    int i;
    if(server)
        *output<<"OK"<<std::endl;
    try
    {
        for(i=0;i<7;++i)
        {
            if(!server)
                *output<<"Price for "+figureNames[i]<<std::endl;
            std::getline(*input,response);
            prices[i] = std::stoi(response);
            if(server)
                *output<<"OK"<<std::endl;
        }
        if(!server)
            *output<<"Price for Mate"<<std::endl;
        std::getline(*input,response);
        Mate = std::stoi(response);
        if(server)
            *output<<"OK"<<std::endl;
        else
            *output<<"Price for Pate"<<std::endl;
        std::getline(*input,response);
        Pate = std::stoi(response);
        if(server)
            *output<<"OK"<<std::endl;
        else
            *output<<"Price for First Move"<<std::endl;
        std::getline(*input,response);
        FirstMove = std::stoi(response);
        if(server)
            *output<<"OK"<<std::endl;
        else
            *output<<"Price for Castling"<<std::endl;
        std::getline(*input,response);
        Castling = std::stoi(response);
        if(server)
            *output<<"OK"<<std::endl;
        else
            *output<<"Price for Attack Cost"<<std::endl;
        std::getline(*input,response);
        ATTACK_COST = std::stof(response);
        if(server)
            *output<<"OK"<<std::endl;
        else
            *output<<"Price for Worth of predictions"<<std::endl;
        std::getline(*input,response);
        worth = std::stof(response);
        
    }
    catch(...)
    {
        std::cerr<<"INVALID VALUE";
    }
}
void IOhandler::move(const std::string& move)
{
    Move mv = {transcodePosition(move.substr(0,2)),transcodePosition(move.substr(3,5))};
    Move bestMove;
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
                if(ChessBoard::simplifiedEvaluateCheckMate(!this->side,ChessBoard::findKing(!this->side,ch->getBoard()),ch->getBoard()))
                    *output<<"YOU WON!!!"<<std::endl;
                else
                    *output<<"TIE!!!"<<std::endl;
                printBoard();
                if(ch)
                {
                    delete ch;
                    ch=nullptr;
                }
                checkMate={false,{},{}};
                gameIsOn=false;
                return;
            }
            ch->performMove(bestMove,ch->getBoard());
            id=patOrMate(side,ch->getBoard(),checkMate);
            if(id==-1)
            {
                *output<<"YOU LOST!!!"<<std::endl;
            }
            else if(id==0)
            {
                *output<<"TIE!!!"<<std::endl;
            }
            if(id!=1)
            {
                printBoard();
                if(ch)
                {
                    delete ch;
                    ch=nullptr;
                }
                checkMate={false,{},{}};
                gameIsOn=false;
                return;
            }

        }
        catch(...)
        {
            
            *output<<"ENEMY DECIDED TO SURRENDER -> YOU WON!!!"<<std::endl;
            printBoard();
            if(ch)
            {
                delete ch;
                ch=nullptr;
            }
            checkMate={false,{},{}};
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
    std::pair<int,int> pos = transcodePosition(start);
    std::vector<std::pair<int,int>> buf = ch&&ch->getBoard()[pos.first][pos.second]->isWhite()==side? getMoveCandidates(pos):std::vector<std::pair<int,int>>{};
    int counter=0;
    int i,j;
    if(buf.empty())
    {
        *output<<"NONE"<<std::endl;
    }
    else
    {
        for(std::pair<int,int> el : buf)
        {
            if(side==true)
            {
                j=el.first;
                i=el.second;
            }
            else
            {
                j=7-el.first;
                i=7-el.second;
            }
            ++counter;
            *output<<i<<j;
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
    out=ch->getBoard()[start.first][start.second]->getAttackCandidates(false);
    for(std::pair<int,int> el : ch->getBoard()[start.first][start.second]->getMoveCandidates())
    {
        out.push_back(el);
    }
    if(checkMate.kingAttacked&&ch->getBoard()[start.first][start.second]->getCode()!=KING)
    {
        out = ChessBoard::getOverlap(out,checkMate.saveKingPath);
    }
    if(id!=-1&&ch->getBoard()[start.first][start.second]->getCode()!=KING)
    {
        out = ChessBoard::getOverlap(out,checkMate.restrictions.at(id).unrestrictedPositions);
    }
    return out;
}
void IOhandler::printBoard()
{
    if(ch)
    {
        ch->printBoard(side,output,server);
    }
}
IOhandler::IOhandler()
:output(&std::cout),input(&std::cin)
{
    log = new Logger(true,nullptr);

}
IOhandler::~IOhandler()
{
}