#include "i3CommonType.h"
#include "i3Stream.h"
#include "i3String.h"
#include "i3ErrorHandler.h"
#include "i3StringTable.h"

I3_CLASS_INSTANCE( i3StringTable, i3ElementBase );

i3StringTable::i3StringTable()
{

	m_pTable			= NULL;
	m_pLabelBuf			= NULL;
	m_pStringBuf		= NULL;

	m_nNumString		= 0;
	m_nTotalLabelSize	= 0;
	m_nTotalStringSize	= 0;
}

i3StringTable::~i3StringTable()
{
	DestroyBuffers();
}

void i3StringTable::DestroyBuffers()
{
	I3MEM_SAFE_FREE_POINTER( m_pTable);
	I3MEM_SAFE_FREE_POINTER( m_pLabelBuf);
	I3MEM_SAFE_FREE_POINTER( m_pStringBuf);

	m_nNumString		= 0;
	m_nTotalLabelSize	= 0;
	m_nTotalStringSize	= 0;
}

BOOL i3StringTable::NewBuffers()
{
	I3ASSERT( m_pTable == NULL );
	m_pTable = (STRING_INFO*)i3MemAlloc( sizeof(STRING_INFO) * m_nNumString );
	I3ASSERT( m_pTable != NULL );

	I3ASSERT( m_pLabelBuf == NULL );
	m_pLabelBuf = (char*)i3MemAlloc( m_nTotalLabelSize );
	I3ASSERT( m_pLabelBuf != NULL );

	I3ASSERT( m_pStringBuf == NULL );
	m_pStringBuf = (WCHAR16*)i3MemAlloc( m_nTotalStringSize );
	I3ASSERT( m_pStringBuf != NULL );

	return TRUE;
}

BOOL i3StringTable::CreateFromStream( i3Stream *pStream )
{
	DestroyBuffers();

	BOOL bRet = FALSE;

	// Head
	char szHead[10];

	if( pStream->Read( szHead, 2 ) == STREAM_ERR)
	{
		I3TRACE( "Could not read a header of the stream.");
		return FALSE;
	}

	if( szHead[0] != 'S' || szHead[1] != 'T' )
	{
		I3TRACE( "Not String Table file.\n" );
		goto ExitLabel;
	}

	// Version Number
	UINT16 nVersion;
	if( pStream->Read( &nVersion, sizeof(UINT16) ) == STREAM_ERR)
	{
		I3TRACE( "Could not read a version number\n");
		goto ExitLabel;
	}

	if( nVersion != 1 )
	{
		I3TRACE( "Version number is incorrect. (%d)\n", nVersion );
		goto ExitLabel;
	}

	// Number of Strings
	if( pStream->Read( &m_nNumString, sizeof(m_nNumString) ) == STREAM_ERR)
	{
		I3TRACE( "Could not read number of strings information\n");
		goto ExitLabel;
	}

	if( m_nNumString <= 0 )
	{
		I3TRACE( "Number of string is negative. (%d)\n", m_nNumString );
		goto ExitLabel;
	}

	// Total size of labels
	if( pStream->Read( &m_nTotalLabelSize, sizeof(m_nTotalLabelSize)) == STREAM_ERR)
	{
		I3TRACE( "Could not read total label size information\n");
		goto ExitLabel;
	}
	
	// Total size of strings
	if( pStream->Read( &m_nTotalStringSize, sizeof(m_nTotalStringSize) ) == STREAM_ERR)
	{
		I3TRACE( "Could not read total string size information\n");
		goto ExitLabel;
	}

	// New Buffer
	if( ! NewBuffers() )
	{
		goto ExitLabel;
	}
    
	UINT32 nLength;
	nLength = pStream->Read( m_pTable, sizeof(STRING_INFO) * m_nNumString );
	if( nLength != (sizeof(STRING_INFO) * m_nNumString) )
	{
		I3TRACE( "Failed to read Information table.\n" );
		goto ExitLabel;
	}

	nLength = pStream->Read( m_pLabelBuf, m_nTotalLabelSize );
	if( nLength != m_nTotalLabelSize )
	{
		I3TRACE( "Failed to read labels.\n" );
		goto ExitLabel;
	}

	nLength = pStream->Read( m_pStringBuf, m_nTotalStringSize );
	if( nLength != m_nTotalStringSize )
	{
		I3TRACE( "Failed to read strings.\n" );
		goto ExitLabel;
	}

	bRet= TRUE;
ExitLabel:
	return bRet;
}

const char*	i3StringTable::GetLabel( INT32 nIndex )
{
	if( nIndex < 0 || UINT32( nIndex ) >= m_nNumString )
	{
		return NULL;
	}

	I3ASSERT( m_pLabelBuf != NULL );
	return &m_pLabelBuf[ m_pTable[nIndex].LabelPos ];
}

const WCHAR16 *i3StringTable::GetString( const char *szLabel )
{
	UINT32 i;

	for( i = 0; i < m_nNumString; i++ )
	{
		if( i3String::Compare( szLabel, &m_pLabelBuf[ m_pTable[i].LabelPos ] ) == 0 )
		{
			return &m_pStringBuf[ m_pTable[i].StringPos ];
		}
	}
	return NULL;
}

const WCHAR16*	i3StringTable::GetString( INT32 nIndex )
{
	if( nIndex < 0 || UINT32( nIndex ) >= m_nNumString )
	{
		return NULL;
	}

	I3ASSERT( m_pStringBuf != NULL );
	return &m_pStringBuf[ m_pTable[nIndex].StringPos ];
}

INT32 i3StringTable::GetStringIndex( const char *szLabel )
{
	UINT32 i;

	for( i = 0; i < m_nNumString; i++ )
	{
		if( i3String::Compare( szLabel, &m_pLabelBuf[ m_pTable[i].LabelPos ] ) == 0 )
		{
			return i;
		}
	}
	return -1;
}

