#include "i3CommonType.h"
#include "i3PackVolume.h"
#include "i3String.h"
#include "i3System.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/safe_string_copy.h"

#include "i3ResourceFile_Pack.h"

// 간단하게... i3ResourceFile 에 대한 파생클래스를 만든다...외부 노출이 필요없다고 판단되므로...cpp에서 처리한다..
// 특별이 예외적 상황이 발생치 않는다면 customRTTI도 사용하지 않는다.
// OnSave에서 static_cast를 사용할것이기 때문에 가상함수를 쓰지 않는다...
//
i3PackFileProgressCallback g_PackFileProgressCallback_Null;

I3_CLASS_INSTANCE( i3PackVolume);

i3PackVolume::~i3PackVolume(void)
{
	I3_SAFE_RELEASE( m_pRoot);

//	m_File.Close();		// tls이기 때문에 쓰레드에 배분된 것이며, tls소멸자에서 여러개가 close되도록 해야함...
}

bool i3PackVolume::SaveToFile( const char * pszPath)
{
	return i3PackVolume::SaveToFile(pszPath, &g_PackFileProgressCallback_Null);
}

bool	i3PackVolume::SaveToFile( const char* pszPath, i3PackFileProgressCallback* cb)
{
	if( m_pRoot == nullptr)
		return true;

	i3ResourceFile_Pack resFile(m_pRoot, cb);
	
	resFile.OnProgress_Start();

	resFile.setKeyObject( m_pRoot);

	if( resFile.Save( pszPath) == STREAM_ERR)
	{
		I3TRACE( "Could not save pack-node database. : %s\n", pszPath);
		resFile.OnProgress_End();
		return true;
	}
	
	resFile.OnProgress_End();
	return true;
}

bool	i3PackVolume::Export( const char * pszDirPath )
{
	if ( pszDirPath == nullptr)
		return false;
	if ( m_pRoot == nullptr)
		return false;

#ifdef _DEBUG
	DWORD dwThreadID = i3::get_curr_tls_thread_id();
	I3ASSERT(dwThreadID == 0);
#endif

	i3FileStream * currThreadStream = this->getCurrThreadStream();

	currThreadStream->SetPosition(0);

	INT32 failedCount = Export_Rec(currThreadStream, m_pRoot, pszDirPath);
	return (failedCount > 0) ? false : true;
}

#define READ_UNIT		2048

INT32	i3PackVolume::Export_Rec(i3FileStream* currThreadStream, i3PackNode * pNode, const char * pszDirPath )
{
	INT32 failedCount = 0;
	char szBase[1024] = {};

	if ( i3::generic_is_equal( pNode->GetNameString() , "/") == false )
	{
		i3::snprintf( szBase, 1024, "%s/%s", pszDirPath, pNode->GetNameString() );
		i3System::PrepareDirectory( szBase );
	}
	else
	{
		i3::safe_string_copy( szBase, pszDirPath, 1024);
	}

	for ( INT32 i = 0 ; i < pNode->getFileCount() ; ++i )
	{
		I3_PACK_FILE_REF * pRef = pNode->getFile(i);
		char Path[1024] = {};
		i3::snprintf(Path, 1024, "%s/%s", szBase, pRef->m_szName );

		i3FileStream	DestStream;
		if ( DestStream.Create( Path, STREAM_WRITE ) == false)
		{
			failedCount++;
			continue;
		}

		currThreadStream->SetPosition(pRef->m_Offset);

		if (pRef->m_Attr & I3_PACK_FILE_ATTR_SECURE)	// 이 경우에 대한 익스포트가 없어서 만든다.(2014.10.24.수빈)
		{
			UINT32 rc = 0;
			INT32 j, sz;
			char buff[ READ_UNIT];
			
			for( j = 0; j < (INT32) pRef->m_Size;)
			{
				sz = min( READ_UNIT, (pRef->m_Size - j));

				rc += currThreadStream->Read( buff, sz);

				BitRotateDecript( (UINT8 *) buff, sz, 3);

				if( DestStream.Write( buff, sz) == STREAM_ERR)
				{
					++failedCount;	break;					
				}
				j += sz;
			}		
		}
		else
		{
			if (DestStream.CopyFrom( currThreadStream,  pRef->m_Size) == STREAM_ERR)
				++failedCount;
		}
	}

	for ( INT32 i = 0 ; i < pNode->getChildCount() ; ++i )
	{
		i3PackNode * pChild = (i3PackNode*)pNode->getChild(i);
		failedCount += Export_Rec(currThreadStream, pChild, szBase);
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

	I3ASSERT( file.getKeyObject() != nullptr);
	I3ASSERT( i3::same_of<i3PackNode*>(file.getKeyObject())) ;

	i3PackNode * pNode = (i3PackNode *) file.getKeyObject();

	I3_REF_CHANGE( m_pRoot, pNode);

#ifdef _DEBUG
	DWORD dwThreadID = i3::get_curr_tls_thread_id();
	I3ASSERT(dwThreadID == 0);
#endif

	m_strVolumeFilename = pszPath;
	
	return true;
}

UINT32 i3PackVolume::Preview( const char* pszPath)
{
	bool Rv;
	UINT32 Rc = 0, Result = 0;
	bool bOldVersion = false;

	m_Preview.filename = pszPath;

#ifdef _DEBUG
	DWORD dwThreadID = i3::get_curr_tls_thread_id();
	I3ASSERT(dwThreadID == 0);
#endif

	i3FileStream * currThreadStream = this->getCurrThreadStream();

	Rv = currThreadStream->Open( pszPath, STREAM_READ | STREAM_SHAREREAD);

	if( Rv == false)
	{
		i3Log( "i3PackVolume::Preview", "Could not open %s file in Preview.", pszPath);
		return STREAM_ERR;
	}

	UINT8	id[4];

	Rc = currThreadStream->Read( id, 4);
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not read file-identifier in Preview.");
		currThreadStream->Close();
		return STREAM_ERR;
		
	}
	Result += Rc;

	if( memcmp( id, I3_RSCFILE_HEAD_10, 4) == 0)
		bOldVersion = true;

	// Preview Header Information

	if( bOldVersion)
	{
		i3::pack::RESOURCE_FILE_HEADER1 OldHeader;

		Rc = currThreadStream->Read( &OldHeader, sizeof( OldHeader));
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not read header(version 1.0f) information in Preview.");
			currThreadStream->Close();
			return STREAM_ERR;
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
		Rc = currThreadStream->Read( &m_Preview.header, sizeof(m_Preview.header));
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not read header information in Preview.");
			currThreadStream->Close();
			return STREAM_ERR;
		}
		Result += Rc;
	}

	currThreadStream->Close();

	return Result;
}

void i3PackVolume::DumpVolume()
{
	if( m_pRoot == nullptr)
	{
		I3TRACE( "	-- not files \n");
		return;
	}

	I3TRACE( "	-- Root Address : (0x%p) (i3PackVolume)\n", m_pRoot);
	m_pRoot->DumpPack();
}

i3FileStream*		i3PackVolume::getMainThreadStream() const
{
	return m_tlsFileStream.get_raw_by_thread_id_unsafe(0);			// 0번쓰레드가 메인쓰레드임! 널값일수 있기 때문에 미리 로딩된걸 가정해야함.
}

//
// 메인쓰레드에 파일로딩이 되있으면 서브스레드쪽은 접근시에 각각 지연 로딩....
// 메인쓰레드에서 파일로딩이 되있지 않으면 그냥 빈 스트림 채로 리턴.
// 메인쓰레드쪽 클로즈의 경우는 i3PackVolume객체 소거전까지는 없기 때문에 일단 놔두도록 한다!...
//
i3FileStream *		i3PackVolume::getCurrThreadStream(void) const
{
	return m_tlsFileStream.get();
}
