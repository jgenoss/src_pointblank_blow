#ifndef __IPCHECKER_H__
#define __IPCHECKER_H__

class CIPChecker 
{
	BOOL			m_bUsed;

	INT32			m_i32OpenIPCount;
	IPBLOCK_IP		m_stOpenIP[ IPBLOCK_IP_OPEN_COUNT ];

	BOOL			m_bOpenNation[ GEOIP2_MAX_COUNTRY_COUNT ];

	BOOL			_LoadFromFile(const char * pstrFileName);							// Config.ini 파일 읽어서 멤버변수(구조체들) 셋팅 하기.
	BOOL			_CheckCountryCode( UINT32 ui32IpNum, INT32 i32NationalCode );		// 인자로 넘어온 IP를 Config.ini에서 셋팅한 멤버변수 값과 비교하여 결과값 리턴.

	unsigned long	_GeoIP_addr_to_num(const char *paddr);

	INT32			_GetCountryIdx_By_Code(char * pCountryCode); 
	
public: 
	CIPChecker();
	~CIPChecker();
	BOOL			Create( BOOL bUsed, INT32 i32IPCount, IPBLOCK_IP* pOpenIP, INT32 i32NationCount, UINT8* pui8OpenNation );
	void			SetOpenList( BOOL bUsed, INT32 i32IPCount, IPBLOCK_IP* pOpenIP, INT32 i32NationCount, UINT8* pui8OpenNation );

	BOOL			CheckBlock( UINT32 ui32IpNum, INT32 i32NationalCode );				// 인자로 넘어온 IP를 Config.ini에서 셋팅한 멤버변수 값과 비교하여 결과값 리턴.	
};

#endif