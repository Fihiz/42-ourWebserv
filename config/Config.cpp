#include "Config.hpp"

// ------- CONSTRUCTOR --------------------------------------------------------
Config::Config(void)
: _listen(-1), _serverName(""), _root(""), _maxBodySizeClient(0)
{
}

// ------- DESTRUCTOR ---------------------------------------------------------
Config::~Config(void)
{
}

// ------- OVERLOAD -----------------------------------------------------------
// ------- METHOD -------------------------------------------------------------
// ------- SETTER -------------------------------------------------------------
void
Config::setListen(int portNumber) {
	this->_listen = portNumber;
}

void
Config::setServerName(std::string hostName) {
	this->_serverName = hostName;
}

void
Config::setMaxBodyClient(unsigned long bodySize) {
	this->_maxBodySizeClient = bodySize;
}

void
Config::setErrorPage(int codeError, std::string urlError) {
	this->_errorPage.insert(std::pair<int, std::string>(codeError, urlError));
}

void
Config::setRoutes(std::string routes) {
	t_location bloc;
	bloc.autoindex = -1;
	if (routes == "/")
		bloc.index = this->_index;
	this->_location.insert(std::pair<std::string, t_location>(routes, bloc));
}

void
Config::setMethod(std::string routes, std::string valMethod) {
	std::map<std::string, t_location>::iterator it = this->_location.find(routes);
	it->second.method.push_back(valMethod);
}

void
Config::setRoot(std::string routes, std::string dirName) {
	if (routes.empty() == true)
		this->_root = dirName;
	else {
		std::map<std::string, t_location>::iterator it = this->_location.find(routes);
		if (it != this->_location.end())
			it->second.root = dirName;
	}
}

void
Config::setIndex(std::string routes, std::string fileName) {
	if (routes.empty() == true)
		this->_index.push_back(fileName);
	else {
		std::map<std::string, t_location>::iterator it = this->_location.find(routes);
		if (it != this->_location.end())
			it->second.index.push_back(fileName);
	}
}

void
Config::setCgi(std::string routes, std::string langageType, std::string executableCgi) {
	std::map<std::string, t_location>::iterator it = this->_location.find(routes);
	it->second.cgi.insert(std::pair<std::string, std::string>(langageType, executableCgi));
}

void
Config::setAutoindex(std::string routes, int valAutoindex) {
	std::map<std::string, t_location>::iterator it = this->_location.find(routes);
	it->second.autoindex = valAutoindex;
}

void
Config::setReturn(std::string routes, std::string redirect) {
	std::map<std::string, t_location>::iterator it = this->_location.find(routes);
	it->second.redirect = redirect;
}

void
Config::setDefaultValue(void) {
	if (this->_listen == -1)
		this->_listen = 80;
	if (this->_serverName.empty() == true)
		this->_serverName = "localhost";
	if (this->_root.empty() == true)
		this->_root = "/";
	if (this->_index.empty() == true)
		this->_index.push_back("index.html");
	if (this->_maxBodySizeClient == 0)
		this->_maxBodySizeClient = 1000000;
	if (this->_location.find("/") == this->_location.end())
		this->setRoutes("/");
	for (std::map<std::string, t_location>::iterator it = this->_location.begin(); it != this->_location.end(); ++it) {
		if (it->second.method.empty() == true)
			it->second.method.push_back("GET");
		if (it->second.root.empty() == true)
			it->second.root = this->_root;
		if (it->second.index.empty() == true)
			it->second.index.push_back("index.html");
		if (it->second.autoindex == -1)
			it->second.autoindex = 0;
	}
}

// ------- GETTER -------------------------------------------------------------
int
Config::getListen(void) const {
	return (this->_listen);
}

std::string
Config::getServerName(void) const {
	return (this->_serverName);
}

std::string
Config::getRoot(std::string routes) const {
	if (routes.empty() == true)
		return (this->_root);
	std::map<std::string, t_location>::const_iterator it = this->_location.find(routes);
	if (it != this->_location.end())
		return (it->second.root);
	return ("");
}

std::vector<std::string>
Config::getIndex(std::string routes) const {
	if (routes.empty() == false) {
		std::map<std::string, t_location>::const_iterator it = this->_location.find(routes);
		if (it != this->_location.end())
			return (it->second.index);
	}
	return (this->_index);
}

unsigned long
Config::getMaxBodySize(void) const {
	return (this->_maxBodySizeClient);
}

std::map<int, std::string>
Config::getErrorPage(void) const {
	return (this->_errorPage);
}

std::string
Config::getRoutes(std::string ask) const {
	if (ask.empty() == false)
	{
		std::map<std::string, t_location>::const_iterator it = this->_location.find(ask);
		if (it != this->_location.end())
			return (it->first);
	}
	return ("");
}

std::string
Config::getCgi(std::string routes, std::string extension) const {
	if (routes.empty() == false && extension.empty() == false)
	{
		std::map<std::string, t_location>::const_iterator it1 = this->_location.find(routes);
		if (it1 != this->_location.end()) {
			std::map<std::string, std::string>::const_iterator it2 = it1->second.cgi.find(extension);
			if (it2 != it1->second.cgi.end())
				return (it2->second);
		}
	}
	return ("");
}

const t_location *
Config::getLocation(std::string ask) const {
	if (this->_location.find(ask) != this->_location.end())
		return (&(this->_location.find(ask)->second));
	return (NULL);
}

// ------- UTILS --------------------------------------------------------------
void
Config::reset(void) {
	this->_listen = 0;
	this->_serverName.clear();
	this->_root.clear();
	this->_index.clear();
	this->_maxBodySizeClient = 0;
	this->_errorPage.clear();
	this->_location.clear();
}

void
Config::printListen(void) const {
	std::cout << "Listen: " << this->_listen << std::endl;
}

void
Config::printServerName(void) const {
	std::cout << "Server Name: " << this->_serverName << " " << std::endl;
}

void
Config::printServerRoot(void) const {
	std::cout << "Root: " << this->_root << " " << std::endl;
}

void
Config::printServerIndex(void) const {
	std::cout << "Index: ";
	for (std::vector<std::string>::const_iterator it = this->_index.begin(); it != this->_index.end(); ++it)
		std::cout << *it << " " << std::endl;
}

void
Config::printMaxBodyClient(void) const {
	std::cout << "Max Body Client: " << this->_maxBodySizeClient << std::endl;
}

void
Config::printErrorPage(void) const {
	std::cout << "Error Page: " << std::endl;
	if (this->_errorPage.empty() == true)
		return ;
	for (std::map<int, std::string>::const_iterator it = this->_errorPage.begin(); it != this->_errorPage.end(); it++) {
		std::cout << it->first << "=" << it->second << " ";
	}
	std::cout << std::endl;
}

void
Config::printLocation(void) const {
	std::cout << "Location: " << std::endl;
	if (this->_location.empty() == true)
		return ;
	for (std::map<std::string, t_location>::const_iterator it1 = this->_location.begin(); it1 != this->_location.end(); it1++)
	{
		std::cout << it1->first << " ";
		for (std::vector<std::string>::const_iterator it2 = it1->second.method.begin(); it2 != it1->second.method.end(); it2++)
			std::cout << *it2 << " ";
		std::cout << it1->second.root << " ";
		for (std::vector<std::string>::const_iterator it2 = it1->second.index.begin(); it2 != it1->second.index.end(); it2++)
			std::cout << *it2 << " ";
		for (std::map<std::string, std::string>::const_iterator it2 = it1->second.cgi.begin(); it2 != it1->second.cgi.end(); it2++) {
			std::cout << it2->first << " ";
			std::cout << it2->second << " ";
		}
		std::cout << it1->second.autoindex << " ";
		std::cout << std::endl;
	}
}

// ------- OPERATOR -----------------------------------------------------------
Config &
Config::operator=(Config const & rhs)
{
	if (this != &rhs) {
		this->_listen = rhs._listen;
		this->_serverName = rhs._serverName;
		this->_root = rhs._root;
		this->_index = rhs._index;
		this->_maxBodySizeClient = rhs._maxBodySizeClient;
		this->_errorPage = rhs._errorPage;
		this->_location = rhs._location;
	}
	return *this;
}

// ------- EXCEPTION ----------------------------------------------------------
