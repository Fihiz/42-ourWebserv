#ifndef ALL_HPP
# define ALL_HPP

# include "../config/Config.hpp"
# include <string>
# include <vector>
# include <list>
# include <map>
# include <sstream>


class All
{
    private:
        std::vector<Config>							_setup;
		std::list<int>								_listPortsSocket;
		std::map<std::string, Config>				_mapServerName;
        //std::list<std::string>						_listHostNames;

    public:
        All();
        All(std::vector<Config> setup);
        ~All();

        std::vector<Config>							getConfiguration(void) const;
		std::list<int>								getListPorts(void) const ;
		std::map<std::string, Config>				getMapServerName(void) const ;

        void                                        setPorts(void);
        void                                        setHosts(void);
};

#endif