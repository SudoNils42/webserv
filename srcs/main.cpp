#include "../incl/lib.hpp"
#include "../incl/SocketManager.hpp"

int main() {
    SocketManager server(8080);
    server.run();
    return 0;
}