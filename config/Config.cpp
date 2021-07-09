#include "Config.hpp"

// ------- CONSTRUCTOR --------------------------------------------------------
Config::Config(void)
: _listen(-1), _serverName(""), _maxBodySizeClient(0), _defaultRoot("/") // How set default value of map or vector
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
Config::setListen(std::vector<std::string> input, size_t line) {
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("listen") == 0)
		it++;
	int step = 0;
	while (it != input.end() && it->compare(";") != 0) {
		step++;
		for (size_t i = 0; i < it->size(); i++) {
			if (isdigit(it->at(i)) == false || it->size() > 5) {
				std::cerr << "Error at line #" << line << " : ";
				throw (WrongDataValueDeclarationException());
			}
		}
		this->_listen = std::atoi(it->c_str());
		it++;
	}
	if (step != 1) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

void
Config::setServerName(std::vector<std::string> input, size_t line) {
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("server_name") == 0)
		it++;
	int step = 0;
	while (it != input.end() && it->compare(";") != 0) {
		step++;
		this->_serverName = *it;
		it++;
	}
	if (step != 1) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

void
Config::setMaxBodyClient(std::vector<std::string> input, size_t line) {
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("client_max_body_size") == 0)
		it++;
	int step = 0;
	while (it != input.end() && it->compare(";") != 0) {
		step++;
		for (size_t i = 0; i < it->size(); i++) {
			if (isdigit(it->at(i)) == false) {
				std::cerr << "Error at line #" << line << " : ";
				throw (WrongDataValueDeclarationException());
			}
		}
		this->_maxBodySizeClient = std::atoi(it->c_str());
		it++;
	}
	if (step != 1) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongDataValueDeclarationException());
	}
} /* Valeur MAX qu'on peut avoir / gérer ? */ /* Gérer cas de 8MM ? */

void
Config::setErrorPage(std::vector<std::string> input, size_t line) {
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
	if (step != 2) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongDataValueDeclarationException());
	}
	this->_errorPage.insert(std::pair<int, std::string>(codeError, urlError));
}

void
Config::setRoutes(std::vector<std::string> input, size_t line) {
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("location") == 0)
		it++;
	this->_routes.clear();
	if (it->at(0) != '/') {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongDataValueDeclarationException());
	}
	this->_routes = *it;
	if (this->_location.find(this->_routes) != this->_location.end()) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongDataValueDeclarationException());
	}
	t_location bloc;
	bloc.autoindex = -1;
	this->_location.insert(std::pair<std::string, t_location>(this->_routes, bloc));
}

void
Config::setMethod(std::vector<std::string> input, size_t line) {
	std::map<std::string, t_location>::iterator routes = this->_location.find(this->_routes);
	if (routes == this->_location.end()) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongPathValueDeclarationException());
	}
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("method") == 0)
		it++;
	int step = 0;
	while (it != input.end() && it->compare(";") != 0) {
		if (it->compare("GET") != 0 && it->compare("POST") != 0 && it->compare("DELETE") != 0) {
			std::cerr << "Error at line #" << line << " : ";
			throw(WrongDataValueDeclarationException());
		}
		step++;
		routes->second.method.push_back(*it);
		it++;
	}
	if (step < 1) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

void
Config::setRoot(std::vector<std::string> input, size_t line) {
	std::map<std::string, t_location>::iterator routes = this->_location.find(this->_routes);
	if (routes == this->_location.end()) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongPathValueDeclarationException());
	}
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("root") == 0)
		it++;
	int step = 0;
	if (routes->second.root.empty() == false) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongDataValueDeclarationException());
	}
	while (it != input.end() && it->compare(";") != 0) {
		step++;
		if (it->at(0) != '/') {
			std::cerr << "Error at line #" << line << " : ";
			throw(WrongDataValueDeclarationException());
		}
		routes->second.root = *it;
		it++;
	}
	if (step != 1) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

void
Config::setIndex(std::vector<std::string> input, size_t line) {
	std::map<std::string, t_location>::iterator routes = this->_location.find(this->_routes);
	if (routes == this->_location.end()) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongPathValueDeclarationException());
	}
	std::vector<std::string>::iterator it = input.begin();
	if (it->compare("index") == 0)
		it++;
	int step = 0;
	while (it != input.end() && it->compare(";") != 0) {
		step++;
		routes->second.index.push_back(*it);
		it++;
	}
	if (step < 1) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

void
Config::setAutoindex(std::vector<std::string> input, size_t line) {
	std::map<std::string, t_location>::iterator routes = this->_location.find(this->_routes);
	if (routes == this->_location.end()) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongPathValueDeclarationException());
	}
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
		else {
			std::cerr << "Error at line #" << line << " : ";
			throw(WrongDataValueDeclarationException());
		}
		it++;
	}
	if (step != 1) {
		std::cerr << "Error at line #" << line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

void
Config::setDefault(void) {
	if (this->_listen == -1)
		this->_listen = 80;
	if (this->_serverName.compare("") == 0)
		this->_serverName = "localhost";
	if (this->_maxBodySizeClient == 0)
		this->_maxBodySizeClient = 1000;
	// valeur par default dans un location ou dans server pour root et index ?
	if (this->_location.find("/") == this->_location.end()) {
		t_location bloc;
		bloc.root = "/";
		bloc.autoindex = -1;
		this->_location.insert(std::pair<std::string, t_location>("/", bloc));
	}
	this->_defaultRoot = this->_location.find("/")->second.root;
	for (std::map<std::string, t_location>::iterator it = this->_location.begin(); it != this->_location.end(); ++it) {
		if (it->second.method.empty() == true)
			it->second.method.push_back("GET");
		if (it->second.root.empty() == true)
			it->second.root = it->first; // OU it->second.root = "/"; (?)
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

const t_location *
Config::getSpecificLocation(std::string ask) const {
	if (this->_location.find(ask) != this->_location.end())
		return (&(this->_location.find(ask)->second));
	return (NULL);
}

std::string
Config::getDefautlRoot(void) const
{
	return (_defaultRoot);
}


// ------- UTILS --------------------------------------------------------------
void
Config::reset(void) {
	this->_listen = 0;
	this->_serverName.clear();
	this->_maxBodySizeClient = 0;
	this->_errorPage.clear();
	this->_location.clear();
	this->_routes.clear();
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
	}
	return *this;
}

// ------- EXCEPTION ----------------------------------------------------------
const char *
Config::WrongDataValueDeclarationException::what() const throw () {
	return ("Error: Wrong data value declaration !");
}

const char *
Config::WrongPathValueDeclarationException::what() const throw () {
	return ("Error: Wrong path value declaration !");
}
