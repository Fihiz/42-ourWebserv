#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <fstream>
# include <istream>
# include <exception>
# include <vector>
# include <map>

typedef struct s_location {
	std::vector<std::string>	method;
	std::string 				root;
	std::vector<std::string>	index;
	int			 				autoindex;
} t_location;

class   Config
{
	private:
    	int											_listen;
		std::string									_serverName;
		unsigned long								_maxBodySizeClient;
		std::map<int, std::string>					_errorPage;
		std::map<std::string, t_location>			_location;
		std::string									_routes;
		std::string									_defaultRoot;

	public:
		Config();
		~Config();

		Config & operator=(const Config & rhs);

		void										setListen(std::vector<std::string> input, size_t line);
		void										setServerName(std::vector<std::string> input, size_t line);
		void										setMaxBodyClient(std::vector<std::string> input, size_t line);
		void										setErrorPage(std::vector<std::string> input, size_t line);
		void										setRoutes(std::vector<std::string> input, size_t line);
		void										setMethod(std::vector<std::string> input, size_t line);
		void										setRoot(std::vector<std::string> input, size_t line);
		void										setIndex(std::vector<std::string> input, size_t line);
		void										setAutoindex(std::vector<std::string> input, size_t line);
		void										setDefault(void);

		int											getListen(void) const;
		std::string									getServerName(void) const;
		unsigned long								getMaxBodySize(void) const;
		std::map<int, std::string>					getErrorPage(void) const;
		std::string									getSpecificErrorPage(int ask) const;
		std::map<std::string, t_location>			getLocation(void) const;
		const t_location *							getSpecificLocation(std::string ask) const;
		std::string									getDefautlRoot(void) const;

		void										reset(void);
		void										printListen(void) const;
		void										printServerName(void) const;
		void										printMaxBodyClient(void) const;
		void										printErrorPage(void) const;
		void										printLocation(void) const;
		
		class WrongDataValueDeclarationException : public std::exception {
			virtual const char * 	what() const throw();
		};
		class WrongPathValueDeclarationException : public std::exception {
			virtual const char * 	what() const throw();
		};
};

#endif
