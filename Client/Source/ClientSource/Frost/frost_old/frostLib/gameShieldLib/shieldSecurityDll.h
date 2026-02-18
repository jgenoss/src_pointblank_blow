#ifndef GAME_SHIELD_LIB_SHIELDSECURITYDLL_H
#define GAME_SHIELD_LIB_SHIELDSECURITYDLL_H

#include <winsock2.h>
#include <windows.h>

class shieldSecurityDll
{
public:
    static bool init(const char* dllPath);

    static void setUserName(const char*);

    static int connect(SOCKET s,
                       const struct sockaddr* name,
                       int namelen);

    static int send(SOCKET s,
                    const char* buf,
                    int len,
                    int flags);

    static int recv(SOCKET s,
                    char* buf,
                    int len,
                    int flags);

    static int WSASend(SOCKET s,
                       LPWSABUF lpBuffers,
                       DWORD dwBufferCount,
                       LPDWORD lpNumberOfBytesSent,
                       DWORD dwFlags,
                       LPWSAOVERLAPPED lpOverlapped,
                       LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

    static int WSARecv(SOCKET s,
                       LPWSABUF lpBuffers,
                       DWORD dwBufferCount,
                       LPDWORD lpNumberOfBytesRecvd,
                       LPDWORD lpFlags,
                       LPWSAOVERLAPPED lpOverlapped,
                       LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

    static int sendto(SOCKET s,
                      const char* buf,
                      int len,
                      int flags,
                      const struct sockaddr* to,
                      int tolen);

    static int recvfrom(SOCKET s,
                        char* buf,
                        int len,
                        int flags,
                        struct sockaddr* from,
                        int* fromlen);

public:
    ~shieldSecurityDll();
private:
    void (*m_pSetUserName)(const char*);

    int (*m_pConnect)(SOCKET,
                      const struct sockaddr*,
                      int);

    int (*m_pSend)(SOCKET,
                   const char*,
                   int,
                   int);

    int (*m_pRecv)(SOCKET,
                   char*,
                   int,
                   int);

    int (*m_pWSASend)(SOCKET,
                      LPWSABUF,
                      DWORD,
                      LPDWORD,
                      DWORD,
                      LPWSAOVERLAPPED,
                      LPWSAOVERLAPPED_COMPLETION_ROUTINE);

    int (*m_pWSARecv)(SOCKET,
                      LPWSABUF,
                      DWORD,
                      LPDWORD,
                      LPDWORD,
                      LPWSAOVERLAPPED,
                      LPWSAOVERLAPPED_COMPLETION_ROUTINE);

    int (*m_pSendto)(SOCKET,
                     const char*,
                     int,
                     int,
                     const struct sockaddr*,
                     int);

    int (*m_pRecvfrom)(SOCKET,
                       char*,
                       int,
                       int,
                       struct sockaddr*,
                       int*);

private:
    static shieldSecurityDll& get(const char* dllPath = 0);

private:
    shieldSecurityDll(const char* dllPath);
private:
    HMODULE m_lib;
private:
    shieldSecurityDll(const shieldSecurityDll&);
    shieldSecurityDll& operator=(const shieldSecurityDll&);
};

#endif //GAME_SHIELD_LIB_SHIELDSECURITYDLL_H

