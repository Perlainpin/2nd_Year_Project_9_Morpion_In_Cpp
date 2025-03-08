#include "ClientGame.h"

ClientGame::ClientGame() {
	window = new ClientWindow();
	inputs = new ClientInputManager(window);
	currentPlayerTurn = 'O';
	playing = true;
	for (int i = 0; i < 9; i++) {
		currentGrid.push_back(' ');
	}
}