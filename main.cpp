#include"chess-board.h"
#include"logger.h"
#include"chess-peice.h"
#include <set>
#include <SFML/Graphics.hpp>
#define SCALE 200
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
void translateClick(sf::RenderWindow& wind,bool white,ChessBoard& ch, sf::RectangleShape** squares)
{
    int x,y,i,j;
    bool wh;
    std::set<std::pair<int,int>> dang;
    std::set<std::pair<int,int>> move;
    sf::Color clr;
    x = sf::Mouse::getPosition(wind).x/SCALE;
    y = sf::Mouse::getPosition(wind).y/SCALE;
    ch.cycleFigure({y,x},white,QUEEN);
    dang = ch.getDangerousPoints(ch.getBoard(),!white);
    move = getAllMoveCandidates(ch.getBoard());
    for(i=0;i<BOARDSIZE;++i)
    {
        for(j=0;j<BOARDSIZE;++j)
        {
            wh = ch.getBoard()[j][i]->isWhite();
            if(ch.getBoard()[j][i]->getCode() == EMPTY)
            {
                squares[i][j].setFillColor({25,25,25});
            }
            else
            {
                squares[i][j].setFillColor({128+72*wh,128+72*wh,128+72*wh});
            }
        }
    }
    for(std::pair<int,int> el : dang)
    {
        clr  = squares[el.second][el.first].getFillColor();
        clr.r=255;
        squares[el.second][el.first].setFillColor(clr);
    }
    for(std::pair<int,int> el : move)
    {
        clr  = squares[el.second][el.first].getFillColor();
        clr.g=255;
        squares[el.second][el.first].setFillColor(clr);
    }
    ch.printBoard();
    ch.debugPrintDanger();
}
int main()
{
    //setlocale(LC_ALL, "ALL");
    int i,j;
    Logger* log = new Logger(false,&std::cout);
    ChessBoard ch(log,5);
    //ch.clear();
    /*sf::RectangleShape** squares= new sf::RectangleShape*[BOARDSIZE];
    for(i=0;i<BOARDSIZE;++i)
    {
        squares[i] = new sf::RectangleShape[BOARDSIZE];
        for(j=0;j<BOARDSIZE;++j)
        {
            squares[i][j].setSize({SCALE,SCALE});
            squares[i][j].setPosition({i*SCALE,j*SCALE});
            squares[i][j].setFillColor({25,25,25});
        }
    }
    //ch.printBoard();
    //ch.debugPrintDanger();
    //getchar();
    sf::RenderWindow window(sf::VideoMode(8*SCALE, 8*SCALE), "SFML works!");
     while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if(event.type == sf::Event::MouseButtonPressed)
            {
                translateClick(window,sf::Mouse::isButtonPressed(sf::Mouse::Left),ch,squares);
            }
        }

        window.clear();
        for(i=0;i<BOARDSIZE;++i)
        {
            for(j=0;j<BOARDSIZE;++j)
            {
                window.draw(squares[i][j]);
            }
        }
        window.display();
    }*/
    
    ch.printBoard();
    std::cout<<std::endl;
    ChessBoard::performMove(ch.getBestMove(true),ch.getBoard());
    ch.printBoard();
    return 0;

}