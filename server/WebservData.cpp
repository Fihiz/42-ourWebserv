#include "./WebservData.hpp"


WebservData::WebservData()
{
}

WebservData::WebservData(std::vector<Config> setup)
: _setup(setup)
{
	FD_ZERO(&_readSet);
    FD_ZERO(&_readCopy);
	FD_ZERO(&_writeSet);
    FD_ZERO(&_writeCopy);

	_initHosts();
	_initPorts();
	_initSocket();
}

WebservData::~WebservData()
{
}

void
WebservData::_initPorts(void) {
	for (std::vector<Config>::iterator it = this->_setup.begin(); it != this->_setup.end(); ++it)
		this->_listPortsSocket.push_back(it->getListen());
	this->_listPortsSocket.sort();
	this->_listPortsSocket.unique();
}

void
WebservData::_initHosts() {
	for (std::vector<Config>::iterator it = this->_setup.begin(); it != this->_setup.end(); ++it) {
		std::stringstream number;
		number << it->getListen();
		std::string port = number.str();
		std::string tmp = it->getServerName() + ":" + port;
		this->_mapServerName.insert(std::pair<std::string, Config>(tmp, *it));
	}
	std::cout << std::endl;
}


void
WebservData::_initSocket() 
{
	Socket *master;

	std::list<int>::iterator ite = this->_listPortsSocket.end();
	for ( std::list<int>::iterator it = this->_listPortsSocket.begin(); it != ite ; it++)
	{
		master = new Socket(*it);
        _tabMaster.push_back(master);
        FD_SET(master->getMasterSock(), &_readSet);
		FD_SET(master->getMasterSock(), &_writeSet);
	}
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

fd_set &
WebservData::getReadSet(void)
{
	return(_readSet);
}

fd_set &
WebservData::getReadCopy(void)
{
	return(_readCopy);
}

fd_set &
WebservData::getWriteSet(void)
{
	return(_writeSet);
}


fd_set &
WebservData::getWriteCopy(void)
{
	return(_writeCopy);
}

std::vector<Socket *> &
WebservData::getTabMaster(void)
{
	return(_tabMaster);
}