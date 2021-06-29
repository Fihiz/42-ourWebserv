#include "./All.hpp"

All::All()
{
}

All::All(std::vector<Config> setup, std::list<int> listPortsSocket, std::map<std::string, Config> mapServerName, std::list<std::string> listHostNames)
: _setup(setup), _listPortsSocket(listPortsSocket), _mapServerName(mapServerName), _listHostNames(listHostNames)
{
}

All::~All()
{
}

std::vector<Config>
All::getConfiguration(void) const {
	return (this->_setup);
}

std::list<int>
All::getListPorts(void) const {
	return (this->_listPortsSocket);
}

std::map<std::string, Config>
All::getMapServerName(void) const {
	return (this->_mapServerName);
}
