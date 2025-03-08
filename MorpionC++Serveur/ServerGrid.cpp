#include "ServerGrid.h"

//creates a new grid composed of 9 empty boxes
ServerGrid::ServerGrid() {
	Bank bank = Bank();
	for (int i = 0; i < 9; i++)
	{
		boxList.push_back(new ServerGridBox());
	}
}

//shows the current state of the grid in the console
void ServerGrid::Display() {
	for (int line = 0; line < 3; line++) {
		for (int column = 0; column < 3; column++) {
			
		}
	}
}

void ServerGrid::SendGrid(vector<SOCKET> client_fd){
	Json::Value grid;
	string socketString;
	for (int line = 0; line < 3; line++) {
		for (int column = 0; column < 3; column++) {
			socketString.push_back(boxList[line * 3 + column]->GetValue());
			//bank.AddToBank(line * 3 + column, boxList[line * 3 + column]->GetValue());
		}
	}
	
	//preparing the json for departure
	grid["requestType"] = "sendGrid";
	grid["grid"] = socketString;

	//making the Json into a string.
	Json::FastWriter fastWriter;
	std::string output = fastWriter.write(grid);
	const char* tmp = output.c_str();

	//sending the message to the client.
	for (int i = 0; i < client_fd.size(); i++) {
		int sendResult = send(client_fd[i], tmp, strlen(tmp), 0);
		if (sendResult != strlen(tmp)) {
			MessageBox(NULL, L"Send Failed", L"Error", MB_OK | MB_ICONERROR);
		}
	}
}

//Checks if the grid is full
bool ServerGrid::IsFull() {
	for (int i = 0; i < 9; i++) {
		if (boxList[i]->GetValue() == ' ')
			return false;
	}
	return true;
}

//Returns the character of the winner, if either player wins.
char ServerGrid::WinCheck() {
	//checking the lines
	for (int i = 0; i < 3; i++) {
		if (((boxList[i * 3]->GetValue() == boxList[i * 3 + 1]->GetValue()) && (boxList[i * 3 + 1]->GetValue() == boxList[i * 3 + 2]->GetValue())) && (boxList[i * 3]->GetValue() == 'O' || boxList[i * 3]->GetValue() == 'X'))
			return boxList[i * 3]->GetValue();
	}

	//checking the columns
	for (int i = 0; i < 3; i++) {
		if (((boxList[i]->GetValue() == boxList[i + 3]->GetValue()) && (boxList[i + 3]->GetValue() == boxList[i + 6]->GetValue())) && (boxList[i]->GetValue() == 'O' || boxList[i]->GetValue() == 'X'))
			return boxList[i]->GetValue();
	}

	//checking the diagonals
	if ((((boxList[0]->GetValue() == boxList[4]->GetValue()) && (boxList[4]->GetValue() == boxList[8]->GetValue())) || ((boxList[2]->GetValue() == boxList[4]->GetValue()) && (boxList[4]->GetValue() == boxList[6]->GetValue()))) && (boxList[4]->GetValue() == 'O' || boxList[4]->GetValue() == 'X'))
		return boxList[4]->GetValue(); //if someone wins via a diagonal, the winner will always have their symbol in the middle of the grid.

	//if this point is reached, no win condition was met.
	return ' ';
}