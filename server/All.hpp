#ifndef ALL_HPP
# define ALL_HPP

# include "../config/Config.hpp"
# include <string>
# include <vector>
# include <list>
# include <map>


class All
{
    private:
        std::vector<Config>							_setup;
		std::list<int>								_listPortsSocket;
		std::map<std::string, Config>				_mapServerName;


    public:
        All();
        All(std::vector<Config> setup, std::list<int> listPortsSocket, std::map<std::string, Config> mapServerName);
        ~All();

        std::vector<Config>							getConfiguration(void) const;
		std::list<int>								getListPorts(void) const ;
		std::map<std::string, Config>				getMapServerName(void) const ;
};

#endif