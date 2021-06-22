#include "Config.hpp"

// ------- CONSTRUCTOR --------------------------------------------------------
Config::Config(void)
: _listen(std::vector<unsigned short>()), _serverName(std::vector<std::string>()), _maxBodySizeClient(0)/*, _errorPage(std::map<int, std::string>()), _location(std::map<std::string, std::string>(), _socket(std::vector<Socket>())*/
{
}

// Config::Config(const Config & src)
// {
// 	*this = src;
// }

// ------- DESTRUCTOR ---------------------------------------------------------
Config::~Config(void)
{
}

// ------- OVERLOAD -----------------------------------------------------------
// ------- METHOD -------------------------------------------------------------
// ------- SETTER -------------------------------------------------------------
void
Config::setListen(std::vector<std::string> input) {
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("listen") == 0)
		it++;
	while (it != input.end() && it->compare(";") != 0) {
		if (it->size() > 5)
			throw (WrongDataValueDeclarationException());
		for (size_t i = 0; i < it->size(); i++) {
			if (isdigit(it->at(i)) == false)
				throw (WrongDataValueDeclarationException());
		}
		unsigned short value = std::atoi(it->c_str());
		this->_listen.push_back(value);
		it++;
	}
} /* Nombre de port MAX qu'on peut ouvrir / gérer ? */

void
Config::setServerName(std::vector<std::string> input) {
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("server_name") == 0)
		it++;
	while (it != input.end() && it->compare(";") != 0) {
		this->_serverName.push_back(*it);
		it++;
	}
} /* Nombre de nom MAX qu'on peut avoir / gérer ? */

void
Config::setMaxBodyClient(std::vector<std::string> input) {
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("client_max_body_size") == 0)
		it++;
	int step = 0;
	while (it != input.end() && it->compare(";") != 0) {
		step++;
		unsigned long value = std::atoi(it->c_str());
		size_t pos = it->find("M");
		if (pos != std::string::npos)
			value *= 1000;
		this->_maxBodySizeClient = value;
		it++;
	}
	if (step != 1)
		throw(WrongDataValueDeclarationException());
} /* Valeur MAX qu'on peut avoir / gérer ? */ /* Gérer cas de 8MM ? */

void
Config::setErrorPage(std::vector<std::string> input) {
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("error_page") == 0)
		it++;
	int step = 0;
	int codeError = 0;
	std::string urlError;
	while (it != input.end() && it->compare(";") != 0) {
		std::string value;
		step++;
		value = *it;
		if (step == 1)
			codeError = std::atoi(value.c_str());
		else if (step == 2)
			urlError = *it;
		it++;
	}
	if (step != 2)
		throw(WrongDataValueDeclarationException());
	this->_errorPage.insert(std::pair<int, std::string>(codeError, urlError));
}

void
Config::setRoutes(std::vector<std::string> input) {
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("location") == 0)
		it++;
	this->_routes.clear();
	if (it->at(0) != '/')
		throw(WrongDataValueDeclarationException());
	this->_routes = *it;
	t_location bloc;
	this->_location.insert(std::pair<std::string, t_location>(this->_routes, bloc));
}

void
Config::setMethod(std::vector<std::string> input) {
	std::map<std::string, t_location>::iterator routes = this->_location.find(this->_routes);
	if (routes == this->_location.end())
		throw(WrongPathValueDeclarationException());
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("method") == 0)
		it++;
	int step = 0;
	while (it != input.end() && it->compare(";") != 0) {
		if (it->compare("GET") != 0 && it->compare("POST") != 0 && it->compare("DELETE") != 0)
			throw(WrongDataValueDeclarationException());
		step++;
		routes->second.method.push_back(*it);
		it++;
	}
	if (step < 1)
		throw(WrongDataValueDeclarationException());
}

void
Config::setRoot(std::vector<std::string> input) {
	std::map<std::string, t_location>::iterator routes = this->_location.find(this->_routes);
	if (routes == this->_location.end())
		throw(WrongPathValueDeclarationException());
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("root") == 0)
		it++;
	int step = 0;
	if (routes->second.root.empty() == false)
		throw(WrongDataValueDeclarationException());
	while (it != input.end() && it->compare(";") != 0) {
		step++;
		if (it->at(0) != '/')
			throw(WrongDataValueDeclarationException());
		routes->second.root = *it;
		it++;
	}
	if (step != 1)
		throw(WrongDataValueDeclarationException());
}

void
Config::setIndex(std::vector<std::string> input) {
	std::map<std::string, t_location>::iterator routes = this->_location.find(this->_routes);
	if (routes == this->_location.end())
		throw(WrongPathValueDeclarationException());
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("index") == 0)
		it++;
	int step = 0;
	while (it != input.end() && it->compare(";") != 0) {
		step++;
		if (it->at(0) != '/')
			throw(WrongDataValueDeclarationException());
		routes->second.index.push_back(*it);
		it++;
	}
	if (step < 1)
		throw(WrongDataValueDeclarationException());
}

void
Config::setAutoindex(std::vector<std::string> input) {
	std::map<std::string, t_location>::iterator routes = this->_location.find(this->_routes);
	if (routes == this->_location.end())
		throw(WrongPathValueDeclarationException());
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("autoindex") == 0)
		it++;
	int step = 0;
	while (it != input.end() && it->compare(";") != 0) {
		step++;
		if (it->compare("on") == 0)
			routes->second.autoindex = 1;
		else if (it->compare("off") == 0)
			routes->second.autoindex = 0;
		else
			throw(WrongDataValueDeclarationException());
		it++;
	}
	if (step != 1)
		throw(WrongDataValueDeclarationException());
}

void
Config::setDefault(void) {
	if (this->_listen.empty() == true)
		this->_listen.push_back(80);
	if (this->_serverName.empty() == true)
		this->_serverName.push_back("localhost");
	if (this->_maxBodySizeClient == 0)
		this->_maxBodySizeClient = 1000;
	if (this->_location.empty() == true) {
		t_location bloc;
		bloc.method.push_back("GET");
		bloc.root = "/";
		bloc.index.push_back("index.html");
		bloc.autoindex = 0;
		this->_location.insert(std::pair<std::string, t_location>("/", bloc));
	}
} /* Segfault */

// ------- GETTER -------------------------------------------------------------
std::vector<unsigned short>
Config::getListen(void) const {
	return (this->_listen);
}

std::vector<std::string>
Config::getServerName(void) const {
	return (this->_serverName);
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
Config::getSpecificErrorPage(int ask) const {
	if (this->_errorPage.find(ask) != this->_errorPage.end())
		return (this->_errorPage.find(ask)->second);
	else
		return ("");
}

// std::map<std::string, t_location>
// Config::getLocation(void) const {
// 	return (this->_location);
// }

// t_location *
// Config::getSpecificLocation(std::string ask) const {
// 	if (this->_errorPage.find(ask) != this->_errorPage.end())
// 		return (this->_location[ask]);
// 	return (NULL);
// }

// ------- UTILS --------------------------------------------------------------
void
Config::reset(void) {
	this->_listen.clear();
	this->_serverName.clear();
	this->_maxBodySizeClient = 0;
	this->_errorPage.clear();
	this->_location.clear();
	this->_routes.clear();
}

void
Config::printListen(void) const {
	if (this->_listen.empty() == true)
		return ;
	std::cout << "Listen: ";
	for (std::vector<unsigned short>::const_iterator it = this->_listen.begin(); it != this->_listen.end(); it++)
		std::cout << *it << " ";
	std::cout << std::endl;
}

void
Config::printServerName(void) const {
	if (this->_serverName.empty() == true)
		return ;
	std::cout << "Server Name: ";
	for (std::vector<std::string>::const_iterator it = this->_serverName.begin(); it != this->_serverName.end(); it++)
		std::cout << *it << " ";
	std::cout << std::endl;
}

void
Config::printMaxBodyClient(void) const {
	std::cout << "Max Body Client: " << this->_maxBodySizeClient << std::endl;
}

void
Config::printErrorPage(void) const {
	if (this->_errorPage.empty() == true)
		return ;
	std::cout << "Error Page: ";
	for (std::map<int, std::string>::const_iterator it = this->_errorPage.begin(); it != this->_errorPage.end(); it++) {
		std::cout << it->first << "=" << it->second << " ";
	}
	std::cout << std::endl;
}

void
Config::printLocation(void) const {
	if (this->_location.empty() == true)
		return ;
	std::cout << "Location: " << std::endl;
	for (std::map<std::string, t_location>::const_iterator it1 = this->_location.begin(); it1 != this->_location.end(); it1++)
	{
		std::cout << it1->first << " ";
		for (std::vector<std::string>::const_iterator it2 = it1->second.method.begin(); it2 != it1->second.method.end(); it2++)
			std::cout << *it2 << " ";
		std::cout << it1->second.root << " ";
		for (std::vector<std::string>::const_iterator it2 = it1->second.index.begin(); it2 != it1->second.index.end(); it2++)
			std::cout << *it2 << " ";
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
		this->_maxBodySizeClient = rhs._maxBodySizeClient;
		this->_errorPage = rhs._errorPage;
		this->_location = rhs._location;
		// this->_socket = rhs._socket;
	}
	return *this;
}

// ------- EXCEPTION ----------------------------------------------------------
const char *
Config::WrongDataValueDeclarationException::what() const throw () {
	return ("Error: Wrong data value declaration in configuration file !");
}

const char *
Config::WrongPathValueDeclarationException::what() const throw () {
	return ("Error: Wrong path value declaration in configuration file !");
}
