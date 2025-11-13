#include "../incl/lib.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>  // atoi
#include <unistd.h> // write, close
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// === VARIABLES GLOBALES (ultra-simple) ===
int         g_port = 8080;
std::string g_root = "./frontend";
std::string g_index = "home.html";

// === FONCTIONS UTILITAIRES C++98 ===
void trim(std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) s = "";
    else s = s.substr(start, end - start + 1);
}

// to_string() en C++98
std::string to_string(size_t n) {
    std::ostringstream oss;
    oss << n;
    return oss.str();
}

// === PARSER MINIMAL ===
void parserConfig(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open config file: " << filepath << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string key, value;

        iss >> key;
        if (key == "listen") {
            iss >> value;
            if (!value.empty() && value[value.size() - 1] == ';')
                value.erase(value.size() - 1);  // ← C++98
            g_port = std::atoi(value.c_str());
        }
        else if (key == "root") {
            iss >> value;
            if (!value.empty() && value[value.size() - 1] == ';')
                value.erase(value.size() - 1);
            g_root = value;
        }
        else if (key == "index") {
            iss >> value;
            if (!value.empty() && value[value.size() - 1] == ';')
                value.erase(value.size() - 1);
            g_index = value;
        }
    }
    file.close();
}

// === MAIN ===
int main(int ac, char **av) {
    if (ac != 2) {
        std::cerr << "Error: configuration file needed" << std::endl;
        return 1;
    }
    parserConfig(av[1]);

    // --- SOCKET ---
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(g_port);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); exit(1);
    }
    if (listen(server_fd, 10) < 0) {
        perror("listen"); exit(1);
    }

    std::cout << "Webserv running on http://localhost:" << g_port << std::endl;

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) { perror("accept"); continue; }

        // --- LIRE FICHIER ---
        std::string filepath = g_root + "/" + g_index;
        std::ifstream file(filepath.c_str());
        std::string content;
        if (file.is_open()) {
            std::ostringstream oss;
            oss << file.rdbuf();
            content = oss.str();
            file.close();
        } else {
            content = "<h1>404 - File not found</h1>";
        }

        // --- RÉPONSE HTTP (C++98) ---
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << content.size() << "\r\n";
        response << "\r\n";
        response << content;

        std::string resp_str = response.str();
        write(client_fd, resp_str.c_str(), resp_str.size());
        close(client_fd);
    }

    close(server_fd);
    return 0;
}