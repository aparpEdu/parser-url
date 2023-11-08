#include <iostream>
#include <cctype>

enum TSymbolType {
    protocol, domain, port, path, semicolon, period, quotas, othersy
};

char Char;
TSymbolType Symbol;
char Spelling[256];
const int MAXLENGTH = 255;

const char* Input = R"(https://www.example.com:8080/path/to/resource;https://otherwebsite.com/)";
int inputIndex = 0;

void GetNextChar() {
    Char = Input[inputIndex];
    inputIndex++;
}

void error(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
}

void GetNextSymbol() {
    int k = 0;

    while (Char != '\0' && !isalpha(Char) && Char != ':' && Char != '.' && Char != '/' && Char != ';' && Char != '\"') {
        GetNextChar();
    }

    if (Char == '\0') {
        Symbol = othersy;
        std::cout << "End of Input" << std::endl;
        return;
    }

    while (Char == ' ') {
        GetNextChar();
    }

    switch (toupper(Char)) {
        case 'A' ... 'Z': {
            while (isalpha(Char)) {
                Spelling[k] = Char;
                k++;
                GetNextChar();
            }

            Spelling[k] = '\0';

            if (k > MAXLENGTH) {
                error("error: URL is too long");
            }

            Symbol = protocol;
            std::cout << "Protocol: " << Spelling << std::endl;
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
        case '.' : {
            Symbol = period;
            GetNextChar();
            std::cout << "Period" << std::endl;
        } break;
        case '/' : {
            Symbol = path;
            GetNextChar();
            int insidePath = 0;
            while (Char != '\0' && Char != ';' && Char != '\"') {
                Spelling[insidePath] = Char;
                insidePath++;
                GetNextChar();
            }
            Spelling[insidePath] = '\0';
            std::cout << "Path: " << Spelling << std::endl;
        } break;
        case ':' : {
            Symbol = semicolon;
            GetNextChar();
            std::cout << "Semicolon" << std::endl;
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

//int expect(TSymbolType symbol) {
//    if (accept(symbol)) {
//        return 1;
//    }
//    error("expect: unexpected symbol");
//    return 0;
//}

void Field() {
    if (accept(protocol) || accept(port) || accept(quotas) || accept(path) || accept(domain)) {
        return;
    } else if (Symbol != othersy) {
        error("field: expects valid URL components");
    }
}

void Record() {
    Field();
    while (Symbol == period) {
        accept(period);
        Field();
    }
//    expect(semicolon);
}

void DataFile() {
    while (Symbol != othersy) {
        Record();
        while (accept(semicolon)) {
            std::cout << "Semicolon" << std::endl;
            while (Char == ';') {
                GetNextChar();
            }
        }
    }
}

int main() {
    GetNextChar();
    GetNextSymbol();
    DataFile();
    return 0;
}
