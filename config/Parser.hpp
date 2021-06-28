#ifndef PARSER_HPP
# define PARSER_HPP

# include <fstream>
# include <istream>
# include <iostream>
# include <exception>
# include <vector>
# include <map>
# include "./Config.hpp"

class   Parser
{
	private:
		std::fstream								_file;
		std::map< int, std::vector<std::string> > 	_content;
		std::vector<std::string>					_tokens;
		std::vector<std::string>					_context;
		std::vector<Config>							_setup;

		Parser();

	public:
		Parser(const std::string & path);
		~Parser();

		void										setContent(void);
		void										setStartContext(void);
		void										setEndContext(void);
		void										setConfiguration(void);

		std::string									getContext(void) const;
		std::string									getDirective(void) const;
		std::vector<Config>							getConfiguration(void) const;
	
		void										checkSyntax(void);
		void										CheckOpenContext(size_t line);
		void										CheckCloseContext(size_t line);
		void										CheckDirective(size_t line);
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
