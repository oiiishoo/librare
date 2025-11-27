#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <unordered_map>
#include<string>
#include <chrono>
#include<thread>
#pragma comment(lib, "ws2_32.lib")
bool InitWSA() {
    WSADATA wsa;
    int res = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (res != 0) {
        std::cerr << "WSAStartup failed: " << res << "\n";
        return false;
    }
    std::cout << "[WSA] Initialized (Winsock "
        << LOBYTE(wsa.wVersion) << "." << HIBYTE(wsa.wVersion) << ")\n";
    return true;
}

void ClearWSA() {
    int res = WSACleanup();
    if (res != 0) {
        std::cerr << "WSACleanup failed: " << WSAGetLastError() << "\n";
    }
    else {
        std::cout << "[WSA] Cleaned up\n";
    }
}
class UDPSock {
public:
    SOCKET sock;
    sockaddr_in6 remote, sender;
    int senderLen = sizeof(sender);
    DWORD timeoutMs;

    UDPSock() : sock(INVALID_SOCKET), timeoutMs(0) {}

    bool init(const char* ip, short port) {
        clear();

        sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == INVALID_SOCKET) {
            std::cerr << "socket() failed: " << WSAGetLastError() << "\n";
            return false;
        }

        // IPv4 + IPv6
        DWORD off = 0;
        setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&off, sizeof(off));

        memset(&remote, 0, sizeof(remote));
        remote.sin6_family = AF_INET6;
        remote.sin6_port = htons(port);
        // Попытка как IPv6
        if (inet_pton(AF_INET6, ip, &remote.sin6_addr) == 1)
            return true;

        // Попытка как IPv4 → преобразуем в IPv4-mapped IPv6
        in_addr ipv4;
        if (inet_pton(AF_INET, ip, &ipv4) == 1) {
            // ::ffff:a.b.c.d
            memset(remote.sin6_addr.s6_addr, 0, 10);
            remote.sin6_addr.s6_addr[10] = 0xff;
            remote.sin6_addr.s6_addr[11] = 0xff;
            memcpy(remote.sin6_addr.s6_addr + 12, &ipv4, 4);
            return true;
        }

        return false;
    }

    bool serve(short port, DWORD timeout = 0) {
        clear();

        sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == INVALID_SOCKET) {
            std::cerr << "socket() failed: " << WSAGetLastError() << "\n";
            return false;
        }

        // IPv4 + IPv6
        DWORD off = 0;
        setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&off, sizeof(off));

        if (bindport(port) == SOCKET_ERROR) {
            return false;
        }
        if(timeout)
            setTimeout(timeout);

        std::cout << "[UDP] Listening on port " << (unsigned short)port << '\n';
        return true;
    }

    int sendToRemote(const char* data, int len) {
        return sendto(sock, data, len, 0, (sockaddr*)&remote, sizeof(remote));
    }

    int recvFromRemote(char* buf, int len, sockaddr_in6* fromAddr6=0) {
        if(!fromAddr6)
            return recvfrom(sock, buf, len, 0, (sockaddr*)&remote, &senderLen);

        return recvfrom(sock, buf, len, 0, (sockaddr*)fromAddr6, &senderLen);
    }
    int sendToAddr(char* data, int len, sockaddr_in6& Addr6) {
        return sendto(sock, data, len, 0, (sockaddr*)&Addr6, sizeof(remote));
    }
    int recvFromAddr(char* buf, int len, sockaddr_in6& fromAddr6) {
        return recvfrom(sock, buf, len, 0, (sockaddr*)&fromAddr6, &senderLen);
    }
    int recvFromAll(char* buf, int len, sockaddr_in6&receivedAddr6) {
        receivedAddr6.sin6_addr = IN6_ADDR{};
        receivedAddr6.sin6_port = 0;
        receivedAddr6.sin6_family = AF_INET6;
        return recvfrom(sock, buf, len, 0, (sockaddr*)&receivedAddr6, &senderLen);
    }
    
    int bindport(short port) {
        // Биндим порт
        sender.sin6_family = AF_INET6;
        sender.sin6_addr = in6addr_any;
        sender.sin6_port = htons(port);

        if (bind(sock, (sockaddr*)&sender, sizeof(sender)) == SOCKET_ERROR) {
            std::cerr << "bind() failed: " << WSAGetLastError() << "\n";
            closesocket(sock);
            sock = INVALID_SOCKET;
            return SOCKET_ERROR;
        }
        return 0;
    }


    void setRemote(const char* ip, short port) {
        memset(&remote, 0, sizeof(remote));
        remote.sin6_family = AF_INET6;
        remote.sin6_port = htons(port);

        in_addr ipv4;
        if (inet_pton(AF_INET6, ip, &remote.sin6_addr) != 1) {
            // Попытка IPv4 → преобразуем в IPv4-mapped IPv6
            if (inet_pton(AF_INET, ip, &ipv4) == 1) {
                memset(remote.sin6_addr.s6_addr, 0, 10);
                remote.sin6_addr.s6_addr[10] = 0xff;
                remote.sin6_addr.s6_addr[11] = 0xff;
                memcpy(remote.sin6_addr.s6_addr + 12, &ipv4, 4);
            }
        }
    }

    bool setTimeout(DWORD timeout) {
        timeoutMs = timeout;
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
            return false;
        if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
            return false;
        return true;
    }

    void clear() {
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
            sock = INVALID_SOCKET;
        }
        memset(&remote, 0, sizeof(remote));
        memset(&sender, 0, sizeof(sender));
        senderLen = sizeof(sender);
        timeoutMs = 0;
    }

    ~UDPSock() {
        if (sock != INVALID_SOCKET)
            closesocket(sock);
    }
};

struct Addr {
    IN6_ADDR addr6;
    short port;
    Addr(IN6_ADDR _addr6, short _port) :addr6(_addr6),port(_port) {}
};

std::string ipv6ToString(const sockaddr_in6& addr) {
    char buf[INET6_ADDRSTRLEN] = {};
    inet_ntop(AF_INET6, (void*)&addr.sin6_addr, buf, sizeof(buf));

    uint16_t port = ntohs(addr.sin6_port);

    std::string ip(buf);

    // IPv4‑mapped (::ffff:x.x.x.x) — выводим красиво
    if (ip.rfind("::ffff:", 0) == 0) {
        return ip.substr(7) + ":" + std::to_string(port);
    }

    return "[" + ip + "]:" + std::to_string(port);
}
