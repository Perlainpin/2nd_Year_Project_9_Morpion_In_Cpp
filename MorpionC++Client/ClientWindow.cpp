#include "ClientWindow.h"

ClientWindow::ClientWindow() {
	windowW = 800;
	windowH = 600;
	oWindow = new sf::RenderWindow(sf::VideoMode(windowW, windowH), "Morpion", sf::Style::Default);
	if (windowH < windowW)
		gameSquareS = windowH;
	else
		gameSquareS = windowW;

	oCircle = new sf::CircleShape(gameSquareS / 6 - gameSquareS / 12, 12);
	oCrossBit1 = new sf::RectangleShape(sf::Vector2f(gameSquareS / 3, gameSquareS / 60));
	oCrossBit2 = new sf::RectangleShape(sf::Vector2f(gameSquareS / 3, gameSquareS / 60));

	oCircle->setOutlineThickness(gameSquareS / 60);
	oCircle->setOutlineColor(sf::Color(0, 128, 255));
	oCircle->setFillColor(sf::Color::Transparent);
	oCircle->setOrigin(sf::Vector2f(gameSquareS / 12, gameSquareS / 12));

	oCrossBit1->setFillColor(sf::Color(255, 0, 128));
	oCrossBit1->setOrigin(sf::Vector2f(gameSquareS / 6, gameSquareS / 120));
	oCrossBit2->setFillColor(sf::Color(255, 0, 128));
	oCrossBit2->setOrigin(sf::Vector2f(gameSquareS / 6, gameSquareS / 120));
	oCrossBit1->setRotation(45);
	oCrossBit2->setRotation(135);

	//lines of the grid
	for (int i = 0; i < 4; i++) {
		gridLines.push_back(new sf::RectangleShape(sf::Vector2f(gameSquareS, gameSquareS / 300)));
	}
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			gridLines[i * 2 + j]->setRotation(i * 90);
			gridLines[i * 2 + j]->setFillColor(sf::Color(0, 153, 0));
		}
	}
	gridLines[0]->setPosition((windowW - gameSquareS) / 2, (windowH - gameSquareS) / 2 + (gameSquareS / 3));
	gridLines[1]->setPosition((windowW - gameSquareS) / 2, (windowH - gameSquareS) / 2 + (gameSquareS / 3) * 2);
	gridLines[2]->setPosition((windowW - gameSquareS) / 2 + (gameSquareS / 3), (windowH - gameSquareS) / 2);
	gridLines[3]->setPosition((windowW - gameSquareS) / 2 + (gameSquareS / 3) * 2, (windowH - gameSquareS) / 2);


}

void ClientWindow::DrawCircle(int posX, int posY) {
	int X = (windowW - gameSquareS) / 2 + (gameSquareS / 3) * posX + gameSquareS / 6;
	int Y = (windowH - gameSquareS) / 2 + (gameSquareS / 3) * posY + gameSquareS / 6;
	oCircle->setPosition(sf::Vector2f(X, Y));
	oWindow->draw(*oCircle);
}

void ClientWindow::DrawCross(int posX, int posY) {
	int X = (windowW - gameSquareS) / 2 + (gameSquareS / 3) * posX + (gameSquareS / 6);
	int Y = (windowH - gameSquareS) / 2 + (gameSquareS / 3) * posY + (gameSquareS / 6);
	oCrossBit1->setPosition(sf::Vector2f(X, Y));
	oCrossBit2->setPosition(sf::Vector2f(X, Y));
	oWindow->draw(*oCrossBit1);
	oWindow->draw(*oCrossBit2);
}

void ClientWindow::Display(vector<char> currentGrid) {
	oWindow->clear();

	for (int i = 0; i < 4; i++) {
		oWindow->draw(*gridLines[i]);
	}

	for (int line = 0; line < 3; line++) {
		for (int column = 0; column < 3; column++) {
			if (currentGrid[line * 3 + column] == 'O') {
				DrawCircle(column, line);
			}
			else if (currentGrid[line * 3 + column] == 'X') {
				DrawCross(column, line);
			}
		}
	}

	//Updates the window.
	oWindow->display();
}

void ClientWindow::LoadingDisplay() {
	oWindow->clear();

	//temporarily changing the lines' color
	for (int i = 0; i < 4; i++) {
		gridLines[i]->setFillColor(sf::Color(153, 0, 153));

	}

	for (int i = 0; i < 4; i++) {
		oWindow->draw(*gridLines[i]);
	}

	for (int i = 0; i < 4; i++) {
		gridLines[i]->setFillColor(sf::Color(0, 153, 0));

	}

	//Updates the window.
	oWindow->display();
}
