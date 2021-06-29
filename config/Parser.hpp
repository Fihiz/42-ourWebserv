#ifndef PARSER_HPP
# define PARSER_HPP

# include <fstream>
# include <istream>
# include <iostream>
# include <sstream>
# include <exception>
# include <vector>
# include <map>
# include <list>
# include "./Config.hpp"
# include "../server/Webserv.hpp"

#include <stdio.h>
#include <stdlib.h>

class   Parser
{
	private:
		std::fstream								_file;
		std::map< int, std::vector<std::string> > 	_content;
		std::vector<std::string>					_tokens;
		std::vector<std::string>					_context;

		std::vector<Config>							_setup;
		std::list<int>								_listPortsSocket;
		std::map<std::string, Config>				_mapServerName;

		Parser();

	public:
		Parser(const std::string & path);
		~Parser();

		void										setContent(void);
		void										setStartContext(void);
		void										setEndContext(void);
		void										setConfiguration(void);

		void										setPorts(void);
		void										setHosts(void);

		std::string									getContext(void) const;
		std::string									getDirective(void) const;
		std::vector<Config>							getConfiguration(void) const;
		std::list<int>								getListPorts(void) const ;
		std::map<std::string, Config>				getMapServerName(void) const ;
	
		void										checkSyntax(void);
		void										checkOpenContext(size_t line);
		void										checkCloseContext(size_t line);
		void										checkDirective(size_t line);
		void										reset(void);

		class FileOpeningFailedException : public std::exception {
			virtual const char * 	what() const throw();
		};
		class FileClosingFailedException : public std::exception {
			virtual const char * 	what() const throw();
		};
		class WrongServerDeclarationException : public std::exception {
			virtual const char * 	what() const throw();
		};
		class WrongLocationDeclarationException : public std::exception {
			virtual const char * 	what() const throw();
		};
		class UnexpectedContextDeclarationException : public std::exception {
			virtual const char * 	what() const throw();
		};
		class WrongDirectiveDeclarationException : public std::exception {
			virtual const char * 	what() const throw();
		};
};

#endif
