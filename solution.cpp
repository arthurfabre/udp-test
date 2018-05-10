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
#include <chrono>
#include <tuple>

static const uint16_t PORT = 53;

static std::vector<std::pair<std::thread, std::shared_ptr<std::unordered_set<uint32_t>>>> threads;

int thread_recv(std::shared_ptr<std::unordered_set<uint32_t>> ips);

int main(int argc, char **argv) {
    for (int i = 0; i < 4; i++) {
        std::shared_ptr<std::unordered_set<uint32_t>> ips(new std::unordered_set<uint32_t>());
        threads.push_back(std::make_pair(std::thread([ips](){thread_recv(ips);}), ips));
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        int unique_ips = 0;
        for (int i = 0; i < threads.size(); i++) {
            unique_ips += threads[i].second->size();
        }

        std::cout << "Unique IPs: " << unique_ips << std::endl;
    }

    for (int i = 0; i < threads.size(); i++) {
        threads[i].first.join();
    }
}

int thread_recv(std::shared_ptr<std::unordered_set<uint32_t>> ips) {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd < 0) {
        std::cerr << "Error openning socket: " << strerror(errno) << std::endl;
        return -1;
    }

    int reuse_addr = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reuse_addr, sizeof(reuse_addr)) == -1) {
        std::cerr << "Error setting REUSE_ADDR: " << strerror(errno) << std::endl;
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

    std::array<uint8_t, 1024> buffer;
    struct sockaddr_in src_addr = {};
    socklen_t src_addr_len;

    while (true) {
        int recv_len = recvfrom(sock_fd, buffer.data(), buffer.size(), 0, (struct sockaddr *) &src_addr, &src_addr_len);
        if (recv_len == -1) {
            std::cerr << "Error recvfrom() " << strerror(errno) << std::endl;
        }

        ips->insert(src_addr.sin_addr.s_addr);
    }
}
