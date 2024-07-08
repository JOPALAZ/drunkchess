#pragma once
#include"logger.h"
#include"chess-board.h"
#include<algorithm>
#include<fstream>
#include<vector>
class IOhandler
{
private:
    Logger* log=nullptr;
    ChessBoard* ch=nullptr;
    std::ostream* output;
    std::istream* input;
    bool gameIsOn=false;
    bool loop = true;
    bool server = false;
    bool side;
    bool startGame();
    bool startPreDefinedGame();
    void move(const std::string& move);
    std::vector<std::string> getPossibleOptions();
    void processInput(const std::string& response);
public:
    static void toLowercase(std::string& str);
    void mainLoop();
    IOhandler(std::ostream* output,std::istream* input);
    IOhandler();

    ~IOhandler();
};
