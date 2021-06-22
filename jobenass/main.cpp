#include "Parser.hpp"
#include "Config.hpp"
#include <iostream>

std::vector<Config> configuration(const std::string & path) {
	Parser	file(path);
	file.setContent();
	file.checkSyntax();
	file.setConfiguration();
	return (file.getConfiguration());
}

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cout << "Error: At least one argument is needed." << std::endl;
		return (1);
	}
	std::vector<Config> setup;
	try {
		setup = configuration(argv[1]);
	}
	catch (std::exception & err) {
		std::cout << err.what() << std::endl;
		return (1);
	}

	// DISPLAY SERVER
	int i = 1;
	for (std::vector<Config>::const_iterator it = setup.begin(); it != setup.end(); it++)
	{
		std::cout << "-- [ " << "SETUP " << i++ << " ]" << std::endl;
		it->printListen();
		it->printServerName();
		it->printMaxBodyClient();
		it->printErrorPage();
		it->printLocation();
		std::cout << std::endl;
	}
	// ^

	return (0);
}
