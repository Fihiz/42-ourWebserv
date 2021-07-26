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

#include <stdio.h>
#include <stdlib.h>

class   Parser
{
	private:
		std::fstream								_file;
		std::map< int, std::vector<std::string> > 	_content;
		size_t									 	_line;
		std::vector<std::string>					_tokens;
		std::vector<std::string>					_context;
		std::string									_routes;
		std::vector<Config>							_setup;

		Parser();

	public:
		Parser(std::string const & path);
		~Parser();

		void										setContentFile(void);
		void										setStartContext(void);
		void										setEndContext(void);
		void										setConfiguration(void);

		std::string									getContext(void) const;
		std::string									getDirective(void) const;
		std::vector<Config>							getConfiguration(void) const;

		void										checkGeneralSyntax(void);
		void										checkOpenContext();
		void										checkCloseContext();
		void										checkDirective();
		void										checkListen(Config & virtualHost);
		void										checkServerName(Config & virtualHost);
		void										checkMaxBodyClient(Config & virtualHost);
		void										checkErrorPage(Config & virtualHost);

		void										checkMethod(Config & virtualHost);
		void										checkRoot(Config & virtualHost);
		void										checkIndex(Config & virtualHost);
		void										checkRoutes(Config & virtualHost);
		void										checkCgi(Config & virtualHost);
		void										checkReturn(Config & virtualHost);
		void										checkAutoindex(Config & virtualHost);
		
		void										reset(void);
	
		void										printContent(void);
		void										printConfigs(void);

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
		class WrongDataValueDeclarationException : public std::exception {
			virtual const char * 	what() const throw();
		};
		class WrongPathValueDeclarationException : public std::exception {
			virtual const char * 	what() const throw();
		};
};

#endif
