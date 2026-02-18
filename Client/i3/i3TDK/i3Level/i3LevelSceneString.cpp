#include "stdafx.h"
#include "i3LevelScene.h"
#include "i3LevelElement3D.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelResSceneGraph.h"

///////////////////////////////
// String
bool			i3LevelScene::LoadStringTable( const char *szFileName )
{
	i3StringTable *pStringTable = nullptr;

	i3FileStream *pStream = i3FileStream::new_object();
	if( ! pStream->Open( szFileName, STREAM_READ | STREAM_SHAREREAD ) )
	{
		I3_SAFE_RELEASE( pStream );
		return false;
	}

	pStringTable = i3StringTable::new_object();
	if( ! pStringTable->CreateFromStream( pStream ) )
	{
		I3_SAFE_RELEASE( pStringTable );
		I3_SAFE_RELEASE( pStream );
		return false;
	}

	I3_SAFE_RELEASE( pStream );

	I3_SAFE_RELEASE( m_pStringTable );
	m_pStringTable = pStringTable;
	strncpy( m_szStringTablePath, szFileName, sizeof(m_szStringTablePath) -1);

	return true;
}

INT32			i3LevelScene::GetStringCount(void)
{
	if( m_pStringTable == nullptr )
	{
		return -1;
	}

	return m_pStringTable->GetCount();
}

const char *	i3LevelScene::GetLabel( INT32 idx )
{
	I3ASSERT( m_pStringTable != nullptr );

	return m_pStringTable->GetLabel( idx );
}

const WCHAR16 *	i3LevelScene::GetString( INT32 idx )
{
	I3ASSERT( m_pStringTable != nullptr );

	return m_pStringTable->GetString( idx );
}
