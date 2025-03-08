#include "Bank.h"

Bank::Bank() {

	bank = { 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O' };

}

void Bank::AddToBank(int i, char signe) {
	if (signe == 'O' || signe == 'X') {
		bank[i] = signe;
	}
}

std::vector<char> Bank::GetBank() {

	return bank;

}