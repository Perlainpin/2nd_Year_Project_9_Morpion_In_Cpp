#include "ServerGridBox.h"

ServerGridBox::ServerGridBox() {
	value = ' ';
};

//returns the value of a box
char ServerGridBox::GetValue() {
	return value;
};

//changes the value of the box
void ServerGridBox::SetValue(char newValue) {
	value = newValue;
};