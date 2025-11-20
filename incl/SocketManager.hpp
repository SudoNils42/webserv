#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP
#include "lib.hpp"

class SocketManager {
    private:
        int _server_fd;
        std::vector<pollfd> _fds;
        std::vector<std::string> _body_parts;

    public:
        SocketManager(int port);
        ~SocketManager();

        void addClient(int client_fd);
        void removeClient(size_t index);
        void setupServerSocket(int port);

        void run();
        void handleNewConnection();
        void handleClientData(int client_fd);

        std::string itos(int value);
};

#endif