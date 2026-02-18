// DediMember.h
//
// YouJong Ha
//	Last update : 2010-10-01 (yyyy:mm:dd)
//	
// Description:
//	각 멤버는 클라이언트에 해당한다.	
//	테스트 버전에서는 group이 read only라고 가정.
//	이 파일은 변경 가능성이 많다.
//
// Dependency: 
//
#ifndef _DediMember_h
#define _DediMember_h

class CDediMember: public i3ElementBase
{
	I3_CLASS_DEFINE( CDediMember );

private: 
	BOOL		m_bPlaying;
	UINT32		m_ui32Ip;
	UINT16		m_ui16Port;

public:
	CDediMember();
	virtual ~CDediMember();

	void		Init(void);
	BOOL		InsertContext( UINT32 ui32IP, UINT16 ui16Port );
	BOOL		DeleteContext( );

	//BOOL		IsSameAddress( N_DSERVER_USER_ENTER* user );
	BOOL		IsSameAddress( UINT32 wIp, UINT16 sPort);
	//BOOL		CheckRegisteredIP(UINT32 dwIP);

	BOOL		IsMember()								{	return ((( 0 == m_ui32Ip ) || ( 0 == m_ui16Port ) ) ? FALSE : TRUE );	};

	UINT32		GetIp(void) const						{	return m_ui32Ip;		};
	void		SetIp(UINT32 ui32Ip)					{   m_ui32Ip = ui32Ip;		};
	UINT16		GetPort(void) const						{	return m_ui16Port;		};
	void		SetPort(UINT16 ui16Port)				{	m_ui16Port = ui16Port;	};

	BOOL		GetPlaying(void) const					{	return m_bPlaying;		};
	void		SetPlaying( BOOL bPlaying ) 			{	m_bPlaying = bPlaying;	};
};

#endif
