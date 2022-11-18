#include "command.h"

bool login()
{
	shellOutput("\n----LOGIN----\n");

	char userName[MAX_USER_NAME_LEN];
	char userPassword[MAX_USER_PASSWORD_LEN];
	while (1)
	{
		shellOutput("\nPlease input username: ");
		shellInput(userName, MAX_USER_NAME_LEN);
		shellOutput("Please input password: ");
		shellInput(userPassword, MAX_USER_PASSWORD_LEN);

		bool ifMatch = false;
		for (const auto& user : users)
			if (!strcmp(user.userName, userName) &&
				!strcmp(user.userPassword, userPassword))
			{
				currentUser = user;
				ifMatch = true;
				break;
			}
		if (ifMatch)
			break;
		shellOutput("Incorrect username or password\n");
		shellOutput("Whether to retry: (Y/N) ");
		if (!shellInputYN())
		{
			shellOutput("Exit system\n");
			return false;
		}
	}

	shellOutput("Login successful\n");
	return true;
}