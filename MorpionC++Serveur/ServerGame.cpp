#include "ServerGame.h"

ServerGame::ServerGame() {
	//server data
	playerList;
	socketList;
	actionList;

	//game data
	currentGrid = new ServerGrid();
	currentTurnSymbol = 'O';
	playing = true;

}

//////////////////////////////
// 
//Server-specific functions
//
//////////////////////////////

bool ServerGame::CheckIfPlayer(string playerName) {
	/*Checks if there is any player with this name in playerList*/

	for (int i = 0; i < (int)playerList.size(); i++)
	{
		if (playerList[i]["username"].toStyledString() == playerName) {
			return true;
		}
	}
	return false;
}

Json::Value ServerGame::GetPlayerData(Json::String playerName) {
	/*Checks if there is anGetPlayerDatay player with this name in playerList*/

	for (int i = 0; i < playerList.size(); i++)
	{
		if (playerList[i]["username"] == playerName) {
			return playerList[i];
		}
	}
	return false;
}


Json::Value ServerGame::GetPlayerDataFromSymbol(char symbol) {
	/*Checks if there is anGetPlayerDatay player with this turnSymbol in playerList*/

	for (int i = 0; i < playerList.size(); i++)
	{
		if (static_cast<char>(playerList[i]["turnSymbol"].asInt()) == symbol) {
			return playerList[i];
		}
	}
	return false;
}


void ServerGame::NewPlayer(string playerName) {
	/*Adds a new player in the playerList json*/

	//we have to make a variable of the current size here or it'll change once we add the first chunk of data.
	int newPlrID = playerList.size();


	//determining the player's ID lets us decide if they're playing or spectating in this server
	playerList[newPlrID]["playerSessionID"] = newPlrID;
	//setting their name
	playerList[newPlrID]["username"] = playerName;
	//initializing the score
	playerList[newPlrID]["score"] = 0;
	//the login is done instantly after the accept, so we can afford to pull directly the last piece of data in the socketList
	playerList[newPlrID]["clientSocket"] = socketList[socketList.size() - 1];
	//determining the player's symbol from their ID for the session. First two players to join get to play.
	switch (newPlrID){
	case 0:
		playerList[newPlrID]["turnSymbol"] = 'O';
		break;
	case 1:
		playerList[newPlrID]["turnSymbol"] = 'X';
		break;
	default:
		playerList[newPlrID]["turnSymbol"] = ' ';
	}

	/* Player JSON template :
	{
		"playerSessionID" : (int)
		"username" : playerName (string)
		"score" : 0 (int)
		"turnSymbol" : (char)
		"clientSocket" : (SOCKET*)
	}
	*/
}

void ServerGame::PlayerSocketUpdate(string playerName) {
	/*Updates a returning player's socket info in the playerList json*/

	//finding the data of the player who came back.
	int newPlrID = GetPlayerData(playerName)["playerSessionID"].asInt();

	//updating the socket number to match their new one
	playerList[newPlrID]["clientSocket"] = socketList[socketList.size() - 1];
}


//////////////////////////////
//
//Functions that were in the offline version of the game
//
//////////////////////////////

void ServerGame::ChangeTurn() {
	if (currentTurnSymbol == 'O') {
		currentTurnSymbol = 'X';
	}
	else if (currentTurnSymbol == 'X') {
		currentTurnSymbol = 'O';
	}
}

//should take parameters acquired from a player's request: the box's index
bool ServerGame::Place(int boxIndex) {
	//checks if the spot is occupied
	//if it isnt, add the current player's symbol.
	
	if (boxIndex < 0 || boxIndex > 8) {
		return false;
	}
	else if (currentGrid->boxList[boxIndex]->GetValue() != ' ') {
		return false;
	}
	else {
		currentGrid->boxList[boxIndex]->SetValue(currentTurnSymbol);

		//a return true will send a request to update the players' view outside of this function.
		return true;
	}

}

void ServerGame::End(char Winner, vector<SOCKET> client_fd) {
	/*this function should send a json, telling the players how the game ended, ie :
	{
		requestType: "end" (string)
		gameWinner: "draw/playername1/playername2" (string)
		score: (int)
	}
	the clientside will figure out how to display adequate text on its own
	*/

	Json::Value gameResult;
	Json::Value winnerData;
	gameResult["requestType"] = "end";

	if (Winner == ' ') {
		//return "draw", this will simply just end the game without changing any scores
		gameResult["winner"] = "draw";
	}
	else {
		//return the data of the winning player

		//get the winner's data
		winnerData = GetPlayerDataFromSymbol(Winner);
		//return the winner's username
		gameResult["winner"] = winnerData["username"].asString();

		//change the winner's score
		//retrieve the current one
		winnerData["score"] = winnerData["score"].asInt() + 1;
		//add 1
		playerList[winnerData["playerSessionID"].asInt()] = winnerData;
	}

	//sending the message to the client.
	for (int i = 0; i < client_fd.size(); i++) {
		//retrieve player's score and put it in the Json
		gameResult["score"] = playerList[i]["score"];

		//making the Json into a string.
		Json::FastWriter fastWriter;
		std::string output = fastWriter.write(gameResult);
		const char* tmp = output.c_str();
		int sendResult = send(client_fd[i], tmp, strlen(tmp), 0);
		if (sendResult != strlen(tmp)) {
			MessageBox(NULL, L"Send Failed", L"Error", MB_OK | MB_ICONERROR);
		}
	}
}

void ServerGame::Reset() {
	delete currentGrid;
	currentGrid = new ServerGrid();
}