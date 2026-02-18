#ifndef __GEOIP2_H__
#define __GEOIP2_H__

#define GEOIP_IP_COUNT			150000

struct GEOIP_DATA
{
	INT32		m_i32Count;
	UINT32		m_pui32IP[ GEOIP_IP_COUNT ];
	UINT8		m_pui8NationCode[ GEOIP_IP_COUNT ];
};

class CGeoIP2
{
	char					m_strFilePath[ MAX_PATH ];

	INT32					m_i32UseIdx;
	GEOIP_DATA				m_stIPData[ 2 ];

	BOOL					_LoadFile( INT32 i32ThreadIdx, GEOIP_DATA* pData );

public:
	~CGeoIP2();

	BOOL					Create();
	UINT8					GetNationCode( UINT32 ui32IP );
	BOOL					ChangeIPFile( INT32 i32ThreadIdx );
};

#endif