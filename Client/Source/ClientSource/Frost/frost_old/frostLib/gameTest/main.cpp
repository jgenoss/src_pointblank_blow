#include "../gameShieldLib/shieldSecurity.h"

#include <iostream>
#include <string>
#include <windows.h>

static SOCKET connectto(unsigned ip, unsigned port)
{
    SOCKET sc = INVALID_SOCKET;
    try {
        sockaddr_in clnt;
        clnt.sin_family = AF_INET;
        clnt.sin_addr.s_addr = ip;
        clnt.sin_port = htons(port);
        sc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(INVALID_SOCKET == sc) {
            std::cout << "connectto() err 2: " << WSAGetLastError() << std::endl;
            throw 1;
        }
        int err = frostConnect(sc, (SOCKADDR*)&clnt, sizeof(clnt));
        if (0 != err) {
            std::cout << "connectto() err 3: " << WSAGetLastError() << std::endl;
            throw 2;
        }
    } catch (...) {
        closesocket(sc);
        sc = INVALID_SOCKET;
    }
    return sc;
}

static void start_echo_client(const char* ip_str, unsigned short port)
{
    SOCKET sc = connectto(::inet_addr(ip_str), port);
    if(INVALID_SOCKET == sc) {
        std::cout << "failed to connect to " << ip_str << ":" << port << std::endl;
    } else {
        const unsigned buffLen = 1024;
        char buff[buffLen + 1] = {0};
        while (true) {
            std::cin.getline(buff, buffLen);
            std::string str(buff);
            if(str.empty()) continue;
            frostSend(sc, str.c_str(), str.size(), 0);
            int l = frostRecv(sc, buff, buffLen, 0);
            if(l > 0) {
                std::cout << "recv: " << buff << std::endl;
            } else {
                closesocket(sc);
                if(l < 0) {
                    std::cout << "error on socket " << (int)sc << ", err = " <<
                        WSAGetLastError() << std::endl;
                }
                std::cout << "connection closed for " << (int)sc << std::endl;
                break;
            }
        }
    }
}

static const unsigned buffLen = 1024;
static WSABUF rlpBuffers;
static char rbuff[buffLen + 1] = {0};
static WSAOVERLAPPED pOverlapped;

static void CALLBACK myCompletionRoutine(DWORD dwError,
                                         DWORD cbTransferred,
                                         LPWSAOVERLAPPED lpOverlapped,
                                         DWORD dwFlags )
{
    std::cout << "myCompletionRoutine: ";
    if(lpOverlapped) {
        std::cout << cbTransferred << " - ";
        for(DWORD i=0; i<rlpBuffers.len && i < cbTransferred; ++i) {
            std::cout << rlpBuffers.buf[i];
        }
    }
    std::cout << std::endl;
}

static void start_echo_client_async(const char* ip_str, unsigned short port)
{
    SOCKET sc = connectto(::inet_addr(ip_str), port);
    if(INVALID_SOCKET == sc) {
        std::cout << "failed to connect to " << ip_str << ":" << port << std::endl;
    } else {
        WSABUF lpBuffers;
        char buff[buffLen + 1] = {0};
        lpBuffers.buf = buff;
        lpBuffers.len = buffLen;
        DWORD lpNumberOfBytesSent = 0;

        rlpBuffers.buf = rbuff;
        rlpBuffers.len = buffLen;
        ZeroMemory(&pOverlapped, sizeof(WSAOVERLAPPED));

        while (true) {
            std::cin.getline(buff, buffLen);
            std::string str(buff);
            if(str.empty()) continue;
            lpBuffers.len = str.size();

            frostWSASend(sc, &lpBuffers, 1, &lpNumberOfBytesSent, 0, 0, 0);
            DWORD flags = 0;
            DWORD rlpNumberOfBytesRecv = 0;
            int e = 
                frostWSARecv(sc, &rlpBuffers, 1,
                             &rlpNumberOfBytesRecv,
                             &flags,
                             &pOverlapped,
                             myCompletionRoutine);
            if(e) {
                if ((e = WSAGetLastError()) != WSA_IO_PENDING) {
                    std::cout << "error on socket " << (int)sc << ", err = " <<
                        e << std::endl;
                    closesocket(sc);
                    break;
                }
            }
            ::SleepEx(1000, TRUE);
        }
    }
}

static void start_echo_client_udp(const char* ip_str, unsigned short port)
{
    SOCKET sc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(INVALID_SOCKET == sc) {
        std::cout << "failed to connect to " << ip_str << ":" << port << std::endl;
    } else {
        sockaddr_in srv;
        srv.sin_family = AF_INET;
        srv.sin_port   = htons(port);
        srv.sin_addr.s_addr = ::inet_addr(ip_str);

        char buff[buffLen + 1] = {0};
        while(true){
            std::cin.getline(buff, buffLen);
            std::string str(buff);
            if(str.empty()) continue;
            int s = sizeof(srv);
            frostSendto(sc, str.c_str(), str.size(), 0, (SOCKADDR*) &srv, s);
            s = sizeof(srv);
            int l = frostRecvfrom(sc, buff, buffLen, 0, (SOCKADDR*)&srv, &s);
            if(l > 0) {
                std::cout << "recvFrom: " << buff << std::endl;
            } else if (l <0) {
                std::cout << "error in recvFrom: " << WSAGetLastError() << std::endl;
            } else {
                std::cout << "0 in recvFrom" << std::endl;
            }
        }
    }
}

int main()
{
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
        std::cout << "WSAStartup failed" << std::endl;
        WSACleanup();
        return 0;
    }

	const char * ip = "81.177.139.75";
	unsigned short port = 39190;


    frostInitialize("../../gameShieldDll.dll");

    start_echo_client(ip, port);

    //start_echo_client_async(ip, port);

    //start_echo_client_udp(ip, port);

    return 0;
}

