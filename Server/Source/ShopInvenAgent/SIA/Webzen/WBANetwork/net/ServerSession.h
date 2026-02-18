#ifndef  __WBANetwork_ServerSession_H
#define  __WBANetwork_ServerSession_H


#include <net/Socket.h>
#include <util/EventQueue.h>
#include <util/CircuitQueue.h>
#include <util/Mutex.h>
#include <util/AsyncResult.h>


namespace WBANetwork
{
	enum SEND_RET {SUCESS, FAILD_OVERFLOW, FAILD_CLOSED, FAILD_NULL};

	const int FIXED_MAX_BUF_SIZE = 10240;
	
	class Stream;
	class ServerNetwork;

	class ServerSession : public EventHandler
	{
		friend class SessionManager;

	public:
		ServerSession(int sendBufSize, int recvBufSize);
		virtual	~ServerSession();

	public:
		AsyncResult			m_AResultAccept;
		AsyncResult			m_AResultConnect;
		AsyncResult			m_AResultClose;
		AsyncResult			m_AResultSend;
		AsyncResult			m_AResultRecv;

	private:
		DWORD				m_UID;
		Socket				m_Socket;

		CircuitQueue<BYTE>	m_CQSendBuffer;
		CircuitQueue<BYTE>	m_CQRecvBuffer;
		PBYTE				m_szDequeueBuffer;

		bool				m_bReadyToSend;
		DWORD				m_dwSendBufSize;
		DWORD				m_dwRecvBufSize;
		DWORD				m_dwSendReqSize;
		DWORD				m_dwSendCompletionSize;
		DWORD				m_dwMaxSizePerUpdate;

		ServerNetwork*		m_pServerNetwork;
		Mutex				m_MutexSend, m_MutexRecv;

	public:
		//  Derived virtual function
		virtual	void		HandleEvent(AsyncResult* result);

		virtual	Socket*		GetHandle()			{ return &m_Socket;	}
		virtual	char*		GetBuffer()			{ return 0; }
		virtual	int			GetBufferSize()		{ return 0; }

		void				Flush();

		void				Create();
		void				Create(SOCKET s, Socket::SocketAddr& addr);
		void				Close();

		void				Connect(const char* ipAddress, unsigned short portNo);
		DWORD				SetKeepAlive(u_long onoff, u_long KeepaliveTime, u_long Keepaliveinterval);
		void				SetKill(DWORD err = 0);

		//bool				CompulsionSendPacket(PBYTE buffer, int length, AsyncResult* result);

		void				SendPacket(void* buffer, int size, SEND_RET& ret);
		virtual	bool		SendPacket(void* buffer, int size);
		virtual	bool		SendPacket(Stream& stream);

		char*				GetIPAddress()						{ return m_Socket.GetIPAddress();			}
		DWORD				GetUID()							{ return m_UID;								}

		PBYTE				GetSendBuffer()						{ return m_CQSendBuffer.GetReadPtr();		}
		PBYTE				GetRecvBuffer()						{ return m_CQRecvBuffer.GetWritePtr();		}
		DWORD				GetNextSendSize()					{ return m_CQSendBuffer.GetReadableSize();	}
		void				GetSendBufferSize(DWORD* remain, DWORD* max);
		void				GetRecvBufferSize(DWORD* remain, DWORD* max);

		DWORD				GetSendReadableSize()				{ return m_CQSendBuffer.GetReadableSize();	}
		DWORD				GetSendWritableSize()				{ return m_CQSendBuffer.GetWritableSize();	}

		DWORD				GetRecvReadableSize()				{ return m_CQRecvBuffer.GetReadableSize();	}
		DWORD				GetRecvWritableSize()				{ return m_CQRecvBuffer.GetWritableSize();	}

		void				SetMPU(DWORD size)					{ m_dwMaxSizePerUpdate = size;				}
		DWORD				GetMPU()							{ return m_dwMaxSizePerUpdate;				}

		void				InactiveSession();	// 직접 세션 Close()하는 경우 SessionManager.cpp 의 m_Pool에 반납하기위해 추가됨.

	protected:
		//  Member virtual function
		virtual	void		OnAccept()										{}
		virtual	void		OnConnect(bool /*success*/, DWORD /*error*/)	{}
		virtual	void		OnSend(int /*size*/)							{}
		virtual	void		OnReceive(PBYTE /*buffer*/, int /*size*/)		{}
		virtual	void		OnClose(DWORD /*error*/ )						{}
		virtual	void		Update();


		virtual	bool		IsValidPacket(PBYTE ptr, DWORD recvBytes, DWORD* totalSize)
							{
								*totalSize = *(DWORD*)ptr;

								return (recvBytes >= *totalSize);
							}

		ServerNetwork*		GetServerNetwork()	{ return m_pServerNetwork; }

	private:
		void				Clear();
		void				WaitForRecv();

	};
}

#endif