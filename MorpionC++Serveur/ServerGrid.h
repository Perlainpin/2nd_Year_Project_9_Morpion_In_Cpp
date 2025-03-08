#pragma once

#include <vector>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <json/json.h>

#include "ServerGridBox.h"
#include "..\MorpionC++Webserver\Bank.h"

using namespace std;

class ServerGrid
{
public:
	

	vector<ServerGridBox*> boxList;

	ServerGrid();

	void Display();
	bool IsFull();
	char WinCheck();
	void SendGrid(vector<SOCKET> client_fd);
};

