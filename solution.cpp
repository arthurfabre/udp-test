#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

static const uint16_t PORT = 53;

int main(int argc, char **argv) {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd < 0) {
        std::cerr << "Error openning socket: " << sock_fd << std::endl;
        return -1;
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(PORT);

    if (bind(sock_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        std::cerr << "Error binding socket " << strerror(errno) << std::endl;

        return -1;
    }

    std::cout << "Bind() socket" << std::endl;

    while (true) {
        


    }
}
