#include "pch.h"
#include "MedalDef.h"
const char	g_strNomalMedalPath[][MAX_PATH] = { NORMAL_MEDAL_PATH_NONE, NORMAL_MEDAL_PATH_KOREA, NORMAL_MEDAL_PATH_THAILAND, NORMAL_MEDAL_PATH_INDONESIA,
NORMAL_MEDAL_PATH_RUSSIA, NORMAL_MEDAL_PATH_TURKEY, NORMAL_MEDAL_PATH_CHINA, NORMAL_MEDAL_PATH_TAIWAN, NORMAL_MEDAL_PATH_JAPAN, NORMAL_MEDAL_PATH_BRAZIL,
NORMAL_MEDAL_PATH_LATIN_AMERICA, NORMAL_MEDAL_PATH_NORTHAMERICA, NORMAL_MEDAL_PATH_ITALY, NORMAL_MEDAL_PATH_MIDDLE_EAST, NORMAL_MEDAL_PATH_PHILIPPINES,
NORMAL_MEDAL_PATH_SINGMAL, NORMAL_MEDAL_PATH_VIETNAM, NORMAL_MEDAL_PATH_KOREA_DAUM, NORMAL_MEDAL_PATH_KOREA_PICAON };

MQF_MEDAL_INFO::MQF_MEDAL_INFO() 
: m_eMedalType		( MEDAL_TYPE_NORMAL )
, m_ui16Idx			( 0 )
, m_ui16ImageIdx	( 0 )
, m_eCheckPosition	( MEDAL_MC_CHECK_POSITION_NONE )
, m_eActionID		( MEDAL_MC_ACTION_ID_NA )
, m_eTeam			( MEDAL_MC_TEAM_NONE )
, m_ui8Temp			( 0 )
, m_eField			( MEDAL_MCF_FIELD_NONE )
, m_eStage			( STAGE_MODE_NA )
, m_eMap			( STAGE_UID_NONE )
, m_eItem			( MEDAL_MCF_ITEM_NONE )
, m_eUseItemType	( ITEM_TYPE_UNKNOWN )
, m_eWeaponType		( WEAPON_CLASS_UNKNOWN )
, m_ui32UseItemID	( 0 )
, m_ui8StageLevel	( 0 )
, m_eTargetRankBegin	( RANK_00 )
, m_eTargetRankEnd		( RANK_00 )
{
	::memset( m_szNameKey, 0, sizeof( m_szNameKey ) );
	::memset( m_szDescription, 0, sizeof( m_szDescription ) );	

	for( int i = 0; i < MEDAL_LEVEL_MAX; ++i )
	{
		m_ui16Count[i] = 0; 
		::memset( &m_eRewardInfo[i], 0, sizeof( MQF_REWARD_INFO ) );	 
	}
}
void MQF_MEDAL_INFO::SetOldData(MQF_MEDAL_INFO_OLD* old) 
{
	m_eMedalType =				old->m_eMedalType;
	m_eMedalType =				old->m_eMedalType;			
	m_ui16Idx =					old->m_ui16Idx;				
	m_ui16ImageIdx =			old->m_ui16ImageIdx;		
	m_eCheckPosition =			old->m_eCheckPosition;		
	m_eActionID =				old->m_eActionID;			
	m_eTeam =					old->m_eTeam;				
	m_ui8Temp =					old->m_ui8Temp;
	m_eField =					old->m_eField;				
	m_eStage =					old->m_eStage;					
	m_eMap =					(STAGE_UID)old->m_eMap;				
	m_eItem =					old->m_eItem;				
	m_eUseItemType =			old->m_eUseItemType;		
	m_eWeaponType =				old->m_eWeaponType;			
	m_ui32UseItemID =			old->m_ui32UseItemID;		
	m_ui8StageLevel =			old->m_ui8StageLevel;		
	m_eTargetRankBegin =		old->m_eTargetRankBegin;	
	m_eTargetRankEnd =			old->m_eTargetRankEnd;		
	strcpy (m_szNameKey, old->m_szNameKey);
	strcpy (m_szDescription, old->m_szDescription);

	for( int i = 0; i < MEDAL_LEVEL_MAX; ++i )
	{
		m_ui16Count[i] = old->m_ui16Count[i];
		::memcpy( &m_eRewardInfo[i], &old->m_eRewardInfo[i], sizeof( MQF_REWARD_INFO_OLD ) );
		m_eRewardInfo[i].m_ui32RewardMaster		= 0;
	}

}


MQF_MEDAL_INFO_OLD::MQF_MEDAL_INFO_OLD() 
: m_eMedalType		( MEDAL_TYPE_NORMAL )
, m_ui16Idx			( 0 )
, m_ui16ImageIdx	( 0 )
, m_eCheckPosition	( MEDAL_MC_CHECK_POSITION_NONE )
, m_eActionID		( MEDAL_MC_ACTION_ID_NA )
, m_eTeam			( MEDAL_MC_TEAM_NONE )
, m_ui8Temp			( 0 )
, m_eField			( MEDAL_MCF_FIELD_NONE )
, m_eStage			( STAGE_MODE_NA )
, m_eMap			( STAGE_UID_NONE )
, m_eItem			( MEDAL_MCF_ITEM_NONE )
, m_eUseItemType	( ITEM_TYPE_UNKNOWN )
, m_eWeaponType		( WEAPON_CLASS_UNKNOWN )
, m_ui32UseItemID	( 0 )
, m_ui8StageLevel	( 0 )
, m_eTargetRankBegin	( RANK_00 )
, m_eTargetRankEnd		( RANK_00 )
{
	::memset( m_szNameKey, 0, sizeof( m_szNameKey ) );
	::memset( m_szDescription, 0, sizeof( m_szDescription ) );	

	for( int i = 0; i < MEDAL_LEVEL_MAX; ++i )
	{
		m_ui16Count[i] = 0;
		::memset( &m_eRewardInfo[i], 0, sizeof( MQF_REWARD_INFO_OLD ) ); 
	}
}



CMedalSystemFile::CMedalSystemFile() 
: m_pInfo				(nullptr)
, m_hf					( INVALID_HANDLE_VALUE )
, m_bAutoMeoryDelete	( false )
{
	m_header.m_eMedalSetType	= MEDAL_SET_TYPE_NORMAL;
	m_header.m_szSignature[0]	= 'm';
	m_header.m_szSignature[1]	= 'q';
	m_header.m_szSignature[2]	= 'f';
	m_header.m_ui32MedalCount	= 0;
}

CMedalSystemFile::~CMedalSystemFile()
{
	Destroy();
}

void	CMedalSystemFile::SetMedalSetType( MEDAL_SET_TYPE eType )
{
	m_header.m_eMedalSetType = eType;
}

void	CMedalSystemFile::SetMedalUiqueIdx( unsigned int idx )
{
	m_header.m_ui32Idx = idx;
}

void	CMedalSystemFile::SetMedalInfo( MQF_MEDAL_INFO* pInfo, unsigned int uCount )
{
	m_pInfo = pInfo;
	m_header.m_ui32MedalCount = uCount;
}

void	CMedalSystemFile::Destroy()
{
	if( INVALID_HANDLE_VALUE != m_hf )
	{
		::CloseHandle( m_hf );
	}

	if( m_bAutoMeoryDelete )
	{
		I3_SAFE_DELETE_ARRAY( m_pInfo );
	}

	m_pInfo = nullptr;
	m_hf	= INVALID_HANDLE_VALUE;

	m_header.m_ui32MedalCount	= 0;
	m_header.m_szSignature[0]	= 'm';
	m_header.m_szSignature[1]	= 'q';
	m_header.m_szSignature[2]	= 'f';
}


bool	CMedalSystemFile::LoadMQF( const char* psz )
{
	if( INVALID_HANDLE_VALUE != m_hf )
	{
		::CloseHandle( m_hf );
		m_hf = INVALID_HANDLE_VALUE;
	}

	if(nullptr == psz )
	{
		return false;
	}

	m_hf = ::CreateFile( psz
					   , GENERIC_READ
					   , FILE_SHARE_READ
					   , NULL
					   , OPEN_ALWAYS
					   , FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == m_hf )
	{
		return false;
	}

	DWORD nLen = 0;	

	nLen = sizeof( MQF_MEDAL_FILEHEADER );
	::ReadFile( m_hf, &m_header, nLen, &nLen, NULL );

	if( 'm' != m_header.m_szSignature[0] || 'q' != m_header.m_szSignature[1] || 'f' != m_header.m_szSignature[2] ) 
	{
		::CloseHandle( m_hf);
		m_hf = INVALID_HANDLE_VALUE;
		return false;
	}
	
	switch( m_header.m_ui32Version )
	{
	case	MEDAL_FILE_VERSION_1:
			{
				if( m_header.m_ui32MedalCount == 0 )
				{
					::CloseHandle( m_hf);
					m_hf = INVALID_HANDLE_VALUE;
					return true;
				}

				if( m_header.m_ui32MedalCount > 4096)
				{
					::CloseHandle( m_hf);
					m_hf = INVALID_HANDLE_VALUE;
					return false;
				}

				m_pInfo = new MQF_MEDAL_INFO[ m_header.m_ui32MedalCount ];

				for( UINT32 i = 0 ; i < m_header.m_ui32MedalCount; ++i )
				{
					MQF_MEDAL_INFO_OLD OldInfo;
					nLen = sizeof( MQF_MEDAL_INFO_OLD );
					::ReadFile( m_hf, &OldInfo , nLen, &nLen, NULL );
					m_pInfo[i].SetOldData(&OldInfo);
				}
			}
			break;
	case	MEDAL_FILE_VERSION_CURRENT:
			{
				if( m_header.m_ui32MedalCount == 0 )
				{
					::CloseHandle( m_hf);
					m_hf = INVALID_HANDLE_VALUE;
					return true;
				}

				if( m_header.m_ui32MedalCount > 4096)
				{
					::CloseHandle( m_hf);
					m_hf = INVALID_HANDLE_VALUE;
					return false;
				}

				m_pInfo = new MQF_MEDAL_INFO[ m_header.m_ui32MedalCount ];

				for( UINT32 i = 0 ; i < m_header.m_ui32MedalCount; ++i )
				{
					nLen = sizeof( MQF_MEDAL_INFO );
					::ReadFile( m_hf, &m_pInfo[i] , nLen, &nLen, NULL );
				}
			}
		break;

	default:
		{
			// error
			::CloseHandle( m_hf);
			m_hf = INVALID_HANDLE_VALUE;
		}
		return false;
	}

	::CloseHandle( m_hf);
	m_hf = INVALID_HANDLE_VALUE;

	return true;
}

bool	CMedalSystemFile::SaveMQF( const char* psz )
{
	if( INVALID_HANDLE_VALUE != m_hf )
	{
		::CloseHandle( m_hf );
		m_hf = INVALID_HANDLE_VALUE;
	}

	if(nullptr == psz || nullptr == m_pInfo )
	{
		return false;
	}

	m_header.m_szSignature[0]	= 'm';
	m_header.m_szSignature[1]	= 'q';
	m_header.m_szSignature[2]	= 'f';
	m_header.m_ui32Version		= MEDAL_FILE_VERSION_CURRENT;

	m_hf = ::CreateFile( psz
					   , GENERIC_WRITE
					   , FILE_SHARE_WRITE | FILE_SHARE_READ
					   , NULL
					   , OPEN_ALWAYS
					   , FILE_ATTRIBUTE_NORMAL, NULL );	

	if( INVALID_HANDLE_VALUE == m_hf )
	{
		return false;
	}

	DWORD nLen = 0;

	nLen = sizeof( MQF_MEDAL_FILEHEADER );
	::WriteFile( m_hf, &m_header, nLen, &nLen, NULL );

	for( UINT32 i = 0; i < m_header.m_ui32MedalCount; ++i )
	{
		nLen = sizeof( MQF_MEDAL_INFO );
		::WriteFile( m_hf, &m_pInfo[i], nLen, &nLen, NULL );
	}

	return true;
}





bool	CMedalSystemFile::LoadMQFinPack( const char* psz )
{
	i3FileStream stream;
	if( stream.Open(psz, STREAM_READ | STREAM_SHAREREAD) == false )
	{
		I3PRINTLOG(I3LOG_FATAL,  "%s 파일을 열 수 없습니다.", psz );
		return false;
	}

	stream.Read( &m_header, sizeof( m_header ) );
	if( 'm' != m_header.m_szSignature[0] || 'q' != m_header.m_szSignature[1] || 'f' != m_header.m_szSignature[2] ) 
	{
		return false;
	}

	switch( m_header.m_ui32Version )
	{
	case	MEDAL_FILE_VERSION_1:
		{
			if( m_header.m_ui32MedalCount == 0 )
			{
				return true;
			}

			I3ASSERT( m_header.m_ui32MedalCount < 4096);

			m_pInfo = new MQF_MEDAL_INFO[ m_header.m_ui32MedalCount ];
			for( UINT32 i = 0 ; i < m_header.m_ui32MedalCount; ++i )
			{
				MQF_MEDAL_INFO_OLD OldInfo;
				stream.Read( &OldInfo, sizeof( MQF_MEDAL_INFO_OLD ) );
				
				m_pInfo[i].SetOldData(&OldInfo);
			}
		}
		break;
	case	MEDAL_FILE_VERSION_CURRENT:
		{
			if( m_header.m_ui32MedalCount == 0 )
			{
				return true;
			}

			I3ASSERT( m_header.m_ui32MedalCount < 4096);

			m_pInfo = new MQF_MEDAL_INFO[ m_header.m_ui32MedalCount ];

			for( UINT32 i = 0 ; i < m_header.m_ui32MedalCount; ++i )
			{
				stream.Read( &m_pInfo[i], sizeof( MQF_MEDAL_INFO ) );
			}
		}
		break;

	default:
		{
			// error
		}
		return false;
	}

	return true;
}

bool	CMedalSystemFile::SaveMQFinPack( const char* psz )
{
	m_header.m_szSignature[0]	= 'm';
	m_header.m_szSignature[1]	= 'q';
	m_header.m_szSignature[2]	= 'f';
	m_header.m_ui32Version		= MEDAL_FILE_VERSION_CURRENT;

	i3FileStream stream;
	if( stream.Open(psz, STREAM_WRITE | STREAM_SHAREWRITE) == false )
	{
		I3PRINTLOG(I3LOG_FATAL,  "%s 파일을 열 수 없습니다.", psz );
		return false;
	}

	stream.Write( &m_header, sizeof(m_header) );

	for( UINT32 i = 0; i < m_header.m_ui32MedalCount; ++i )
	{
		stream.Write( &m_pInfo[i], sizeof(MQF_MEDAL_INFO) );
	}

	return true;
}
