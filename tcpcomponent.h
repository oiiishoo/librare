#pragma once
#include<iostream>
//#include<windows.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
static char v6ds[] = {
        0,0,0,0, 0,0,0,0, 0,0, 255,255
};
static bool isUpWSA = 0;

bool initWSA() {
	if (!isUpWSA) {
		WSADATA wsa;
		if (WSAStartup(514, &wsa)) {
			std::cerr << "WSAStartup failed" << std::endl;
			return 0;
		}
		isUpWSA = 1;
	}
	return 1;
}

char* iptextV6(const sockaddr_in6& addr6) {
    char* buf = (char*)malloc(INET6_ADDRSTRLEN);
    if (!buf) return 0;

    if (!inet_ntop(AF_INET6, &addr6.sin6_addr, buf, INET6_ADDRSTRLEN)) {
        free(buf);
        return 0;
    }

    return buf;
}
char* iptext(sockaddr_in6& addr6) {
    char* buf = (char*)malloc(60);
    if (!buf) return 0;
    if (memcmp(addr6.sin6_addr.s6_addr, v6ds, 12) == 0) {
        in_addr addr4;
        memcpy(&addr4.s_addr, addr6.sin6_addr.s6_addr + 12, 4);
        return (char*)inet_ntop(AF_INET, &addr4, buf, 40);
    }

    return (char*)inet_ntop(AF_INET6, &addr6.sin6_addr, buf, 40);
}
void set_socket_read_timeout(SOCKET sock, DWORD timeout_ms) {
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_ms, sizeof(timeout_ms));
}
void set_socket_write_timeout(SOCKET sock, DWORD timeout_ms) {
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout_ms, sizeof(timeout_ms));
}

void set_socket_timeout(SOCKET sock, DWORD timeout_ms) {
    set_socket_read_timeout(sock, timeout_ms);
    set_socket_write_timeout(sock, timeout_ms);
}

const char* wsa_error_string(int err) {
    switch (err) {
    case WSAEINTR: return "Interrupted function call";
    case WSAEBADF: return "Bad file descriptor";
    case WSAEACCES: return "Permission denied";
    case WSAEFAULT: return "Bad address";
    case WSAEINVAL: return "Invalid argument";
    case WSAEMFILE: return "Too many open sockets";

    case WSAEWOULDBLOCK: return "Resource temporarily unavailable (would block)";
    case WSAEINPROGRESS: return "Operation now in progress";
    case WSAEALREADY: return "Operation already in progress";
    case WSAENOTSOCK: return "Not a socket";
    case WSAEDESTADDRREQ: return "Destination address required";
    case WSAEMSGSIZE: return "Message too long";
    case WSAEPROTOTYPE: return "Protocol wrong type for socket";
    case WSAENOPROTOOPT: return "Bad protocol option";
    case WSAEPROTONOSUPPORT: return "Protocol not supported";
    case WSAESOCKTNOSUPPORT: return "Socket type not supported";
    case WSAEOPNOTSUPP: return "Operation not supported";
    case WSAEPFNOSUPPORT: return "Protocol family not supported";
    case WSAEAFNOSUPPORT: return "Address family not supported";
    case WSAEADDRINUSE: return "Address already in use";
    case WSAEADDRNOTAVAIL: return "Cannot assign requested address";

    case WSAENETDOWN: return "Network is down";
    case WSAENETUNREACH: return "Network is unreachable";
    case WSAENETRESET: return "Network dropped connection on reset";
    case WSAECONNABORTED: return "Software caused connection abort";
    case WSAECONNRESET: return "Connection reset by peer";
    case WSAENOBUFS: return "No buffer space available";
    case WSAEISCONN: return "Socket is already connected";
    case WSAENOTCONN: return "Socket is not connected";
    case WSAESHUTDOWN: return "Cannot send after socket shutdown";
    case WSAETIMEDOUT: return "Connection timed out";
    case WSAECONNREFUSED: return "Connection refused";
    case WSAEHOSTDOWN: return "Host is down";
    case WSAEHOSTUNREACH: return "No route to host";

    case WSASYSNOTREADY: return "Network subsystem is unavailable";
    case WSAVERNOTSUPPORTED: return "Winsock.dll version out of range";
    case WSAEPROCLIM: return "Too many processes";
    case WSANOTINITIALISED: return "WSAStartup not called";

    default: return "Unknown WSA error";
    }
}
/// Создать клиентское подключение.
/// host, port — куда подключаться.
/// localPort — опционально, какой локальный порт использовать.
SOCKET newConnection(const char* host, const char* port, const char* localPort = 0) {
    if (!initWSA()) return INVALID_SOCKET;

    addrinfo hints{}, * res = 0;
    hints.ai_family = AF_INET6;    
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_protocol = IPPROTO_TCP;

    int err = getaddrinfo(host, port, &hints, &res);
    if (err != 0) {
        std::cerr << "getaddrinfo failed: " << gai_strerrorA(err) << '\n';
        return INVALID_SOCKET;
    }

    SOCKET sock = INVALID_SOCKET;

    for (auto p = res; p != 0; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == INVALID_SOCKET) continue;

        // IPv6 dual stack (разрешить IPv4-mapped адреса)
        if (p->ai_family == AF_INET6) {
            DWORD off = 0;
            setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&off, sizeof(off));
        }

        // биндим локальный порт, если указан
        if (localPort) {
            addrinfo lhints{}, * lres = 0;
            lhints.ai_family = p->ai_family;
            lhints.ai_socktype = SOCK_STREAM;
            lhints.ai_protocol = IPPROTO_TCP;
            lhints.ai_flags = AI_PASSIVE;

            if (getaddrinfo(0, localPort, &lhints, &lres) == 0) {
                if (bind(sock, lres->ai_addr, (int)lres->ai_addrlen) != 0) {
                    std::cerr << "bind(localPort) failed: " << WSAGetLastError() << '\n';
                    closesocket(sock);
                    sock = INVALID_SOCKET;
                    freeaddrinfo(lres);
                    continue;
                }
                freeaddrinfo(lres);
            }
        }

        if (connect(sock, p->ai_addr, (int)p->ai_addrlen) == 0) {
            break; // успешное подключение
        }

        closesocket(sock);
        sock = INVALID_SOCKET;
    }

    freeaddrinfo(res);

    if (sock == INVALID_SOCKET) {
        std::cerr << "❌ Connection failed" << '\n';
    }
    return sock;
}

/// Создать TCP сервер.
/// port — локальный порт для прослушивания.
/// reuseAddr — включить SO_REUSEADDR.
/// backlog — размер очереди (по умолчанию 1024).
SOCKET newServer(const char* port, bool reuseAddr = 0, int backlog = 1024) {
    if (!initWSA()) return INVALID_SOCKET;

    addrinfo hints{}, * res = 0;
    hints.ai_family = AF_INET6;     // IPv4 или IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;     // для bind

    int err = getaddrinfo(0, port, &hints, &res);
    if (err != 0) {
        std::cerr << "getaddrinfo failed: " << gai_strerrorA(err) << '\n';
        return INVALID_SOCKET;
    }

    SOCKET sock = INVALID_SOCKET;

    for (auto p = res; p != 0; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == INVALID_SOCKET) continue;

        if (reuseAddr) {
            int opt = 1;
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
        }

        // IPv6 dual stack
        if (p->ai_family == AF_INET6) {
            DWORD off = 0;
            setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&off, sizeof(off));
        }

        if (bind(sock, p->ai_addr, (int)p->ai_addrlen) == 0) {
            if (!listen(sock, backlog)) {
                break; // готовый сервер
            }
        }

        closesocket(sock);
        sock = INVALID_SOCKET;
    }

    freeaddrinfo(res);

    if (sock == INVALID_SOCKET) {
        std::cerr << "❌ Failed to create server on port " << port << '\n';
    }
    return sock;
}
bool rst(SOCKET sockfd) {
    if (sockfd == INVALID_SOCKET) return 1;

    // Установим "abortive close" через linger = 0
    linger lg;
    lg.l_onoff = 1;   // включить linger
    lg.l_linger = 0;  // 0 секунд => RST при close
    if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (char*)&lg, sizeof(lg)) == SOCKET_ERROR) {
        int e = WSAGetLastError();
        std::cerr << "setsockopt(SO_LINGER) failed: " << wsa_error_string(e) << '\n';
        // тем не менее попробуем закрыть socket
    }

    // Закрываем сокет (будет отправлен RST из-за linger)
    if (closesocket(sockfd) == SOCKET_ERROR) {
        int e = WSAGetLastError();
        std::cerr << "closesocket failed: " << wsa_error_string(e) << '\n';
        return 0;
    }
    
    return 1;
}
void fin(SOCKET sock) {
    shutdown(sock, SD_SEND);  
    closesocket(sock);
}

