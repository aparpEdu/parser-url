#include <iostream>
#include <cctype>
#include <fstream>
#include <string>

enum TSymbolType {
    protocol, domain, port, path, colon, period, quotas, othersy, semicolon
};

char Char;
TSymbolType Symbol;
std::string Spelling;

std::ifstream inputFile("input.txt");

void GetNextChar() {
    if (!inputFile.is_open()) {
        std::cerr << "Error opening the file!" << std::endl;
        return;
    }
    Char = inputFile.get();
}

void error(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
}

void GetNextSymbol() {
    while (Char != '\0' && !isalpha(Char) && Char != ':' && Char != '.' && Char != '/' && Char != ';' && Char != '\"') {
        GetNextChar();
    }

    if (Char == '\0' || inputFile.eof()) {
        Symbol = othersy;
        std::cout << "End of Input" << std::endl;
        return;
    }

    while (Char == ' ') {
        GetNextChar();
    }

    switch (toupper(Char)) {
        case 'A' ... 'Z': {
            Spelling = "";
            while (isalpha(Char) || isdigit(Char) || Char == '.') {
                Spelling.push_back(Char);
                GetNextChar();
            }

            if (Spelling == "http" || Spelling == "https") {
                Symbol = protocol;
                std::cout << "Protocol: " << Spelling << std::endl;
            } else {
                Symbol = domain;
                std::cout << "Domain: " << Spelling << std::endl;
            }

        } break;
        case '0' ... '9': {
            Spelling = "";
            while (isdigit(Char)) {
                Spelling.push_back(Char);
                GetNextChar();
            }

            Symbol = port;
            std::cout << "Port: " << Spelling << std::endl;
        } break;
        case '/' : {
            Symbol = path;
            Spelling = "";
            while (Char != ';' && Char != '\"' && Char != '\n' && Char != EOF) {
                Spelling.push_back(Char);
                GetNextChar();
            }
            std::cout << "Path: " << Spelling << std::endl;
        } break;
        case ':' : {
            Symbol = colon;
            std::cout << "Column" << std::endl;

            while (Char == ':' || Char == ' ') {
                GetNextChar();
            }

            Spelling = "";
            while (Char != '\0' && Char != ';' && Char != '\"' && Char != '\n') {
                Spelling.push_back(Char);
                GetNextChar();
            }
            std::cout << "Path: " << Spelling << std::endl;
        } break;
        case ';' : {
            Symbol = semicolon;
            std::cout << "Semicolon" << std::endl;

            while (Char != '\n' && Char != EOF) {
                GetNextChar();
            }
        } break;
        case '\"' : {
            Symbol = quotas;
            Spelling = "";
            while (Char != '\"' && Char != '\n' && Char != EOF) {
                Spelling.push_back(Char);
                GetNextChar();
            }
            std::cout << "String: " << Spelling << std::endl;
        } break;
        default: {
            Symbol = othersy;
            GetNextChar();
            std::cout << "Unknown Symbol" << std::endl;
        } break;
    }
}

int accept(TSymbolType symbol) {
    if (Symbol == symbol) {
        GetNextSymbol();
        return 1;
    }
    return 0;
}

void Field() {
    while (!accept(protocol) && !accept(port) && !accept(path) && !accept(colon) && !accept(semicolon)) {
        if (Symbol == othersy) {
            error("field: unexpected symbol");
            return;
        } else {
            GetNextSymbol();
        }
    }
}

void Record() {
    Field();
    while (Symbol == period) {
        accept(period);
        Field();
    }
}

void DataFile() {
    Record();
    while (Symbol != othersy && !inputFile.eof()) {
        Record();
    }
    inputFile.close();
}

int main() {
    GetNextChar();
    GetNextSymbol();
    DataFile();
    return 0;
}
