#include <iostream>
#include <unordered_set>
#include <vector>
#include <functional>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <array>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#include <thread>

static const uint16_t PORT = 53;

static std::unordered_set<uint32_t> ips;

static std::vector<std::thread> threads;

int thread_recv(const int sock_fd);

int main(int argc, char **argv) {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd < 0) {
        std::cerr << "Error openning socket: " << strerror(errno) << std::endl;
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

    std::cout << "Socket bound" << std::endl;
    for (int i = 0; i < 4; i++) {
        threads.push_back(std::thread([sock_fd](){thread_recv(sock_fd);}));
    }

    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
}

int thread_recv(const int sock_fd) {
    std::array<uint8_t, 1024> buffer;
    struct sockaddr_in src_addr = {};
    socklen_t src_addr_len;

    while (true) {
        int recv_len = recvfrom(sock_fd, buffer.data(), buffer.size(), 0, (struct sockaddr *) &src_addr, &src_addr_len);
        if (recv_len == -1) {
            std::cerr << "Error recvfrom() " << strerror(errno) << std::endl;
        }

        ips.insert(src_addr.sin_addr.s_addr);
        std::cout << "Unique IPs: " << ips.size() << std::endl;
    }
}
