#include"chess-board.h"
#include"logger.h"
#include"chess-peice.h"
int main()
{
    //setlocale(LC_ALL, "ALL");
    Logger* log = new Logger(false,&std::cout);
    ChessBoard ch(log);
}