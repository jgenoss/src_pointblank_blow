#include "i3CommonType.h"
#include "i3PackFile.h"
#include "i3CRC32.h"

I3_CLASS_INSTANCE( i3PackFile, i3ElementBase);

i3PackFile::i3PackFile(void)
{
	m_pFileInfo = NULL;
	m_pPackNode = NULL;
	m_pVol = NULL;
	m_CurPos = 0;
}

i3PackFile::~i3PackFile(void)
{
	I3_SAFE_RELEASE( m_pPackNode);
	I3_SAFE_RELEASE( m_pVol);
}

bool i3PackFile::Open( i3PackVolume * pVol, i3PackNode * pPackNode, I3_PACK_FILE_REF * pInfo)
{
	I3_REF_CHANGE( m_pVol, pVol);
	I3_REF_CHANGE( m_pPackNode, pPackNode);

	m_pFileInfo = pInfo;
	m_CurPos = 0;

	return true;
}

#define READ_UNIT		2048

UINT32 i3PackFile::Read( void * pBuff, UINT32 sz)
{
	UINT32 rc = STREAM_ERR;
	I3ASSERT( m_pVol != NULL);
	char * pDest = (char *) pBuff;

	i3FileStream * pFile = m_pVol->getStream();

	if( (m_pFileInfo->m_Attr & I3_PACK_FILE_ATTR_SECURE) == 0)
	{
		pFile->SetPosition( m_pFileInfo->m_Offset + m_CurPos, STREAM_BEGIN);

		// 파일의 크기를 벗어나서 읽으려는 경우, 읽는 크기 조절
		if( (m_CurPos + sz) > GetSize())
			sz -= (m_CurPos + sz) - GetSize();

		rc = pFile->Read( pBuff, sz);
	}
	else
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
				I3TRACE( "Could not read pack file.\n");
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

UINT32 i3PackFile::GetPosition(void)
{
	return m_CurPos;
}

UINT32 i3PackFile::SetPosition( INT32 pos, INT32 origin)
{
	switch( origin)
	{
		case STREAM_BEGIN :			m_CurPos = pos;		break;
		case STREAM_CURRENT :		m_CurPos += pos;	break;
		case STREAM_END :			m_CurPos = m_pFileInfo->m_Size - pos; break;
	}

	i3FileStream * pFile = m_pVol->getStream();

	pFile->SetPosition( m_pFileInfo->m_Offset + m_CurPos, STREAM_BEGIN);

	return m_CurPos;
}
