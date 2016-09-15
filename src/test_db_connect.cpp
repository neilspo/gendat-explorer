//
// This program conducts a simple test to ensure that a connection to the database can be successfully opened.
//


#include <string>
#include <iostream>
#include <stdexcept>
#include "database.h"

int main(int argc, char* argv[])
{
	if (argc!=5)
	{
		std::cerr << "Usage: " << argv[0] << " Host, User, Password, Database" << std::endl;
		return 1;
	}

	std::string host    = argv[1];
	std::string user    = argv[2];
	std::string passwd  = argv[3];
	std::string db_name = argv[4];
	database    db;

	try
	{
		// Connect to the database.

		db.connect(host, user, passwd, db_name);

		// Change the window status message.

		std::cout << "Successfully connected to database" << std::endl;
	}
	catch (std::runtime_error& exception)
	{
		std::cout << exception.what() << std::endl;
	}
}
