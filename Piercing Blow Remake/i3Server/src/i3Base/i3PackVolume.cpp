#include "i3CommonType.h"
#include "i3PackVolume.h"
#include "i3String.h"

I3_CLASS_INSTANCE( i3PackVolume, i3ElementBase);

i3PackVolume::i3PackVolume(void)
{
	m_pRoot = NULL;
	i3mem::FillZero( &m_Preview, sizeof(PACK_VOLUME_PREVIEW)); 
}

i3PackVolume::~i3PackVolume(void)
{
	I3_SAFE_RELEASE( m_pRoot);

	m_File.Close();
}

bool i3PackVolume::SaveToFile( const char * pszPath)
{
	i3ResourceFile resFile;

	if( m_pRoot == NULL)
		return true;

	resFile.setKeyObject( m_pRoot);

	if( resFile.Save( pszPath) == STREAM_ERR)
	{
		I3TRACE( "Could not save pack-node database. : %s\n", pszPath);
		return true;
	}

	return true;
}

bool	i3PackVolume::Export( const char * pszDirPath )
{
	if ( pszDirPath == NULL )
		return false;
	if ( m_pRoot == NULL )
		return false;


	m_File.SetPosition(0);

	INT32 failedCount = Export_Rec(m_pRoot, pszDirPath);
	return (failedCount > 0) ? false : true;
}

INT32	i3PackVolume::Export_Rec( i3PackNode * pNode, const char * pszDirPath )
{
	INT32 failedCount = 0;
	char szBase[1024] = {};

	if ( i3String::Compare( pNode->GetName() , "/") != 0 )
	{
		i3String::Format( szBase, 1024, "%s/%s", pszDirPath, pNode->GetName() );
		i3System::PrepareDirectory( szBase );
	}
	else
	{
		i3String::Copy( szBase, pszDirPath, 1024);
	}

	for ( INT32 i = 0 ; i < pNode->getFileCount() ; ++i )
	{
		I3_PACK_FILE_REF * pRef = pNode->getFile(i);
		char Path[1024] = {};
		i3String::Format(Path, 1024, "%s/%s", szBase, pRef->m_szName );

		i3FileStream	DestStream;
		if ( DestStream.Create( Path, STREAM_WRITE ) == FALSE )
		{
			failedCount++;
			continue;
		}

		m_File.SetPosition(pRef->m_Offset);
		DestStream.CopyFrom( &m_File,  pRef->m_Size);
	}

	for ( INT32 i = 0 ; i < pNode->getChildCount() ; ++i )
	{
		i3PackNode * pChild = (i3PackNode*)pNode->getChild(i);
		failedCount += Export_Rec(pChild, szBase);
	}

	return failedCount;
}

bool i3PackVolume::LoadFromFile( const char * pszPath)
{
	i3ResourceFile file;

	if( file.Load( pszPath) == STREAM_ERR)
	{
		I3TRACE( "Could not load pack-node database : %s\n", pszPath);
		return false;
	}

	I3ASSERT( file.getKeyObject() != NULL);
	I3ASSERT( file.getKeyObject()->IsExactTypeOf( i3PackNode::GetClassMeta()));

	i3PackNode * pNode = (i3PackNode *) file.getKeyObject();

	I3_REF_CHANGE( m_pRoot, pNode);

	if( m_File.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == FALSE)
	{
		I3TRACE( "Could not open pack-node database : %s\n", pszPath);
		return false;
	}

	return true;
}

UINT32 i3PackVolume::Preview( const char* pszPath)
{
	BOOL Rv;
	UINT32 Rc = 0, Result = 0;
	bool bOldVersion = false;

	i3String::Copy( m_Preview.filename, pszPath, MAX_PATH);

	Rv = m_File.Open( pszPath, STREAM_READ | STREAM_SHAREREAD);
	if( Rv == FALSE)
	{
		i3Log( "i3PackVolume::Preview", "Could not open %s file in Preview.", pszPath);
		return STREAM_ERR;
	}

	UINT8	id[4];

	Rc = m_File.Read( id, 4);
	if( Rc == STREAM_ERR)
	{
		I3FATAL( "Could not read file-identifier in Preview.\n");
		return STREAM_ERR;
		
	}
	Result += Rc;

	if( memcmp( id, I3_RSCFILE_HEAD_10, 4) == 0)
		bOldVersion = true;

	// Preview Header Information

	if( bOldVersion)
	{
		I3_RESOURCE_FILE_HEADER1 OldHeader;

		Rc = m_File.Read( &OldHeader, sizeof( OldHeader));
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not read header(version 1.0f) information in Preview.\n");
			Result = STREAM_ERR;
		}
		Result += Rc;

		m_Preview.header.m_MajorVersion			= OldHeader.m_MajorVersion;
		m_Preview.header.m_MinorVersion			= OldHeader.m_MinorVersion;
		m_Preview.header.m_ObjectCount			= OldHeader.m_ObjectCount;
		m_Preview.header.m_ObjectDirOffset		= OldHeader.m_ObjectDirOffset;
		m_Preview.header.m_ObjectDirSize		= OldHeader.m_ObjectDirSize;
		m_Preview.header.m_StringTableCount		= OldHeader.m_StringTableCount;
		m_Preview.header.m_StringTableOffset	= OldHeader.m_StringTableOffset;
		m_Preview.header.m_StringTableSize		= OldHeader.m_StringTableSize;
	}
	else
	{
		Rc = m_File.Read( &m_Preview.header, sizeof( I3_RESOURCE_FILE_HEADER2));
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not read header information in Preview.");
			Result = STREAM_ERR;
		}
		Result += Rc;
	}

	m_File.Close();

	return Result;
}

void i3PackVolume::DumpVolume()
{
	if( m_pRoot == NULL)
	{
		I3TRACE( "	-- not files \n");
		return;
	}

	I3TRACE( "	-- Root Address : (0x%p) (i3PackVolume)\n", m_pRoot);
	m_pRoot->DumpPack();
}