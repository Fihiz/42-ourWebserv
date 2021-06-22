#include "./Parser.hpp"

// ------- CONSTRUCTOR --------------------------------------------------------
Parser::Parser(void) {}

Parser::Parser(const std::string & path)
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
Parser::checkSyntax() {
	size_t i = 0;
	while (i <= this->_content.size()) {
		this->_tokens = this->_content.find(i)->second;
		if (this->_tokens.empty() == false) {
			if (find(this->_tokens.begin(), this->_tokens.end(), "{") != this->_tokens.end())
				this->CheckOpenContext(i);
			else if (find(this->_tokens.begin(), this->_tokens.end(), "}") != this->_tokens.end())
				this->CheckCloseContext(i);
			else
				this->CheckDirective(i);
		}
		i++;
	}
	if (this->_context.size() != 0) {
		std::cerr << "Error at #eof" << " : ";
		throw (UnexpectedContextDeclarationException());
	}
	this->reset();
}

void
Parser::CheckOpenContext(size_t line) {
	if (this->_tokens.front().compare("server") == 0) {
		// Argument different de 2 || Server Imbriqué
		if (this->_tokens.size() != 2
		|| (this->_context.empty() == false && find(this->_context.begin(), this->_context.end(), "server") != this->_context.end())) {
			std::cerr << "Error at line #" << line << " : ";
			throw (WrongServerDeclarationException());
		}
	}
	else if (this->_tokens.front().compare("location") == 0) {
		// Argument different de 3 || Location Imbriqué
		if (this->_tokens.size() != 3 
		|| (this->_context.empty() == false && find(this->_context.begin(), this->_context.end(), "location") != this->_context.end())) {
			std::cerr << "Error at line #" << line << " : ";
			throw (WrongLocationDeclarationException());
		}
	}
	else {
		std::cerr << "Error at line #" << line << " : ";
		throw (UnexpectedContextDeclarationException());
	}
	this->_context.push_back(this->_tokens.front());
	if (this->_context.size() > 2) {
		std::cerr << "Error at line #" << line << " : ";
		throw (UnexpectedContextDeclarationException());
	}
}

void
Parser::CheckCloseContext(size_t line) {
	size_t len = this->_tokens.size();
	if (len != 1) {
		std::cerr << "Error at line #" << line << " : ";
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
		std::cerr << "Error at line #" << line << " : ";
		throw (UnexpectedContextDeclarationException());
	}
	this->_context.pop_back();
}

void
Parser::CheckDirective(size_t line) {
	if (find(this->_tokens.begin(), this->_tokens.end(), ";") == this->_tokens.begin()) {
		std::cerr << "Error at line #" << line << " : ";
		throw (WrongDirectiveDeclarationException());
	}
	if (this->_tokens.front().compare("listen") != 0 
	&& this->_tokens.front().compare("server_name") != 0
	&& this->_tokens.front().compare("client_max_body_size") != 0
	&& this->_tokens.front().compare("error_page") != 0
	&& this->_tokens.front().compare("method") != 0
	&& this->_tokens.front().compare("root") != 0
	&& this->_tokens.front().compare("index") != 0
	&& this->_tokens.front().compare("autoindex") != 0) {
		std::cerr << "Error at line #" << line << " : ";
		throw (WrongDirectiveDeclarationException());
	}
}

// ------- SETTER -------------------------------------------------------------
void
Parser::setContent() {
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
	Config unit;
	size_t  i = 1;
	while (i <= this->_content.size()) {
		this->_tokens = this->_content.find(i)->second;
		if (this->_tokens.empty() == false) {
			this->setStartContext();
			std::string context = this->getContext();
			std::string directive = this->getDirective();
			if (context.compare("server") == 0) {
				if (directive.compare("listen") == 0)
					unit.setListen(this->_tokens);
				else if (directive.compare("server_name") == 0)
					unit.setServerName(this->_tokens);
				else if (directive.compare("client_max_body_size") == 0)
					unit.setMaxBodyClient(this->_tokens);
				else if (directive.compare("error_page") == 0)
					unit.setErrorPage(this->_tokens);
			}
			else if (context.compare("location") == 0) {
				if (this->_tokens.front().compare("location") == 0)
					unit.setRoutes(this->_tokens);
				if (directive.compare("method") == 0)
					unit.setMethod(this->_tokens);
				else if (directive.compare("root") == 0)
					unit.setRoot(this->_tokens);
				else if (directive.compare("index") == 0)
					unit.setIndex(this->_tokens);
				else if (directive.compare("autoindex") == 0)
					unit.setAutoindex(this->_tokens);
			}
			this->setEndContext();
			if (this->getContext().compare("") == 0) {
				//		!!!!!	 	unit.setDefault(); // Gestion location vide ou server vide
				this->_setup.push_back(unit);
				unit.reset();
			}
			this->_tokens.clear();
		}
		i++;
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
	&& find(this->_tokens.begin(), this->_tokens.end(), "}") != this->_tokens.end())
		this->_context.pop_back();
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
	if (this->_tokens.empty() == false)
		this->_tokens.clear();
	if (this->_context.empty() == false)
		this->_context.clear();
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
