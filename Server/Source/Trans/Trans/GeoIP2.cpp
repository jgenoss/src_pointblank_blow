#include "pch.h"
#include <stdlib.h>
#include <string.h>
#include "GeoIP2.h"

BOOL CGeoIP2::Create()
{
	i3String::Format( m_strFilePath, MAX_PATH, "%s\\GeoIPCountryWhois.csv", g_pCCurrentPath );

	m_i32UseIdx = 0;
	i3mem::FillZero( m_stIPData, sizeof(GEOIP_DATA)*2 );	

	return _LoadFile( g_i32MainLogIdx, &m_stIPData[m_i32UseIdx] );
}

CGeoIP2::~CGeoIP2()
{
}

BOOL CGeoIP2::_LoadFile( INT32 i32ThreadIdx, GEOIP_DATA* pData )
{
	char strTmp[512];
	UINT32 ui32StoreEnd = 0;
	FILE* fp = fopen( m_strFilePath, "rt" );
	if ( NULL == fp) return FALSE;
	
	INT32 i32Idx = 0;
	char* pFirst, *pSecond, *pThird, *pForth;
	UINT32 ui32StoreBegin;

	while( TRUE )
	{
		fgets( strTmp, 512, fp );
		if ( feof(fp) ) break;
		
		pFirst	= strstr( strTmp,		"," );
		pSecond	= strstr( pFirst + 1,	"," );
		pThird	= strstr( pSecond + 1,	"," );
		pForth	= strstr( pThird + 1,	"," );

		*(pThird - 1) = '\0';
		ui32StoreBegin = (UINT32)_atoi64( pSecond + 2 );

		if ( i32Idx != 0 && ui32StoreEnd + 1 != ui32StoreBegin )
		{
			pData->m_pui32IP[i32Idx]		= ui32StoreEnd + 1;
			pData->m_pui8NationCode[i32Idx]	= 0;
			i32Idx++;
			if( GEOIP_IP_COUNT <= i32Idx )
			{
				g_pLog->WriteLog( L"[ERRO] GeoIP2 Load Count" );
				break;
			}
		}

		pData->m_pui32IP[i32Idx] = ui32StoreBegin;

		*(pForth - 1) = '\0';
		ui32StoreEnd = (UINT32)_atoi64( pThird + 2 );

		*(pForth + 4) = '\0';

		BOOL bFound						= FALSE;
		pData->m_pui8NationCode[i32Idx] = 0;
		for( INT32 i = 0; i < GEOIP2_MAX_COUNTRY_COUNT; i++ )
		{
			if ( 0 == i3String::Compare( pForth + 2, g_astrGeoIP2_country_code[i] ) )
			{
				pData->m_pui8NationCode[i32Idx] = (UINT8)i;
				bFound = TRUE;
				break;
			}
		}
		if ( !bFound )
		{
			g_pLog->WriteLog( L"[ERRO] GeoIP2 Not Find Country" );
			continue;
		}
		i32Idx++;
		if( GEOIP_IP_COUNT <= i32Idx )
		{
			g_pLog->WriteLog( L"[ERRO] GeoIP2 Load Count" );
			break;
		}
	}
	fclose( fp );

	pData->m_i32Count = i32Idx;

	return TRUE;
}

BOOL CGeoIP2::ChangeIPFile( INT32 i32ThreadIdx )
{
	INT32 i32Idx = m_i32UseIdx + 1;
	if( 1 < i32Idx ) i32Idx = 0;

	if( FALSE == _LoadFile( i32ThreadIdx, &m_stIPData[ i32Idx ] ) )
	{
		return FALSE;
	}

	g_pLog->WriteLog( L"[SUCC] GeoIP2 File Change" );
	m_i32UseIdx = i32Idx;
	return TRUE;
}

UINT8 CGeoIP2::GetNationCode( UINT32 ui32IP )
{
	GEOIP_DATA* pstIPData = &m_stIPData[m_i32UseIdx];

	INT32 i32Low	= 0;
	INT32 i32High	= pstIPData->m_i32Count - 1;
	INT32 i32Mid	= 0;

	while( i32Low <= i32High )
	{
		i32Mid = (i32Low + i32High) / 2;
		if ( pstIPData->m_pui32IP[i32Mid] > ui32IP)			i32High = i32Mid - 1;
		else if ( pstIPData->m_pui32IP[i32Mid] < ui32IP)	i32Low	= i32Mid + 1;
		else return pstIPData->m_pui8NationCode[i32Mid];
	}

	if ( ui32IP < pstIPData->m_pui32IP[i32Mid] )		return pstIPData->m_pui8NationCode[i32Mid - 1];
	if ( ui32IP < pstIPData->m_pui32IP[i32Mid + 1] )	return pstIPData->m_pui8NationCode[i32Mid];
	return pstIPData->m_pui8NationCode[i32Mid + 1];
}