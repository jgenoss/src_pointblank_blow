========================================================================
    STATIC LIBRARY : gameShieldLib Project Overview
========================================================================

to applay frost on game at first functions declared in "shieldSecurity.h" 
should be used in gameClient, they are wrappers on api fntionas.


1. void frostInitialize(const char* dllPath);

2. void frostSetUserName(const char* uin);

3. int frostConnect(SOCKET s,
                    const struct sockaddr* name,
                    int namelen);

4. int frostSend(SOCKET s,
                 const char* buf,
                 int len,
                 int flags);

5. int frostRecv(SOCKET s,
                 char* buf,
                 int len,
                 int flags);

6. int frostWSASend(SOCKET s,
                    LPWSABUF lpBuffers,
                    DWORD dwBufferCount,
                    LPDWORD lpNumberOfBytesSent,
                    DWORD dwFlags,
                    LPWSAOVERLAPPED lpOverlapped,
                    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

7. int frostWSARecv(SOCKET s,
                    LPWSABUF lpBuffers,
                    DWORD dwBufferCount,
                    LPDWORD lpNumberOfBytesRecvd,
                    LPDWORD lpFlags,
                    LPWSAOVERLAPPED lpOverlapped,
                    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

8. int frostSendto(SOCKET s,
                   const char* buf,
                   int len,
                   int flags,
                   const struct sockaddr* to,
                   int tolen);

9. int frostRecvfrom(SOCKET s,
                     char* buf,
                     int len,
                     int flags,
                     struct sockaddr* from,
                     int* fromlen);


