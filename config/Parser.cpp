#include "./Parser.hpp"

// ------- CONSTRUCTOR --------------------------------------------------------
Parser::Parser(std::string const & path)
{
    this->_file.open(path);
    if (this->_file.fail() == true)
		throw (FileOpeningFailedException());
}

// ------- DESTRUCTOR ---------------------------------------------------------
Parser::~Parser(void) {
	if (this->_file.is_open() == true)
		this->_file.close();
}

// ------- OVERLOAD -----------------------------------------------------------
// ------- METHOD -------------------------------------------------------------
void
Parser::checkGeneralSyntax() {
	this->_line = 1;
	while (this->_line <= this->_content.size()) {
		this->_tokens = this->_content.find(this->_line)->second;
		if (this->_tokens.empty() == false) {
			if (find(this->_tokens.begin(), this->_tokens.end(), "{") != this->_tokens.end())
				this->checkOpenContext();
			else if (find(this->_tokens.begin(), this->_tokens.end(), "}") != this->_tokens.end())
				this->checkCloseContext();
			else
				this->checkDirective();
		}
		this->_line++;
	}
	if (this->_context.size() != 0) {
		std::cerr << "Error at #eof" << " : ";
		throw (UnexpectedContextDeclarationException());
	}
	this->reset();
}

void
Parser::checkOpenContext() {
	if (this->_tokens.front().compare("server") == 0) {
		if (this->_tokens.size() != 2
		|| (this->_context.empty() == false && find(this->_context.begin(), this->_context.end(), "server") != this->_context.end())) {
			std::cerr << "Error at line #" << this->_line << " : ";
			throw (WrongServerDeclarationException());
		}
	}
	else if (this->_tokens.front().compare("location") == 0) {
		if (this->_tokens.size() != 3 
		|| (this->_context.empty() == false && find(this->_context.begin(), this->_context.end(), "location") != this->_context.end())) {
			std::cerr << "Error at line #" << this->_line << " : ";
			throw (WrongLocationDeclarationException());
		}
	}
	else {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw (UnexpectedContextDeclarationException());
	}
	this->_context.push_back(this->_tokens.front());
	if (this->_context.size() > 2) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw (UnexpectedContextDeclarationException());
	}
}

void
Parser::checkCloseContext() {
	size_t len = this->_tokens.size();
	if (len != 1) {
		std::cerr << "Error at line #" << this->_line << " : ";
		if (this->_context.empty() == false
		&& this->_context.back().compare("server") == 0)
			throw (WrongServerDeclarationException());
		else if (this->_context.empty() == false
		&& this->_context.back().compare("location") == 0)
			throw (WrongLocationDeclarationException());
		else
			throw (UnexpectedContextDeclarationException());
	}
	if (this->_context.empty() == true) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw (UnexpectedContextDeclarationException());
	}
	this->_context.pop_back();
}

void
Parser::checkDirective() {
	if (find(this->_tokens.begin(), this->_tokens.end(), ";") == this->_tokens.end()) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw (WrongDirectiveDeclarationException());
	}
	if (this->_tokens.front().compare("listen") != 0
	&& this->_tokens.front().compare("server_name") != 0
	&& this->_tokens.front().compare("max_body_size") != 0
	&& this->_tokens.front().compare("error_page") != 0
	&& this->_tokens.front().compare("method") != 0
	&& this->_tokens.front().compare("root") != 0
	&& this->_tokens.front().compare("index") != 0
	&& this->_tokens.front().compare("cgi") != 0
	&& this->_tokens.front().compare("autoindex") != 0) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw (WrongDirectiveDeclarationException());
	}
}

void
Parser::checkListen(Config & virtualHost) {
	std::vector<std::string>::iterator it = this->_tokens.begin();
	if (it->compare("listen") == 0)
		it++;
	int step = 0;
	while (it != this->_tokens.end() && it->compare(";") != 0) {
		step++;
		for (size_t i = 0; i < it->size(); i++) {
			if (isdigit(it->at(i)) == false || it->size() > 5) {
				std::cerr << "Error at line #" << this->_line << " : ";
				throw (WrongDataValueDeclarationException());
			}
		}
		virtualHost.setListen(std::atoi(it->c_str()));
		it++;
	}
	if (step != 1) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

void
Parser::checkServerName(Config & virtualHost) {
	std::vector<std::string>::iterator it = this->_tokens.begin();
	if (it->compare("server_name") == 0)
		it++;
	int step = 0;
	while (it != this->_tokens.end() && it->compare(";") != 0) {
		step++;
		virtualHost.setServerName(*it);
		it++;
	}
	if (step != 1) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

void
Parser::checkMaxBodyClient(Config & virtualHost) {
	std::vector<std::string>::iterator it = this->_tokens.begin();
	if (it->compare("max_body_size") == 0)
		it++;
	int step = 0;
	while (it != this->_tokens.end() && it->compare(";") != 0) {
		step++;
		for (size_t i = 0; i < it->size(); i++) {
			if (isdigit(it->at(i)) == false) {
				std::cerr << "Error at line #" << this->_line << " : ";
				throw (WrongDataValueDeclarationException());
			}
		}
		virtualHost.setMaxBodyClient(std::atoi(it->c_str()));
		it++;
	}
	if (step != 1) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

void
Parser::checkErrorPage(Config & virtualHost) {
	std::vector<std::string>::iterator it = this->_tokens.begin();
	if (it->compare("error_page") == 0)
		it++;
	int step = 0;
	int codeError = 0;
	std::string pageError;
	while (it != this->_tokens.end() && it->compare(";") != 0) {
		std::string value;
		step++;
		value = *it;
		if (step == 1)
			codeError = std::atoi(value.c_str());
		else if (step == 2)
			pageError = *it;
		it++;
	}
	if (step != 2) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw(WrongDataValueDeclarationException());
	}
	virtualHost.setErrorPage(codeError, pageError);
}

void
Parser::checkRoutes(Config & virtualHost) {
	std::vector<std::string>::iterator it = this->_tokens.begin();
	if (it->compare("location") == 0)
		it++;
	if (it->at(0) != '/') {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw(WrongDataValueDeclarationException());
	}
	this->_routes = *it;
	virtualHost.setRoutes(this->_routes);
}

void
Parser::checkMethod(Config & virtualHost) {
	std::vector<std::string>::iterator it = this->_tokens.begin();
	if (it->compare("method") == 0)
		it++;
	int step = 0;
	while (it != this->_tokens.end() && it->compare(";") != 0) {
		if (it->compare("GET") != 0 && it->compare("POST") != 0 && it->compare("DELETE") != 0) {
			std::cerr << "Error at line #" << this->_line << " : ";
			throw(WrongDataValueDeclarationException());
		}
		step++;
		virtualHost.setMethod(this->_routes, *it);
		it++;
	}
	if (step < 1) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

void
Parser::checkRoot(Config & virtualHost) {
	if ((this->_routes.empty() == true && virtualHost.getRoot("").empty() == false)
	|| (this->_routes.empty() == false && virtualHost.getRoot(this->_routes).empty() == false)) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw(WrongPathValueDeclarationException());
	}
	std::vector<std::string>::iterator it = this->_tokens.begin();
	if (it->compare("root") == 0)
		it++;
	int step = 0;
	while (it != this->_tokens.end() && it->compare(";") != 0) {
		step++;
		if (it->at(0) != '/') {
			std::cerr << "Error at line #" << this->_line << " : ";
			throw(WrongDataValueDeclarationException());
		}
		virtualHost.setRoot(this->_routes, *it);
		it++;
	}
	if (step != 1) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

void
Parser::checkIndex(Config & virtualHost) {
	std::vector<std::string>::iterator it = this->_tokens.begin();
	if (it->compare("index") == 0)
		it++;
	int step = 0;
	while (it != this->_tokens.end() && it->compare(";") != 0) {
		step++;
		virtualHost.setIndex(this->_routes, *it);
		it++;
	}
	if (step < 1) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

void
Parser::checkCgi(Config & virtualHost) {
	std::vector<std::string>::iterator it = this->_tokens.begin();
	if (it->compare("cgi") == 0)
		it++;
	int step = 0;
	std::string langage;
	std::string executable;
	while (it != this->_tokens.end() && it->compare(";") != 0) {
		step++;
		if (step == 1) {
			if (it->at(0) != '.') {
				std::cerr << "Error at line #" << this->_line << " : ";
				throw(WrongDataValueDeclarationException());
			}
			langage = *it;
		}
		else if (step == 2) {
			if (it->at(0) != '/') {
				std::cerr << "Error at line #" << this->_line << " : ";
				throw(WrongDataValueDeclarationException());
			}
			executable = *it;
		}
		it++;
	}
	if (step != 2) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw(WrongDataValueDeclarationException());
	}
	virtualHost.setCgi(this->_routes, langage, executable);
}

void
Parser::checkAutoindex(Config & virtualHost) {
	std::vector<std::string>::iterator it = this->_tokens.begin();
	if (it->compare("autoindex") == 0)
		it++;
	int step = 0;
	while (it != this->_tokens.end() && it->compare(";") != 0) {
		step++;
		if (it->compare("on") == 0)
			virtualHost.setAutoindex(this->_routes, 1);
		else if (it->compare("off") == 0)
			virtualHost.setAutoindex(this->_routes, 0);
		else {
			std::cerr << "Error at line #" << this->_line << " : ";
			throw(WrongDataValueDeclarationException());
		}
		it++;
	}
	if (step != 1) {
		std::cerr << "Error at line #" << this->_line << " : ";
		throw(WrongDataValueDeclarationException());
	}
}

// ------- SETTER -------------------------------------------------------------
void
Parser::setContentFile() {
	int i = 1;
	std::string line;
	while (getline(this->_file, line, '\n')) {
		size_t len = line.size();
		for (size_t idx = 0; idx < len;) {
			std::string word;
			size_t min = idx;
			while (min < len && isspace(line.at(min)) == true)
				min++;
			size_t max = 0;
			while (min + max < len && isspace(line.at(min + max)) == false)
				max++;
			if (max > 0) {
				word = line.substr(min, max);
				this->_tokens.push_back(word);
				idx = min + max;
			}
			else
				idx = min + 1;
		}
		line.clear();
		this->_content.insert(std::pair<int, std::vector<std::string> >(i, this->_tokens));
		this->_tokens.clear();
		i++;
	}
}

void
Parser::setConfiguration() {
	Config virtualHost;
	this->_line = 1;
	while (this->_line <= this->_content.size()) {
		this->_tokens = this->_content.find(this->_line)->second;
		if (this->_tokens.empty() == false) {
			this->setStartContext();
			std::string context = this->getContext();
			std::string directive = this->getDirective();
			if (context.compare("server") == 0) {
				if (directive.compare("listen") == 0)
					this->checkListen(virtualHost);
				else if (directive.compare("server_name") == 0)
					this->checkServerName(virtualHost);
				else if (directive.compare("root") == 0)
					this->checkRoot(virtualHost);
				else if (directive.compare("index") == 0)
					this->checkIndex(virtualHost);
				else if (directive.compare("max_body_size") == 0)
					this->checkMaxBodyClient(virtualHost);
				else if (directive.compare("error_page") == 0)
					this->checkErrorPage(virtualHost);
			}
			else if (context.compare("location") == 0) {
				if (this->_tokens.front().compare("location") == 0)
					this->checkRoutes(virtualHost);
				if (directive.compare("method") == 0)
					this->checkMethod(virtualHost);
				else if (directive.compare("root") == 0)
					this->checkRoot(virtualHost);
				else if (directive.compare("index") == 0)
					this->checkIndex(virtualHost);
				else if (directive.compare("cgi") == 0)
					this->checkCgi(virtualHost);
				else if (directive.compare("autoindex") == 0)
					this->checkAutoindex(virtualHost);
			}
			this->setEndContext();
			if (this->getContext().compare("") == 0) {
				virtualHost.setDefaultValue();
				this->_setup.push_back(virtualHost);
				virtualHost.reset();
			}
			this->_tokens.clear();
		}
		this->_line++;
	}
}

void
Parser::setStartContext(void) {
	if (find(this->_tokens.begin(), this->_tokens.end(), "{") != this->_tokens.end())
		this->_context.push_back(this->_tokens.front());
}

void
Parser::setEndContext(void) {
	if (this->_context.empty() == false
	&& find(this->_tokens.begin(), this->_tokens.end(), "}") != this->_tokens.end()) {
		this->_context.pop_back();
		this->_routes.clear();
	}
}

// ------- GETTER -------------------------------------------------------------
std::string
Parser::getContext(void) const {
	if (this->_context.empty() == true)
		return ("");
	return (this->_context.back());
}

std::string
Parser::getDirective(void) const {
	if (this->_tokens.empty() == true
	|| find(this->_tokens.begin(), this->_tokens.end(), ";") == this->_tokens.end())
		return ("");
	return (this->_tokens.front());
}

std::vector<Config>
Parser::getConfiguration(void) const {
	return (this->_setup);
}

// ------- UTILS --------------------------------------------------------------
void
Parser::reset(void) {
	if (this->_line != 0)
		this->_line = 0;
	if (this->_tokens.empty() == false)
		this->_tokens.clear();
	if (this->_context.empty() == false)
		this->_context.clear();
	if (this->_routes.empty() == false)
		this->_routes.clear();
}

void
Parser::printContent(void) {
	for (size_t i = 1; i < this->_content.size(); ++i) {
		std::map<int, std::vector<std::string> >::iterator it_map = this->_content.find(i);
		std::cout << it_map->first << " ";
		for (std::vector<std::string>::iterator it_str = it_map->second.begin(); it_str != it_map->second.end(); ++it_str)
			std::cout << *it_str << " ";
		std::cout << std::endl;
	}
}

void
Parser::printConfigs(void) {
	for (std::vector<Config>::iterator it = this->_setup.begin(); it != this->_setup.end(); ++it) {
		it->printListen();
		it->printServerName();
		it->printServerRoot();
		it->printServerIndex();
		it->printMaxBodyClient();
		it->printErrorPage();
		it->printLocation();
	}
}

// ------- OPERATOR -----------------------------------------------------------
// ------- EXCEPTION ----------------------------------------------------------
const char *
Parser::FileOpeningFailedException::what() const throw () {
	return ("Error: File opening Failed !");
}

const char *
Parser::FileClosingFailedException::what() const throw () {
	return ("Error: File closing Failed !");
}

const char *
Parser::WrongServerDeclarationException::what() const throw () {
	return ("Wrong server declaration !");
}

const char *
Parser::WrongLocationDeclarationException::what() const throw () {
	return ("Wrong location declaration !");
}

const char *
Parser::UnexpectedContextDeclarationException::what() const throw () {
	return ("Unexpected context declaration !");
}

const char *
Parser::WrongDirectiveDeclarationException::what() const throw () {
	return ("Wrong directive declaration !");
}

const char *
Parser::WrongDataValueDeclarationException::what() const throw () {
	return ("Error: Wrong data value declaration !");
}

const char *
Parser::WrongPathValueDeclarationException::what() const throw () {
	return ("Error: Wrong path value declaration !");
}