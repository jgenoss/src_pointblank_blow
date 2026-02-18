#include "i3CommonType.h"
#include "i3PackFile.h"
#include "i3CRC32.h"
#include "i3StreamMem.h"

I3_CLASS_INSTANCE( i3PackFile);

//
// 소멸자 처리만큼은 서브쓰레드에 맞춰서 신경 써줄까?
// 서브스레드에서의 스트림은 PackFile 제거시 함께 제거되는 방향으로 가도 좋을것 같다..
//
I3_EXPORT_BASE
i3PackFile::~i3PackFile(void)
{
	I3_SAFE_RELEASE(m_StreamForENC);
	I3_SAFE_RELEASE( m_pPackNode);

}

#define READ_UNIT		2048

I3_EXPORT_BASE
bool i3PackFile::Open(i3PackVolume* pVolume, i3PackNode * pPackNode, I3_PACK_FILE_REF * pInfo)
{
	i3FileStream* pCurrThreadStream = pVolume->getCurrThreadStream();
	
	if (pCurrThreadStream->IsOpened() == false)			// 안 열려있음...(기대 동작)
	{
		// 쓰레드 별로 Open처리를 해주기로 한다..(읽기 전용임에 유의하자)
		// 게임프로그램이 종료전까지는 Close() 없이 파일포인터가 유지되도록 일단 처리했다.

		const i3::rc_string& strFileName = pVolume->getVolumeFilename(); 

		if (strFileName.empty())
		{
			DWORD tlsCurrThreadID = i3::get_curr_tls_thread_id();
			I3TRACE("VolumeFilename Empty ThreadID = %d \n", tlsCurrThreadID);
			return false;
		}

		if (pCurrThreadStream->Open(strFileName, STREAM_READ | STREAM_SHAREREAD) == false)
		{
			DWORD tlsCurrThreadID = i3::get_curr_tls_thread_id();
			I3TRACE("Open Error On SubThread : %s  ThreadID = %d \n", strFileName.c_str(), tlsCurrThreadID);
			return false;
		}
	}

	m_pCurrThreadStream = pCurrThreadStream;

	I3_REF_CHANGE( m_pPackNode, pPackNode);

	m_pFileInfo = pInfo;
	m_CurPos = 0;

	// Decryption 알고리즘도 속도저하문제가 있지만...각 OnRead함수별로 Decryption이 2048단위로 엄청나게 읽어대는 통에...
	// 퍼포먼스 이슈가 더 커짐...
	// 어차피 i3ResourceFile을 경유한 로딩이라면 메모리 보호같은것은 필요없다고 판단되므로...
	// Open시도때 일괄 Decryption처리하고 i3MemStream으로 들고다니는게 더 이익일것 같다...
	//
	if( m_pFileInfo->m_Attr & I3_PACK_FILE_ATTR_SECURE )
	{
		// Security
		m_StreamForENC = i3MemStream::new_object();

		if ( !m_StreamForENC->Open(nullptr, STREAM_READ|STREAM_WRITE, m_pFileInfo->m_Size) )
		{
			m_StreamForENC->Release();	m_StreamForENC = nullptr;
			return true;
		}
				
		i3FileStream * pSrcFile = m_pCurrThreadStream;

		UINT32 oldPos = m_CurPos;
		UINT32 rc = 0;

		pSrcFile->SetPosition( m_pFileInfo->m_Offset, STREAM_BEGIN);

		INT32 i, sz;
		char buff[ READ_UNIT];

		for( i = 0; i < (INT32) m_pFileInfo->m_Size;)
		{
			sz = min( READ_UNIT, (m_pFileInfo->m_Size - i));

			rc += pSrcFile->Read( buff, sz);

			BitRotateDecript( (UINT8 *) buff, sz, 3);
	
			if( m_StreamForENC->Write( buff, sz) == STREAM_ERR)
			{
				m_StreamForENC->Release();	m_StreamForENC = nullptr;
				return true;
			}

			i += sz;
		}		
		
		pSrcFile->SetPosition( oldPos, STREAM_BEGIN);
	}
	
	return true;
}

I3_EXPORT_BASE
UINT32 i3PackFile::Read( void * pBuff, UINT32 sz)
{
	UINT32 rc = STREAM_ERR;
//	I3ASSERT( m_pVol != nullptr);
	char * pDest = (char *) pBuff;

	i3FileStream * pFile = m_pCurrThreadStream;

	if( (m_pFileInfo->m_Attr & I3_PACK_FILE_ATTR_SECURE) == 0)
	{
		pFile->SetPosition( m_pFileInfo->m_Offset + m_CurPos, STREAM_BEGIN);

		// 파일의 크기를 벗어나서 읽으려는 경우, 읽는 크기 조절
		if( (m_CurPos + sz) > GetSize())
			sz -= (m_CurPos + sz) - GetSize();

		rc = pFile->Read( pBuff, sz);
		if( rc == STREAM_ERR)
		{
			I3TRACE( "Could not read pack file.1\n");
			return STREAM_ERR;
		}
	}
	else if ( m_StreamForENC != nullptr)
	{
		m_StreamForENC->SetPosition( m_CurPos, STREAM_BEGIN);
		if( (m_CurPos + sz) > GetSize())
			sz -= (m_CurPos + sz) - GetSize();
		
		rc = m_StreamForENC->Read(pBuff, sz);
		if( rc == STREAM_ERR)
		{
			I3TRACE( "Could not read pack file.2\n");
			return STREAM_ERR;
		}
	}
	else		// memStream으로 처리하지 못한 경우..기존 ENC 루틴으로..
	{
		UINT32 oldPos = m_CurPos;
		UINT32 idx1, idx2, i, readsz, mod, copysz, curPos;
		char buff[ READ_UNIT];

		// Encript는 2048 block 단위로 이루어져 있으며, 읽으려는 범위에 따라 
		idx1 = m_CurPos / READ_UNIT;
		idx2 = (m_CurPos + sz) / READ_UNIT;

		mod = m_CurPos % READ_UNIT;

		curPos = idx1 * READ_UNIT;

		pFile->SetPosition( m_pFileInfo->m_Offset + curPos, STREAM_BEGIN);

		// 파일의 크기를 벗어나서 읽으려는 경우, 읽는 크기 조절
		if( (m_CurPos + sz) > GetSize())
			sz -= (m_CurPos + sz) - GetSize();

		//I3TRACE( "---------- PACK : RQ size : %d\n", sz);

		for( i = 0; i < sz; )
		{
			readsz = MIN( READ_UNIT, GetSize() - curPos);

			rc = pFile->Read( buff, readsz);
			if( rc == STREAM_ERR)
			{
				I3TRACE( "Could not read pack file.3\n");
				return STREAM_ERR;
			}

			BitRotateDecript( (UINT8 *) buff, readsz, 3);

			copysz = MIN( (readsz - mod), (sz - i));
			i3mem::Copy( pDest, buff + mod, copysz);

			//I3TRACE( "PACK : readsz:%d  copysz:%d  mod:%d  i:%d\n", readsz, copysz, mod, i);

			pDest += copysz;
			i += copysz;
			curPos += readsz;

			// 첫번째 loading에서만 mod가 있을 필요가 있기 때문에...
			mod = 0;
		}

		pFile->SetPosition( oldPos, STREAM_BEGIN);
	}

	rc = sz;
	m_CurPos += sz;

	return rc;
}

I3_EXPORT_BASE
UINT32 i3PackFile::GetPosition(void)
{
	return m_CurPos;
}

I3_EXPORT_BASE
UINT32 i3PackFile::SetPosition( INT32 pos, INT32 origin)
{
	switch( origin)
	{
		case STREAM_BEGIN :			m_CurPos = pos;		break;
		case STREAM_CURRENT :		m_CurPos += pos;	break;
		case STREAM_END :			m_CurPos = m_pFileInfo->m_Size - pos; break;
	}
	
	if (m_StreamForENC == nullptr)
	{
		i3FileStream * pFile = m_pCurrThreadStream;
		pFile->SetPosition( m_pFileInfo->m_Offset + m_CurPos, STREAM_BEGIN);
	}
	else
	{
		m_StreamForENC->SetPosition(m_CurPos, STREAM_BEGIN);
	}

	return m_CurPos;
}
