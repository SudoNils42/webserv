#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP
#include "lib.hpp"

class SocketManager {
    private:
        int _server_fd;
        std::vector<pollfd> _fds;

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