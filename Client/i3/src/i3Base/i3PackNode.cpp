#include "i3CommonType.h"
#include "i3PackNode.h"
#include "i3String.h"
#include "i3CRC32.h"

#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/safe_string_copy.h"
#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/generic_string_cat.h"


#include "i3ResourceFile_Pack.h"
#include "i3PackVolume.h"


void i3ResourceFile_Pack::GetFileCount(i3PackNode* node, int& out_num )
{
	out_num += node->getFileCount();
	const INT32 numChild = node->getChildCount();
	for ( int i = 0 ; i < numChild ; ++i )
	{
		GetFileCount((i3PackNode*)node->getChild(i), out_num);
	}
}

i3ResourceFile_Pack::i3ResourceFile_Pack(i3PackNode* root, i3PackFileProgressCallback* cb) : 
	m_curr_file_count(0), m_total_files(0), m_cb(cb), m_success_count(0)
{
	if (root)
		GetFileCount(root, m_total_files);
}

void		i3ResourceFile_Pack::OnProgress_Start() 
{
	m_cb->OnProgress_Start(m_total_files);
}

void		i3ResourceFile_Pack::OnProgress_Write_ContentsFile_Start( const char* szResFilePath, bool is_enc )
{
	m_cb->OnProgress_BeforePack(szResFilePath, is_enc, ++m_curr_file_count, m_total_files);		
}

void		i3ResourceFile_Pack::OnProgress_Write_ContentsFile_End( const char* szResFilePath, bool is_enc, bool is_success )
{
	if (is_success) ++m_success_count;

	m_cb->OnProgress_AfterPack(szResFilePath, is_enc, is_success, m_curr_file_count, m_success_count, m_total_files);
}

void		i3ResourceFile_Pack::OnProgress_End()
{
	m_cb->OnProgress_End(m_curr_file_count, m_success_count, m_total_files);
}



I3_CLASS_INSTANCE( i3PackNode);

i3PackNode::~i3PackNode(void)
{
	I3MEM_SAFE_FREE( m_pFiles);
}

void i3PackNode::GetFullPath( char * pszBuf)
{
	if( getParent() != nullptr)
	{
		i3PackNode * pParent = (i3PackNode *) getParent();

		pParent->GetFullPath( pszBuf);
	}

	if( hasName() && i3::generic_is_equal( GetNameString(), "/") == false)
	{
		if( pszBuf[0] != 0)
		{
			i3::generic_string_cat( pszBuf, "\\");
		}

		i3::generic_string_cat( pszBuf, GetNameString());
	}
}

UINT32 i3PackNode::getNameCode( const char * pszName)
{
	char szName[MAX_PACK_FILENAME_NEW] = {};

	I3ASSERT( pszName != nullptr);

	i3::safe_string_copy( szName, pszName, MAX_PACK_FILENAME_NEW);
	strupr( szName);

	UINT32 len = i3::generic_string_size( szName);

	I3ASSERT( len < MAX_PACK_FILENAME_NEW);

	return CRC32( 0xFFFFFFFF, (UINT8 *) szName, len);
}

INT32 i3PackNode::Find( const char * pszName)
{
	INT32 i;
	UINT32		code = getNameCode( pszName);

	for( i = 0; i < m_FileCount; i++)
	{
		if( m_pFiles[i].m_codeName == code)
			return i;
	}

	return -1;
}

void i3PackNode::SetFileCount( INT32 cnt)
{
	m_FileCount = cnt;

	if( cnt == 0)
	{
		I3MEM_SAFE_FREE_POINTER(m_pFiles);
	}
	else
	{
		if( m_pFiles == nullptr)
		{
			m_pFiles = (I3_PACK_FILE_REF *) i3MemAlloc( sizeof(I3_PACK_FILE_REF) * cnt);
		}
		else
		{
			m_pFiles = (I3_PACK_FILE_REF *) i3MemReAlloc( m_pFiles, sizeof(I3_PACK_FILE_REF) * cnt);
		}
	}
}

INT32 i3PackNode::AddFile( const char * pszName, UINT32 attr, UINT32 Size, UINT32 Offset)
{
	INT32 idx = m_FileCount;

	SetFileCount( m_FileCount + 1);

	m_pFiles[idx].m_codeName = getNameCode( pszName);
	i3::safe_string_copy( m_pFiles[idx].m_szName, pszName, MAX_PACK_FILENAME_NEW);
	m_pFiles[idx].m_Attr = attr;
	m_pFiles[idx].m_Size = Size;
	m_pFiles[idx].m_Offset = Offset;

	return idx;
}

void i3PackNode::RemoveFile( const char * pszName)
{
	INT32 idx = Find( pszName);
	INT32 moveCnt;

	I3ASSERT( idx != -1);

	moveCnt = (m_FileCount - idx) - 1;

	if( moveCnt > 0)
	{
		memmove( & m_pFiles[ idx], & m_pFiles[idx + 1], moveCnt * sizeof( I3_PACK_FILE_REF));
	}

	m_FileCount--;
}

i3PackNode * i3PackNode::FindPath( const char * pszPath, I3_PACK_FILE_REF ** ppFileInfo)
{
	char conv[ 256] = "";
	INT32 i;

	// Path 구분자가 발견될 때까지 문자열 검색
	for( i = 0; *pszPath != 0; i++, pszPath++)
	{
		if( (*pszPath == '/') || (*pszPath == '\\') || (MAX_PACK_FILENAME_NEW <= i))
			break;

		conv[i] = *pszPath;  // pszPath가 MAX_PACK_FILENAME_NEW 길이를 넘어갈때 buffer overrun 문제가 나올수 있다. komet
	}

	conv[i] = 0;

	// i3PackNode의 최대 MAX_PACK_FILENAME_NEW 길이의 name을 넘어갈 경우 문제가 될수 있다.
	if( i3::generic_string_size( conv) >= MAX_PACK_FILENAME_NEW)
	{
		char tmp[256] = "";

		sprintf( tmp, "Could not find path - %d over maximum length of pack file name (%s)\n", MAX_PACK_FILENAME_NEW, conv);
		I3PRINTLOG(I3LOG_FATAL,  tmp);
	}

	if( *pszPath == 0)
	{
		// nullptr로 종료한 경우, 파일명이라고 판단.
		INT32 idx = Find( conv);

		if( idx != -1)
		{
			*ppFileInfo = getFile( idx);

			return this;
		}
	}

	{
		// Path로 판단.
		// Child Node 중에서 검색
		INT32 i;
		i3PackNode * pChild;

		for( i = 0; i < getChildCount(); i++)
		{
			pChild = (i3PackNode *) getChild( i);

			if( i3::generic_is_iequal( pChild->GetNameString(), conv) )
			{
				if( *pszPath == 0)
				{
					// 원래부터 디렉토리를 검색하려 했다.
					*ppFileInfo = nullptr;
					return pChild;
				}
				else
				{
					return pChild->FindPath( pszPath + 1, ppFileInfo);
				}
			}
		}
	}

	return nullptr;
}

#define		ENC_UNIT		2048

UINT32 i3PackNode::saveFile( i3Stream * pDest, i3Stream * pSrc, I3_PACK_FILE_REF * pFile)
{
	UINT32 rc = 0;

	if((pFile->m_Attr & I3_PACK_FILE_ATTR_SECURE) == 0)
	{
		//일반 파일
		rc = pDest->CopyFrom( pSrc, pFile->m_Size);
	}
	else
	{
		// Security
		INT32 i, sz;
		char buff[ ENC_UNIT];

		for( i = 0; i < (INT32) pFile->m_Size;)
		{
			sz = min( ENC_UNIT, (pFile->m_Size - i));

			rc += pSrc->Read( buff, sz);

			BitRotateEncript( (UINT8 *) buff, sz, 3);

			if( pDest->Write( buff, sz) == STREAM_ERR)
				return STREAM_ERR;

			i += sz;
		}
	}

	return rc;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 NODE_HEADER
	{
		UINT8			m_ID[4] = { 'P', 'F', 'N', '2' };
		UINT32			m_Count = 0;
	};

	struct FILE_REF
	{
		char			m_szName[MAX_PACK_FILENAME] = { 0 };
		UINT32			m_Attr = 0;
		UINT16			m_codeName0 = 0;
		UINT16			m_Size0 = 0;
		UINT16			m_Offset1 = 0;
		UINT16			m_Size1 = 0;
		UINT16			m_Checksum1 = 0;
		UINT16			m_codeName1 = 0;
		UINT16			m_Offset0 = 0;
		UINT16			m_Checksum0 = 0;
		UINT32			pad[2] = { 0 };
	};

	struct FILE_REF2 {
		char			m_szName[MAX_PACK_FILENAME_NEW] = { 0 };
		UINT32			m_Attr = 0;
		UINT16			m_codeName0 = 0;
		UINT16			m_Size0 = 0;
		UINT16			m_Offset1 = 0;
		UINT16			m_Size1 = 0;
		UINT16			m_Checksum1 = 0;
		UINT16			m_codeName1 = 0;
		UINT16			m_Offset0 = 0;
		UINT16			m_Checksum0 = 0;
		UINT32			pad[2] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#define I3_PACK_HEADER_ENC			3
#define I3_PACK_FILE_ENC			2

UINT32 i3PackNode::OnSave( i3ResourceFile * pResFile)
{
	pack::NODE_HEADER header;
	INT32 i;
	pack::FILE_REF2 data;
	UINT32 Rc, Result = 0, FATPos, LastPos;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3TreeNode::OnSave( pResFile);
	I3_CHKIO( Result);

	header.m_Count = getFileCount();

	BitRotateEncript( (UINT8 *) &header, sizeof(header), I3_PACK_HEADER_ENC);

	Rc = pResFile->Write( &header, sizeof(header));
	I3_CHKIO( Rc);
	Result += Rc;

	// File Table을 저장할 공간 확보
	{
		FATPos = pStream->GetPosition();

		pStream->SetPosition( sizeof(pack::FILE_REF2) * getFileCount(), STREAM_CURRENT);
	}

	{
		i3ResourceFile_Pack* pResFile_Pack = static_cast<i3ResourceFile_Pack*>(pResFile);

		char szBase[MAX_PATH], szFull[ MAX_PATH];
		i3FileStream file;

		szBase[0] = 0;
		GetFullPath( szBase);

        for( i = 0; i < getFileCount(); i++)
		{
			sprintf( szFull, "%s/%s", szBase, m_pFiles[i].m_szName);

			pResFile_Pack->OnProgress_Write_ContentsFile_Start(szFull, ( m_pFiles[i].m_Attr & I3_PACK_FILE_ATTR_SECURE ) != 0 );

			if( file.Open( szFull, STREAM_READ | STREAM_SHAREREAD) == false)
			{
				I3TRACE( " - Could not open %s file for merging.\n", szFull);

				pResFile_Pack->OnProgress_Write_ContentsFile_End(szFull, ( m_pFiles[i].m_Attr & I3_PACK_FILE_ATTR_SECURE ) != 0, false);

				continue;		// 저장은 계속 되어야 한다.
			}

			m_pFiles[i].m_Offset = pStream->GetPosition();
			m_pFiles[i].m_Size = file.GetSize();

			saveFile( pStream, &file, (I3_PACK_FILE_REF *) &m_pFiles[i]);

			file.Close();

			pResFile_Pack->OnProgress_Write_ContentsFile_End(szFull, ( m_pFiles[i].m_Attr & I3_PACK_FILE_ATTR_SECURE ) != 0, true);
		}
	}

	LastPos = pStream->GetPosition();

	pStream->SetPosition( FATPos, STREAM_BEGIN);

	//
	for( i = 0; i < getFileCount(); i++)
	{
		i3mem::FillZero( &data, sizeof(data));
		i3::safe_string_copy( data.m_szName, m_pFiles[i].m_szName, sizeof(data.m_szName) );
		data.m_Attr				= m_pFiles[i].m_Attr;
		data.m_codeName0		= LO_16( m_pFiles[i].m_codeName);			// Security를 위한 Shuffle
		data.m_codeName1		= HI_16( m_pFiles[i].m_codeName);
		data.m_Size0			= LO_16( m_pFiles[i].m_Size);
		data.m_Size1			= HI_16( m_pFiles[i].m_Size);
		data.m_Offset0			= LO_16( m_pFiles[i].m_Offset);
		data.m_Offset1			= HI_16( m_pFiles[i].m_Offset);

		BitRotateEncript( (UINT8 *) &data, sizeof(data), I3_PACK_FILE_ENC);

		Rc = pResFile->Write( &data, sizeof(data));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	pStream->SetPosition( LastPos, STREAM_BEGIN);

	return Result;
}

UINT32 i3PackNode::OnLoad( i3ResourceFile * pResFile)
{
	pack::NODE_HEADER header;
	INT32 i;
	UINT32 Rc, Result = 0;

	Result = i3TreeNode::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL, "FileName(%s)", pResFile->getFileName());
		return STREAM_ERR;
	}

	Rc = pResFile->Read( &header, sizeof( header));
	I3_CHKIO( Rc);
	Result += Rc;

	bool bNewVersion = false;

	BitRotateDecript( (UINT8 *) &header, sizeof(header), I3_PACK_HEADER_ENC);

	if( memcmp( header.m_ID, "PFN2", 4) == 0)
	{
		bNewVersion = true;
	}

	if( header.m_Count > 0)
	{
		SetFileCount( header.m_Count);

		if( bNewVersion)
		{
			pack::FILE_REF2 file;
			for( i = 0; i < getFileCount(); i++)
			{
				Rc = pResFile->Read( & file, sizeof(file));
				I3_CHKIO( Rc);
				Result += Rc;

				BitRotateDecript( (UINT8 *) &file, sizeof(file), I3_PACK_FILE_ENC);

				i3::safe_string_copy( m_pFiles[i].m_szName, file.m_szName, sizeof(m_pFiles[i].m_szName) );
				m_pFiles[i].m_codeName		=	getNameCode( file.m_szName);
				m_pFiles[i].m_Attr			= file.m_Attr;
				m_pFiles[i].m_Size			= MAKE_32( file.m_Size0, file.m_Size1);
				m_pFiles[i].m_Offset		= MAKE_32( file.m_Offset0, file.m_Offset1);
			}
		}
		else
		{
			pack::FILE_REF file;
			for( i = 0; i < getFileCount(); i++)
			{
				Rc = pResFile->Read( & file, sizeof(file));
				I3_CHKIO( Rc);
				Result += Rc;

				BitRotateDecript( (UINT8 *) &file, sizeof(file), I3_PACK_FILE_ENC);

				i3::safe_string_copy( m_pFiles[i].m_szName, file.m_szName, sizeof(m_pFiles[i].m_szName) );
				m_pFiles[i].m_codeName		=	getNameCode( file.m_szName);
				m_pFiles[i].m_Attr			= file.m_Attr;
				m_pFiles[i].m_Size			= MAKE_32( file.m_Size0, file.m_Size1);
				m_pFiles[i].m_Offset		= MAKE_32( file.m_Offset0, file.m_Offset1);
			}
		}
	}

	return Result;
}

void i3PackNode::DumpPack()
{
	INT32 i;
	char buf[MAX_PATH] = {0,};
	GetFullPath( buf); 
		
	I3TRACE( "		-- FullPath : %s (i3PackNode)\n", buf);
	
	for( i = 0; i < m_FileCount; ++i)
	{
		I3TRACE( "			-- File(%d) : %s\n", i, (m_pFiles+i)->m_szName);
	}

	
	i3PackNode* pNode;
	for( i = 0; i < getChildCount(); ++i)
	{
		pNode = (i3PackNode*)getChild( i);
		pNode->DumpPack();
	}
}
