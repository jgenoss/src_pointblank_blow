#if !defined( __V3FIREWALL_H_ )
#define __V3FIREWALL_H_

#define		V3_FIREWALL_WAIT		0	
#define		V3_FIREWALL_OK			1
#define		V3_FIREWALL_FAIL		2


class CV3FireWall : public i3Thread
{
	I3_CLASS_DEFINE( CV3FireWall, i3Thread );
protected:
	i3::vector<UINT32>	m_IPList;		///< Local IP List
	INT32	m_CurrentIPIdx;

	INT32	m_IPSendCount;

	UINT32	m_MyIp;			///< Send Success My IP
	i3Udp	m_DummyUdpSocket;
public:
	INT32	m_State;
	INT32	GetState(void)		{return m_State; }

public:
	CV3FireWall();
	virtual ~CV3FireWall();

	void	Create( INT32 nThreadLevel = PRIORITY_NORMAL);
	void	OnDestroy();
	void	Send(); 
	virtual UINT32 OnRunning( void * pUserData ) override;
};

#endif