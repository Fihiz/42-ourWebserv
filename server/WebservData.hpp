#ifndef WEBSERVDATA_HPP
# define WEBSERVDATA_HPP

# include "../config/Config.hpp"
# include <string>
# include <vector>
# include <list>
# include <map>
# include <sstream>


class WebservData
{
    private:
        std::vector<Config>							_setup;
		std::list<int>								_listPortsSocket;
		std::map<std::string, Config>				_mapServerName;
        //std::list<std::string>						_listHostNames;

    public:
        WebservData();
        WebservData(std::vector<Config> setup);
        ~WebservData();

        const std::vector<Config>	&						getConfiguration(void) const;
		std::list<int>								getListPorts(void) const ;
		std::map<std::string, Config> &				getMapServerName(void) ;

        void                                        setPorts(void);
        void                                        setHosts(void);
};

#endif