#include "../incl/lib.hpp"
#include "../incl/SocketManager.hpp"

SocketManager::SocketManager(int port) {
    setupServerSocket(port);
    struct pollfd server_poll;
    server_poll.fd = _server_fd;
    server_poll.events = POLLIN;
    _fds.push_back(server_poll);
    _body_parts.push_back("");
    std::cout << "server listening on port " << port << std::endl;
}

SocketManager::~SocketManager() {
    for (size_t i = 0; i < _fds.size(); i++)
        close(_fds[i].fd);
}

void SocketManager::setupServerSocket(int port) {
    _server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    int opt = 1;
    setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(_server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(_server_fd, SOMAXCONN);
}

void SocketManager::addClient(int client_fd) {
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    struct pollfd client_poll;
    client_poll.fd = client_fd;
    client_poll.events = POLLIN;
    _fds.push_back(client_poll);
    _body_parts.push_back("");
}

void SocketManager::removeClient(size_t i) {
    close(_fds[i].fd);
    _fds.erase(_fds.begin() + i);
    _body_parts.erase(_body_parts.begin() + i);
}

void SocketManager::handleNewConnection() {
    int client_fd = accept(_server_fd, NULL, NULL);
    addClient(client_fd);
}

void SocketManager::handleClientData(int client_fd) {
    char buf[16];
    size_t i;
    for (i = 1; i < _fds.size(); i++)
        if (_fds[i].fd == client_fd)
            break;

    ssize_t r = 1;
    while (r > 0)
    {
        r = read(client_fd, buf, 15);
        if (r > 0)
            _body_parts[i].append(buf, r);
    }

    if (_body_parts[i].find("\r\n\r\n") == std::string::npos)
        return;

    if (_body_parts[i].find("Content-Length: ") != std::string::npos && _body_parts[i].find("\r\n\r\n") != std::string::npos)
    {
        size_t cl = _body_parts[i].find("Content-Length: ") + 16;
        size_t body_len = atoi(_body_parts[i].c_str() + cl);
        size_t body_start = _body_parts[i].find("\r\n\r\n") + 4;
        if (_body_parts[i].size() - body_start < body_len)
            return;
    }

    std::string body = "<h1>Salut bg !</h1>";
    std::string resp =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + itos(body.size()) + "\r\n"
        "Connection: close\r\n\r\n" + body;
    
    write(client_fd, resp.c_str(), resp.size());
    for (size_t i = 1; i < _fds.size(); i++)
    {
        if (_fds[i].fd == client_fd)
        {
            removeClient(i);
            break;
        }
    }
}

void SocketManager::run() {
    while (1)
    {
        poll(&_fds[0], _fds.size(), -1);

        for (size_t i = 0; i < _fds.size(); i++)
        {
            if (_fds[i].revents == POLLIN)
            {
                if (_fds[i].fd == _server_fd)
                    handleNewConnection();
                else
                    handleClientData(_fds[i].fd);
            }
        }
    }
}

std::string SocketManager::itos(int value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
}