#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

struct URLComponents {
    std::string scheme;
    std::string user;
    std::string password;
    std::string host;
    std::string port;
    std::string path;
};

void parseURL(const std::string& urlString) {
    URLComponents components;

    size_t schemePos = urlString.find("://");
    if (schemePos != std::string::npos) {
        components.scheme = urlString.substr(0, schemePos);
        size_t hostStartPos = schemePos + 3;

        size_t userEndPos = urlString.find(':', hostStartPos);
        size_t hostEndPos = urlString.find('@', hostStartPos);
        size_t portStartPos = urlString.find(':', hostStartPos);
        size_t pathStartPos = urlString.find('/', schemePos + 3);

        if (components.scheme != "http" && components.scheme != "https" && userEndPos != std::string::npos && (hostEndPos == std::string::npos || userEndPos < hostEndPos)) {
            components.user = urlString.substr(hostStartPos, userEndPos - hostStartPos);
            components.password = urlString.substr(userEndPos + 1, hostEndPos - userEndPos - 1);
            components.host = urlString.substr(hostEndPos + 1, (portStartPos != std::string::npos ? portStartPos : pathStartPos) - hostEndPos - 1);
        } else {
            components.host = urlString.substr(hostStartPos, (portStartPos != std::string::npos ? portStartPos : pathStartPos) - hostStartPos);
        }

        if (portStartPos != std::string::npos && portStartPos < pathStartPos) {
            components.port = urlString.substr(portStartPos + 1, pathStartPos - portStartPos - 1);
        }

        if (pathStartPos != std::string::npos) {
            components.path = urlString.substr(pathStartPos);
        }
    }

    std::cout << "Scheme: " << components.scheme << std::endl;
    if (components.scheme != "http" && components.scheme != "https") {
        std::cout << "User: " << components.user << std::endl;
        std::cout << "Password: " << components.password << std::endl;
    }
    std::cout << "Host: " << components.host << std::endl;
    std::cout << "Port: " << (components.port.empty() ? "default" : components.port) << std::endl;
    std::cout << "Path: " << components.path << std::endl;
}

int main() {
    std::ifstream file("/Users/moldovexc/CLionProjects/parserurl/input.txt");
    if (!file.is_open()) {
        std::cout << "Unable to open file!" << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        parseURL(line);
        std::cout << "-----------------------------" << std::endl;
    }

    file.close();
    return 0;
}
