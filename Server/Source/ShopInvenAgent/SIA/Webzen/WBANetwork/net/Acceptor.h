#ifndef  __WBANetwork_Acceptor_H
#define  __WBANetwork_Acceptor_H


#include <net/Socket.h>
#include <util/Thread.h>

namespace WBANetwork
{
	class ServerSession;

	class Acceptor : public Thread
	{
	public:
		Acceptor();
		virtual	~Acceptor();

	private:
		Socket			m_ListenSocket;
		bool			m_flagKill;

	protected:
		virtual	void	Run();
		virtual	void	OnAccept(SOCKET /*s*/, Socket::SocketAddr& /*addr*/){}

		bool			Open(char* ip, u_short portNo, int sndBufSize = 0, int rcvBufSize = 0);
		void			Close();

	};
}

#endif