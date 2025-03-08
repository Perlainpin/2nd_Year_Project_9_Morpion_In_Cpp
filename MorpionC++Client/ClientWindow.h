#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;

class ClientWindow
{
public:
    sf::RenderWindow* oWindow;
    sf::VideoMode oVideoMode;
    sf::Vector2i localPosition;
    int windowW;
    int windowH;
    int gameSquareS;

    sf::CircleShape* oCircle;
    sf::RectangleShape* oCrossBit1;
    sf::RectangleShape* oCrossBit2;
    vector <sf::RectangleShape*> gridLines;


    ClientWindow();

    void DrawCircle(int posX, int posY);
    void DrawCross(int posX, int posY);
    void Display(vector<char> currentGrid);
    void LoadingDisplay();
};