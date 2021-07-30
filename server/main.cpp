/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 11:59:24 by sad-aude          #+#    #+#             */
/*   Updated: 2021/07/30 17:33:29 by sad-aude         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebservData.hpp"
#include "../config/Parser.hpp"
#include "../config/Config.hpp"
#include <fstream>
#include <iostream>

std::vector<Config> configuration(int argc, char **argv) {
	std::string path;
	if (argc != 2)
	{
		std::cout << "In view of arguments, the default configuration is used." << std::endl;
		path = "config/default";
	}
	else
		path = argv[1];
	Parser	file(path);
	file.setContentFile();
	file.checkGeneralSyntax();
	file.setConfiguration();

	//file.printConfigs();

	return (file.getConfiguration());
}

void    serverLoop(WebservData &Data)
{
	int running = 1;

	while (running)
	{
		//std::cout << T_GYB "Waiting in passive mode" T_N << std::endl;
		Data.getReadCopy() = Data.getReadSet();
		Data.getWriteCopy() = Data.getWriteSet();
		if (select(FD_SETSIZE, &Data.getReadCopy(), &Data.getWriteCopy(), 0, 0) < 0)
			error("Select", Data);
		else
			for (int fd = 0; fd <= FD_SETSIZE; ++fd)
			{
				if ((FD_ISSET(fd, &Data.getReadCopy()) && (isTabMaster(Data.getTabMaster(), fd) == 1 || FD_ISSET(fd, &Data.getWriteCopy()))))
				{
					std::cout << T_GYB "Waiting in passive mode" T_N << std::endl;
					running = processSockets(fd, Data);
					break ;
				}
			}
	}
}

int     main(int ac, char *av[])
{
	std::vector<Config> setup;
	try {
		setup = configuration(ac, av);
	}
	catch (std::exception & err) {
		std::cout << err.what() << std::endl;
		return (1);
	}

	WebservData Data(setup);
	std::cout << std::endl;

	serverLoop(Data);
	closeAllFdUnlessMaster(Data.getReadSet(), Data.getWriteSet(), Data.getTabMaster()); // Destructor destroys the master
	destroyTabMaster(Data.getTabMaster());
	return (0);
}
