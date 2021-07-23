
#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <fstream>
# include <istream>
# include <exception>
# include <vector>
# include <map>

typedef struct s_location {
	std::vector<std::string>				method;
	std::string 							root;
	std::vector<std::string>				index;
	std::map<std::string, std::string>		cgi;
	int			 							autoindex;
} t_location;

class   Config
{
	private:
    	int									_listen;
		std::string							_serverName;
		std::string							_root;
		std::vector<std::string>			_index;
		unsigned long						_maxBodySizeClient;
		std::map<int, std::string>			_errorPage;
		std::map<std::string, t_location>	_location;

	public:
		Config();
		~Config();

		Config & operator=(const Config & rhs);

		void								setListen(int portNumber);
		void								setServerName(std::string hostName);
		void								setMaxBodyClient(unsigned long bodySize);
		void								setErrorPage(int codeError, std::string urlError);
		void								setRoutes(std::string routes);
		void								setMethod(std::string routes, std::string valMethod);
		void								setRoot(std::string routes, std::string dirName);
		void								setIndex(std::string routes, std::string dirName);
		void								setCgi(std::string routes, std::string langageType, std::string executableCgi);
		void								setAutoindex(std::string routes, int valAutoindex);
		void								setDefaultValue(void);

		int									getListen(void) const;
		std::string							getServerName(void) const;
		std::string							getRoot(std::string routes) const;
		std::vector<std::string>			getIndex(std::string routes) const;
		unsigned long						getMaxBodySize(void) const;
		std::map<int, std::string>			getErrorPage(void) const;
		const	t_location *				getLocation(std::string routes) const;
		std::string							getCgi(std::string routes) const;

		void								reset(void);

		void								printListen(void) const;
		void								printServerName(void) const;
		void								printServerRoot(void) const;
		void								printServerIndex(void) const;
		void								printMaxBodyClient(void) const;
		void								printErrorPage(void) const;
		void								printLocation(void) const;
};

#endif
