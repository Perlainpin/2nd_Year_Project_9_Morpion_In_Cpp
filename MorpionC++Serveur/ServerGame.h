#pragma once

#include <iostream>
#include <json/json.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "ServerGrid.h"

using namespace std;

class ServerGame
{
public:
	//server values
	Json::Value playerList;
	vector<SOCKET> socketList;
	Json::Value actionList;

	//game values
	ServerGrid* currentGrid;
	char currentTurnSymbol;
	
	bool playing;

	ServerGame();
	
	//server functions
	bool CheckIfPlayer(string playerName);
	Json::Value GetPlayerData(Json::String playerName); 
	Json::Value GetPlayerDataFromSymbol(char symbol);
	void NewPlayer(string playerName);
	void PlayerSocketUpdate(string playerName);

	//game functions
	void ChangeTurn();
	bool Place(int boxIndex);
	void End(char Winner, vector<SOCKET> client_fd);
	void Reset();
};

