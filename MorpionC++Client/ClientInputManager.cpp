#include "ClientInputManager.h"
#include "ClientWindow.h"

#include <iostream>
#include <SFML/Graphics.hpp>

ClientInputManager::ClientInputManager(ClientWindow* window) {
	Window = window;
	currentMessage = "";
}

void ClientInputManager::EmptyMessage() {
	currentMessage = "";
}

bool ClientInputManager::EventCheck(std::string username) {
	Json::Value board;

	while (Window->oWindow->pollEvent(oEvent))
	{
		if (oEvent.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			Window->oWindow->close();
			return false;

		}
		else if (oEvent.type == sf::Event::MouseButtonReleased) {

			//if the click is in the gameSquareS
			if (oEvent.mouseButton.button == sf::Mouse::Left &&
				((oEvent.mouseButton.x > (Window->windowW - Window->gameSquareS) / 2) &&
				(oEvent.mouseButton.x < Window->gameSquareS + (Window->windowW - Window->gameSquareS) / 2) &&
				(oEvent.mouseButton.y > (Window->windowH - Window->gameSquareS) / 2) &&
				(oEvent.mouseButton.y < Window->gameSquareS + (Window->windowH - Window->gameSquareS) / 2))) {


				mouseX = (oEvent.mouseButton.x - (Window->windowW - Window->gameSquareS) / 2) / (Window->gameSquareS / 3);
				mouseY = (oEvent.mouseButton.y - (Window->windowH - Window->gameSquareS) / 2) / (Window->gameSquareS / 3) * 3;

				board["requestType"] = "Place";
				board["playerName"] = username;
				board["placeIndex"] = (mouseX + mouseY);

				//making the Json into a string.
				Json::FastWriter fastWriter;
				std::string output = fastWriter.write(board);
				currentMessage = output.c_str();

				readyToSend = true;

				return true;
			}
		}
		mouseX, mouseY = NULL;
		return false;
	}
}
