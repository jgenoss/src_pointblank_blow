#include "stdafx.h"
#include "Memory.h"
#include "IniParser.h"
#include "File.h"

NSM_cIniParser::NSM_cIniParser()
{
	m_ui32LabelCount	= 0;
	m_pLabelData		= NULL;
}

NSM_cIniParser::~NSM_cIniParser()
{
	Close();
}

BOOL NSM_cIniParser::Create( const char* strFileName )
{
	BOOL bSuccess = FALSE;
	NSM_cFile File;

	if( FALSE == File.CreateFile( strFileName ) )	return FALSE;

	UINT32 ui32Size = File.GetFileSize();

	if( 0 == ui32Size )								return FALSE;

	char* pBuffer;
	pBuffer = (char*)NSM_nsMemory::Alloc( ui32Size+1 );
	NSM_nsMemory::FillZero( pBuffer, ui32Size+1 );
	
	File.Read( pBuffer, ui32Size );

	_SetLabelSize( pBuffer );
	bSuccess = _SetLabelBuffer( pBuffer );

	NSM_SAFE_FREE( pBuffer );

	File.Close();

	return bSuccess;
}

void NSM_cIniParser::Close()
{
	NSM_SAFE_FREE( m_pLabelData );
}

void NSM_cIniParser::_SetLabelSize( char* pBuffer )
{
	m_ui32LabelCount = 0;
	while( '\0' != *pBuffer )
	{
		char strLine[ MAX_LABELVALUE ];
		UINT32 ui32Size;
		_ReadLine( strLine, MAX_LABELVALUE, pBuffer, &ui32Size );
		pBuffer += ui32Size;

		LABEL_DATA LabelData;
		if( FALSE == _GetLineValue( strLine, &LabelData ) )	continue;

		m_ui32LabelCount++;
	}
}

BOOL NSM_cIniParser::_SetLabelBuffer( char* pBuffer )
{
	if( 0 == m_ui32LabelCount )	return FALSE;
	
	m_pLabelData = (LABEL_DATA*)NSM_nsMemory::Alloc( sizeof(LABEL_DATA)*m_ui32LabelCount );

	UINT32 ui32Count = 0;
	while( ui32Count != m_ui32LabelCount && '\0' != *pBuffer )
	{
		char strLine[ MAX_LABELVALUE ];
		UINT32 ui32Size;
		_ReadLine( strLine, MAX_LABELVALUE, pBuffer, &ui32Size );
		pBuffer += ui32Size;

		if( _GetLineValue( strLine, &m_pLabelData[ ui32Count ] ) )
		{
			ui32Count++;
		}
	}
	if( ui32Count != m_ui32LabelCount )	return FALSE;

	return TRUE;
}

void NSM_cIniParser::_ReadLine( char* strLine, UINT32 ui32LineSize, char* pBuffer, UINT32 *pui32ReturnSize )
{
	*pui32ReturnSize = 0;
	while( ('\0' != *pBuffer) && ('\n' != *pBuffer) && ('\r' != *pBuffer) && ((*pui32ReturnSize) < (ui32LineSize-1)))
	{
		*strLine = *pBuffer;
		strLine++;
		pBuffer++;
		(*pui32ReturnSize)++;
	}
	*strLine = '\0';			// Over 방지
	(*pui32ReturnSize) += 2;	// '\r\n'
}

BOOL NSM_cIniParser::_GetLineValue( char* strLine, LABEL_DATA* pLabelData )
{
	// ReadLine 에서 이미 얻어올수 있는 라인을 검사하기 때문에 오버플로어 체크 안해도 됩니다.
	// 주석이면 return
	// str = str 의 구조가 아니면 return
	UINT32 ui32Step = 0;
	char* pstrLabel = pLabelData->_strLabel;
	char* pstrValue = pLabelData->_strValue;

	while( '\0' != *strLine )
	{
		if( ';' == *strLine )	break;
		if( '\0' == *strLine )	break;
		if( '\n' == *strLine )	break;
		if( '\r' == *strLine )	break;

		switch( ui32Step )
		{
		case 0:	// 앞쪽 ' ' 이나 '\t'패스
			if( (' ' != *strLine) && ('\t' != *strLine) )
			{
				ui32Step++;
			}
			else
			{
				strLine++;
			}
			break;
		case 1:	// Label 때어내기
			if( (' ' == *strLine) || ('\t' == *strLine) )
			{
				
			}
			else if( '=' == *strLine )
			{
				ui32Step++;				
			}
			else
			{
				*pstrLabel = *strLine;
				pstrLabel++;
			}
			strLine++;
			break;
		case 2:				//필요없는 부분 입니다. 차후 현우대리가 수정할겁니다.
			/*
			if( '=' == *strLine )
			{
				ui32Step++;
			}
			strLine++;
			*/
			ui32Step++;
			break;
		case 3:
			if( (' ' != *strLine) && ('\t' != *strLine) )
			{
                ui32Step++;
			}
			else
			{
				strLine++;
			}
			break;
		case 4:	// Value 때어내기
			if( (' ' == *strLine) || ('\t' == *strLine) )
			{
				*pstrLabel = '\0';
				*pstrValue = '\0';
				return TRUE;
			}
			else
			{
				*pstrValue = *strLine;
				pstrValue++;
			}
			strLine++;
		}
	}

	if( 4 == ui32Step ) 
	{
		*pstrLabel = '\0';
		*pstrValue = '\0';
		return TRUE;
	}

	return FALSE;
}

char* NSM_cIniParser::GetValue( const char* strLabel )
{
	for( UINT32 i = 0 ; i < m_ui32LabelCount ; i++ )
	{
		if( 0 == strcmp( m_pLabelData[ i ]._strLabel, strLabel ) )
		{
			return m_pLabelData[ i ]._strValue;
		}
	}
	return NULL;
}

BOOL NSM_cIniParser::GetValue( const char* strLabel, char* strValue )
{
	for( UINT32 i = 0 ; i < m_ui32LabelCount ; i++ )
	{
		if( 0 == strcmp( m_pLabelData[ i ]._strLabel, strLabel ) )
		{
			strcpy( strValue, m_pLabelData[ i ]._strValue );
			return TRUE;
		}
	}
	return FALSE;
}