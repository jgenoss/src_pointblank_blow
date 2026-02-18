#include "pch.h"
#include "shieldSecurityDll.h"



shieldSecurityDll::shieldSecurityDll(const char* dllPath):
    m_lib(0),
    m_pSetUserName(0),
    m_pConnect(0),
    m_pSend(0),
    m_pRecv(0),
    m_pWSASend(0),
    m_pWSARecv(0),
    m_pSendto(0),
    m_pRecvfrom(0)
{
    m_lib = LoadLibraryA(dllPath ? dllPath : "gameShieldDll.dll");
    if(m_lib) {
        m_pSetUserName = (void (*)(const char*))
            GetProcAddress(m_lib, "frostSetUserName");

        m_pConnect = (int (*)(SOCKET, const struct sockaddr*, int))
            GetProcAddress(m_lib, "frostConnect");

        m_pSend = (int (*)(SOCKET, const char*, int, int))
            GetProcAddress(m_lib, "frostSend");

        m_pRecv = (int (*)(SOCKET, char*, int, int))
            GetProcAddress(m_lib, "frostRecv");

        m_pWSASend = (int (*)(SOCKET, LPWSABUF, DWORD, LPDWORD,
                      DWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE))
            GetProcAddress(m_lib, "frostWSASend");

        m_pWSARecv = (int (*)(SOCKET, LPWSABUF, DWORD,
            LPDWORD, LPDWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE))
            GetProcAddress(m_lib, "frostWSARecv");

        m_pSendto = (int (*)(SOCKET, const char*, int, int,
                             const struct sockaddr*, int))
             GetProcAddress(m_lib, "frostSendto");

        m_pRecvfrom = (int (*)(SOCKET, char*, int, int,
                       struct sockaddr*, int*))
             GetProcAddress(m_lib, "frostRecvfrom");
    }
}

shieldSecurityDll::~shieldSecurityDll()
{
    if(m_lib) {
        FreeLibrary(m_lib);
    }
}

bool shieldSecurityDll::init(const char* dllPath)
{
	shieldSecurityDll& inst = get(dllPath);
	if (inst.m_lib)
		return true;
	return false;
}

shieldSecurityDll& shieldSecurityDll::get(const char* dllPath)
{
    static shieldSecurityDll inst(dllPath);
    return inst;
}

void shieldSecurityDll::setUserName(const char* uin)
{
    static shieldSecurityDll& inst = get();
    if(inst.m_pSetUserName) {
        inst.m_pSetUserName(uin);
    }
}

int shieldSecurityDll::connect(SOCKET s,
                               const struct sockaddr* name,
                               int namelen)
{
    static shieldSecurityDll& inst = get();
    if(inst.m_pConnect) {
        return inst.m_pConnect(s, name, namelen);
    }
    return ::connect(s, name, namelen);
}

int shieldSecurityDll::send(SOCKET s,
                            const char* buf,
                            int len,
                            int flags)
{
    static shieldSecurityDll& inst = get();
    if(inst.m_pSend) {
        return inst.m_pSend(s, buf, len, flags);
    }
    return ::send(s, buf, len, flags);
}

int shieldSecurityDll::recv(SOCKET s,
                            char* buf,
                            int len,
                            int flags)
{
    static shieldSecurityDll& inst = get();
    if(inst.m_pRecv) {
        return inst.m_pRecv(s, buf, len, flags);
    }
    return ::recv(s, buf, len, flags);
}

int shieldSecurityDll::WSASend(SOCKET s,
                               LPWSABUF lpBuffers,
                               DWORD dwBufferCount,
                               LPDWORD lpNumberOfBytesSent,
                               DWORD dwFlags,
                               LPWSAOVERLAPPED lpOverlapped,
                               LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    static shieldSecurityDll& inst = get();
    if(inst.m_pWSASend) {
        return inst.m_pWSASend(s,
                               lpBuffers,
                               dwBufferCount,
                               lpNumberOfBytesSent,
                               dwFlags,
                               lpOverlapped,
                               lpCompletionRoutine);
    }
    return ::WSASend(s,
                     lpBuffers,
                     dwBufferCount,
                     lpNumberOfBytesSent,
                     dwFlags,
                     lpOverlapped,
                     lpCompletionRoutine);
}
    
int shieldSecurityDll::WSARecv(SOCKET s,
                               LPWSABUF lpBuffers,
                               DWORD dwBufferCount,
                               LPDWORD lpNumberOfBytesRecvd,
                               LPDWORD lpFlags,
                               LPWSAOVERLAPPED lpOverlapped,
                               LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    static shieldSecurityDll& inst = get();
    if(inst.m_pWSARecv) {
        return inst.m_pWSARecv(s,
                               lpBuffers,
                               dwBufferCount,
                               lpNumberOfBytesRecvd,
                               lpFlags,
                               lpOverlapped,
                               lpCompletionRoutine);
    }
    return ::WSARecv(s,
                     lpBuffers,
                     dwBufferCount,
                     lpNumberOfBytesRecvd,
                     lpFlags,
                     lpOverlapped,
                     lpCompletionRoutine);
}

int shieldSecurityDll::sendto(SOCKET s,
                              const char* buf,
                              int len,
                              int flags,
                              const struct sockaddr* to,
                              int tolen)
{
    static shieldSecurityDll& inst = get();
    if(inst.m_pSendto) {
        return inst.m_pSendto(s,
                              buf,
                              len,
                              flags,
                              to,
                              tolen);
    }
    return ::sendto(s,
                    buf,
                    len,
                    flags,
                    to,
                    tolen);
}

int shieldSecurityDll::recvfrom(SOCKET s,
                                char* buf,
                                int len,
                                int flags,
                                struct sockaddr* from,
                                int* fromlen)
{
    static shieldSecurityDll& inst = get();
    if(inst.m_pRecvfrom) {
        return inst.m_pRecvfrom(s,
                                buf,
                                len,
                                flags,
                                from,
                                fromlen);
    }
    return ::recvfrom(s,
                      buf,
                      len,
                      flags,
                      from,
                      fromlen);
}
