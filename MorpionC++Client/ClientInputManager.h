#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <json/json.h>

class ClientWindow;

class ClientInputManager
{
public:
	ClientWindow* Window;
	sf::Event oEvent;
	int mouseX;
	int mouseY;

	bool readyToSend;
	std::string currentMessage;

	ClientInputManager(ClientWindow* window);

	void EmptyMessage();
	bool EventCheck(std::string username);
};

