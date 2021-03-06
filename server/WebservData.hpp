#ifndef WEBSERVDATA_HPP
# define WEBSERVDATA_HPP

# include "../config/Config.hpp"
# include <string>
# include <vector>
# include <list>
# include <map>
# include <sstream>
# include "Webserv.hpp"

class WebservData
{
    private:
        std::vector<Config>							_setup;
		std::list<int>								_listPortsSocket;
		std::map<std::string, Config>				_mapServerName;
        std::vector<Socket *>                       _tabMaster;
        fd_set                                      _readSet;
        fd_set                                      _readCopy;
        fd_set                                      _writeSet;
        fd_set                                      _writeCopy;
        void                                        _initPorts(void);
        void                                        _initHosts(void);
        void                                        _initSocket(void);

    public:
        WebservData();
        WebservData(std::vector<Config> setup);
        ~WebservData();

        const std::vector<Config>	&				getConfiguration(void) const;
		std::list<int>								getListPorts(void) const ;
		std::map<std::string, Config> &				getMapServerName(void) ;
        fd_set &                                    getReadSet(void);
        fd_set &                                    getReadCopy(void);
        fd_set &                                    getWriteSet(void);
        fd_set &                                    getWriteCopy(void);
        std::vector<Socket *> &                     getTabMaster();
};

#endif
