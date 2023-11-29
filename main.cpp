#include <iostream>
#include <cctype>
#include <fstream>
#include <string>

enum TSymbolType {
    protocol, domain, port, path, colon, period, quotas, othersy, semicolon, machine
};

char Char;
TSymbolType Symbol;
std::string Spelling;

std::ifstream inputFile("/Users/moldovexc/CLionProjects/parserurl/input.txt");

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
        // Inside the switch statement where the domain is identified
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
                std::string domainStr = Spelling;

                // Check if the domain starts with "www."
                if (domainStr.substr(0, 4) == "www.") {
                    domainStr = domainStr.substr(4); // Remove "www."
                }

                // Extract and output the domain
                std::cout << "Domain: " << domainStr << std::endl;
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
            std::string pathStr = Spelling;

            // Check if "www." exists in the path and remove it
            size_t wwwPos = pathStr.find("www.");
            if (wwwPos != std::string::npos) {
                pathStr.erase(wwwPos, 4); // Remove "www."
            }

//            std::cout << "Path: " << pathStr << std::endl;
            std::cout<<pathStr<<std::endl;
        } break;

        case 'w' : {
            Spelling.push_back(Char);
            GetNextChar();
            if(Char == 'w') {
                GetNextChar();
                Spelling.push_back(Char);
                if(Char == 'w') {
                    Symbol = machine;
                    GetNextChar();
                    Spelling.push_back(Char);
                    std::cout << "Machine: " << Spelling << std::endl;
                }
            }
        }
        case ':' : {
            Symbol = colon;
            std::cout << "Colon" << std::endl;

            while (Char == ':' || Char == ' ') {
                GetNextChar();
            }

            if (Char == '/') {
                GetNextChar();
                Symbol = path;
                Spelling = "";

                std::string removedPart = ""; // Variable to store the removed part

                while (Char != ';' && Char != '\"' && Char != '\n' && Char != EOF) {
                    Spelling.push_back(Char);
                    GetNextChar();
                }

                size_t wwwPos = Spelling.find("www.");
                if (wwwPos != std::string::npos) {
                    removedPart = "www"; // Store the removed part
                    Spelling.erase(wwwPos, 4);
                    Symbol = machine; // Update the symbol to represent the removal of "www"
                }

                size_t www1Pos = Spelling.find("www1.");
                if (www1Pos != std::string::npos) {
                    removedPart = "www1"; // Store the removed part
                    Spelling.erase(www1Pos, 5);
                    Symbol = machine; // Update the symbol to represent the removal of "www1"
                }

                std::cout << "Path: " << Spelling << std::endl;

                if (!removedPart.empty()) {
                    std::cout << "Machine: " << removedPart << std::endl; // Print the removed part
                }
            }
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
