#ifndef _RelayMember_h
#define _RelayMember_h

class CRelayMember: public i3ElementBase
{
	I3_CLASS_DEFINE( CRelayMember );

	UINT32		m_wIp;
	UINT16		m_sPort;
	BOOL		m_bPlaying;

public:
	CRelayMember();
	virtual ~CRelayMember();

	void Init(void);
	BOOL		InsertContext( RELAY_MEMBER_INFO* pRelayMember );
	BOOL		DeleteContext( );

	BOOL		IsSameAddress( RELAY_MEMBER_INFO* pRelayMember );
	BOOL		IsSameAddress(UINT32 wIp, UINT16 sPort);

	BOOL		IsMember()								{	return ((( 0 == m_wIp ) && ( 0 == m_sPort ) ) ? FALSE : TRUE );	};

	UINT32		GetIp(void) const						{	return m_wIp;			};
	UINT16		GetPort(void) const						{	return m_sPort;			};

	BOOL		GetPlaying(void) const					{	return m_bPlaying;		};
	void		SetPlaying( BOOL bPlaying ) 			{	m_bPlaying = bPlaying;	};
};

#endif
