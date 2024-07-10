#include"chess-board.h"
#include"logger.h"
#include"chess-peice.h"
#include"IOhandler.h"
#include <set>
std::set<std::pair<int,int>> getAllMoveCandidates(ChessPieceBase*** board)
{
    std::set<std::pair<int,int>> out;
    std::vector<std::pair<int,int>> buf;
    int i,j;
    for(i=0;i<BOARDSIZE;++i)
    {
        for(j=0;j<BOARDSIZE;++j)
        {
            buf = board[i][j]->getMoveCandidates();
            for(const std::pair<int,int>& el : buf)
            {
                out.insert(el);
            }
        }
    }
    return out;
}
int main()
{

    IOhandler handler(&std::cout,&std::cin);
    handler.mainLoop();
   /* int i,j;
    Logger* log = new Logger(false,&std::cout);
    ChessBoard ch(log,5);
    ch.printBoard();
    std::cout<<std::endl;
    ChessBoard::performMove(ch.getBestMove(true),ch.getBoard());
    ch.printBoard();
    return 0;
    */
}