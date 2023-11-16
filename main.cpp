#include <iostream>
#include <cctype>
#include <fstream>

enum TSymbolType {
    protocol, domain, port, path, colon, period, quotas, othersy, semicolon
};

char Char;
TSymbolType Symbol;
char Spelling[256];
const int MAXLENGTH = 255;

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
    int k = 0;

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
            while (isalpha(Char) || isdigit(Char) || Char == '.') {
                Spelling[k] = Char;
                k++;
                GetNextChar();
            }

            Spelling[k] = '\0';

            if (k > MAXLENGTH) {
                error("error: URL is too long");
            }

            if (Spelling[k - 1] == '.') {
                Symbol = domain;
                std::cout << "Domain: " << Spelling << std::endl;
            } else {
                Symbol = protocol;
                std::cout << "Protocol: " << Spelling << std::endl;
            }

        } break;
        case '0' ... '9': {
            while (isdigit(Char)) {
                Spelling[k] = Char;
                k++;
                GetNextChar();
            }

            Spelling[k] = '\0';

            if (k > MAXLENGTH) {
                error("error: Port is too long");
            }

            Symbol = port;
            std::cout << "Port: " << Spelling << std::endl;
        } break;
        case '/' : {
            Symbol = path;
            GetNextChar();
            int insidePath = 0;
            while (Char != '\0' && Char != ';' && Char != '\"' && Char != '\n') {
                Spelling[insidePath] = Char;
                insidePath++;
                GetNextChar();
            }
            Spelling[insidePath] = '\0';
            std::cout << "Path: " << Spelling << std::endl;
        } break;
        case ':' : {
            Symbol = colon;
            GetNextChar();
            std::cout << "Column" << std::endl;

            // Skip the colon and whitespace if present
            while (Char == ':' || Char == ' ') {
                GetNextChar();
            }

            // Move to the next valid part of the URL
            while (Char != '\0' && Char != ';' && Char != '\"' && Char != '\n') {
                Spelling[k] = Char;
                k++;
                GetNextChar();
            }
            Spelling[k] = '\0';
            std::cout << "Path: " << Spelling << std::endl;
        } break;
        case ';' : {
            Symbol = semicolon;
            GetNextChar();
            std::cout << "Semicolon" << std::endl;

            // Read until the end of the line or end of the file for the second URL's path
            while (Char != '\n' && Char != EOF) {
                GetNextChar();
            }
        } break;
        case '\"' : {
            Symbol = quotas;
            GetNextChar();
            int insideQuote = 0;
            while (Char != '\"') {
                Spelling[insideQuote] = Char;
                insideQuote++;
                GetNextChar();
            }
            if (Char == '\"') {
                GetNextChar();
            } else {
                error("error: URL is too long or missing closing double quote");
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
