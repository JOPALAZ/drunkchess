#include"IOhandler.h"

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
    std::string response;
    while(loop)
    {
        try
        {
            if(!server)
                *output <<std::endl<<"~ (help for help): "<<std::flush;
            std::getline(*input, response);
            toLowercase(response);
            processInput(response);
        }
        catch(std::out_of_range& range)
        {
            if(!server)
                *output<<"YOUR COMMAND '"<<response<<"' WAS GIVEN WRONG: "<<range.what()<<std::flush;
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
            if(!server)
                *output<<el<<std::endl;
        }
    }
    else if(response=="exit")
    {
        loop=false;
    }
    else if(!gameIsOn&&response=="start")
    {
        if(!server) 
            *output<<"Chose a side [w/b]"<<std::flush;
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
    else if(gameIsOn&&response.substr(0,4)=="move")
    {
        move(response.substr(5,10));
    }
    else if(gameIsOn&&response=="surrender")
    {
        if(ch)
        {
            delete ch;
            ch=nullptr;
        }
        *output<<"You lost!!! "<<std::flush;
        gameIsOn=false;
    }
    else if(gameIsOn&&response=="print")
    {
        if(ch)
        {
            ch->printBoard(output);
        }
    }
    else
    {
        *output<<"Unknown input "<<std::flush;
    }
}

bool IOhandler::startGame()
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
        if(!this->side)
        {
            ch->performMove(ch->getBestMove(!this->side),ch->getBoard());
        }
        ch->printBoard(output);
    }
    return ch!=nullptr;
}
void IOhandler::move(const std::string& move)
{
    Move mv = {{move[1]-48,move[0]-48},{move[4]-48,move[3]-48}};
    Move bestMove;
    if (ch->getBoard()[mv.start.first][mv.start.second]->isWhite()==this->side&&ch->getBoard()[mv.start.first][mv.start.second]->getCode()!=EMPTY)
    {
        try
        {
            ch->performMove(mv,ch->getBoard());
        }
        catch(std::exception& ex)
        {
            *output<<ex.what()<<std::endl;
            return;
        }
        try
        {
            bestMove = ch->getBestMove(!this->side);
            if(bestMove.start.first==-1)
            {
                *output<<"YOU WON!!!"<<std::endl;
                if(ch)
                {
                    delete ch;
                    ch=nullptr;
                }
                gameIsOn=false;
                return;
            }
            ch->performMove(bestMove,ch->getBoard());
            ch->printBoard(output);
        }
        catch(std::exception& ex)
        {
            *output<<"ENEMY: "<<ex.what()<<std::endl;
            *output<<"YOU WON!!!"<<std::endl;
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
        *output<<"YOU CAN'T MOVE THAT FIGURE"<<std::flush;
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
// todo backtrack king being attacked and based on that get what cells cannot be moved and where they cant go
// nuzno peredavat v recursive i v obicny specialny parameter bana hotby dlya figur i sostojanije shaha i scitat kajdy raz kogda idem dalse v recurcive.