#include "./All.hpp"

All::All()
{
}

All::All(std::vector<Config> setup)
: _setup(setup)
{
}

All::~All()
{
}

void
All::setPorts(void) {
	for (std::vector<Config>::iterator it = this->_setup.begin(); it != this->_setup.end(); ++it)
		this->_listPortsSocket.push_back(it->getListen());
	this->_listPortsSocket.sort();
	this->_listPortsSocket.unique();
}

void
All::setHosts() {
	for (std::vector<Config>::iterator it = this->_setup.begin(); it != this->_setup.end(); ++it) {
		std::stringstream number;
		number << it->getListen();
		std::string port = number.str();
		std::string tmp = it->getServerName() + ":" + port;
		this->_mapServerName.insert(std::pair<std::string, Config>(tmp, *it));
	}
	std::cout << std::endl;
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
