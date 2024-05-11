#include"chess-board.h"
#include"logger.h"
#include"chess-peice.h"
#include <set>
#include <SFML/Graphics.hpp>
void translateClick(sf::RenderWindow& wind,ChessBoard& ch, sf::RectangleShape** squares)
{
    int x,y,i,j;
    static bool white = 1;
    x = sf::Mouse::getPosition(wind).x/200;
    y = sf::Mouse::getPosition(wind).y/200;
    ch.cycleFigure({y,x},white,PAWN);
    std::set<std::pair<int,int>> dang = ch.getDangerousPoints(ch.getBoard(),!white);
    for(i=0;i<BOARDSIZE;++i)
    {
        for(j=0;j<BOARDSIZE;++j)
        {
            if(ch.getBoard()[j][i]->getCode() == EMPTY)
            {
                squares[i][j].setFillColor({25,25,25});
            }
            else
            {
                squares[i][j].setFillColor({255,255,255});
            }
        }
    }
    for(std::pair<int,int> el : dang)
    {
        squares[el.second][el.first].setFillColor(sf::Color::Red);
    }
    ch.printBoard();
    ch.debugPrintDanger();
}
int main()
{
    //setlocale(LC_ALL, "ALL");
    int i,j;
    Logger* log = new Logger(false,&std::cout);
    ChessBoard ch(log);
    ch.clear();
    sf::RectangleShape** squares= new sf::RectangleShape*[BOARDSIZE];
    for(i=0;i<BOARDSIZE;++i)
    {
        squares[i] = new sf::RectangleShape[BOARDSIZE];
        for(j=0;j<BOARDSIZE;++j)
        {
            squares[i][j].setSize({200,200});
            squares[i][j].setPosition({i*200,j*200});
            squares[i][j].setFillColor({25,25,25});
        }
    }
    //ch.printBoard();
    //ch.debugPrintDanger();
    //getchar();
    sf::RenderWindow window(sf::VideoMode(1600, 1600), "SFML works!");
     while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if(event.type == sf::Event::MouseButtonPressed)
            {
                translateClick(window,ch,squares);
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
    }
    return 0;

}