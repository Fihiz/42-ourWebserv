#include "./WebservData.hpp"

WebservData::WebservData()
{
}

WebservData::WebservData(std::vector<Config> setup)
: _setup(setup)
{
}

WebservData::~WebservData()
{
}

void
WebservData::setPorts(void) {
	for (std::vector<Config>::iterator it = this->_setup.begin(); it != this->_setup.end(); ++it)
		this->_listPortsSocket.push_back(it->getListen());
	this->_listPortsSocket.sort();
	this->_listPortsSocket.unique();
}

void
WebservData::setHosts() {
	for (std::vector<Config>::iterator it = this->_setup.begin(); it != this->_setup.end(); ++it) {
		std::stringstream number;
		number << it->getListen();
		std::string port = number.str();
		std::string tmp = it->getServerName() + ":" + port;
		this->_mapServerName.insert(std::pair<std::string, Config>(tmp, *it));
	}
	std::cout << std::endl;
}

const std::vector<Config> &
WebservData::getConfiguration(void) const {
	return (this->_setup);
}

std::list<int>
WebservData::getListPorts(void) const {
	return (this->_listPortsSocket);
}

std::map<std::string, Config> &
WebservData::getMapServerName(void) {
	return (this->_mapServerName);
}
