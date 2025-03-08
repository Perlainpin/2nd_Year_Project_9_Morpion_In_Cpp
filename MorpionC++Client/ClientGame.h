#pragma once
#include "ClientWindow.h"
#include "ClientInputManager.h"

#include <iostream>

using namespace std;

class ClientGame
{
public:
	ClientWindow* window;
	ClientInputManager* inputs;
	vector<char> currentGrid;
	char currentPlayerTurn;
	bool playing;

	ClientGame();

	void Place();
	void End(char Winner);
	bool AskReplay();
	void Play();
};
