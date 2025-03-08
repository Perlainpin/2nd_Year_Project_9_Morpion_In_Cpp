#pragma once
#include <vector>

class Bank
{
public:
	std::vector<char> bank;

	Bank();

	void AddToBank(int i, char signe);
	std::vector<char> GetBank();
};


