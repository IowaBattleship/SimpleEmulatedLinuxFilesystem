#include "shellio.h"

void shellInput(char cmd[], char arg1[],
	char arg2[])
{
	char allCmd[MAX_INPUT_LEN];
	std::stringstream cmdStream;
	std::cin.getline(allCmd, MAX_INPUT_LEN);
	cmdStream << allCmd;
	cmdStream >> cmd >> arg1 >> arg2;
}

void shellInput(char str[], uint len)
{
	std::cin.getline(str, len);
}

void shellOutput(const std::string str)
{
	std::cout << str;
}

bool shellInputYN()
{
	char buffer[MAX_COMMAND_LEN];
	while (1)
	{
		shellInput(buffer, MAX_COMMAND_LEN);
		if (!strcmp(buffer, "N") || !strcmp(buffer, "n"))
			return false;
		if (!strcmp(buffer, "Y") || !strcmp(buffer, "y"))
			return true;
		std::cout << "Please input Y/N: ";
	}
}