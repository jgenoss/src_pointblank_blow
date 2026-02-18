#include "stdafx.h"
#include "i3LevelScene.h"
#include "i3LevelUndoInfoEdit.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelUndoInfoEdit);

i3LevelUndoInfoEdit::~i3LevelUndoInfoEdit(void)
{
	I3MEM_SAFE_FREE( m_pObjBuf);
}

UINT32 i3LevelUndoInfoEdit::__MakeDataBuf( i3LevelElement * pElement, char **ppBuf)
{
	i3NullStream	nStream;
	UINT32			bufSize;

	// 파일에 저장될 사이즈를 구하고
	{
		i3ResourceFile file;
		file.setKeyObject(pElement);
		file.Save(&nStream);
	}

	bufSize = nStream.GetSize() + 64; // 64: Classmeta를 위한 공간

	// 실제 메모리를 할당하고 저장한다.
	{
		i3MemStream * pStream = i3MemStream::new_object();
		i3ResourceFile file;

		char * pBuf = *ppBuf = (char*)i3MemAlloc( bufSize);

		// Key Object의 Meta Name을 문자열로 저장해 둠.
		strncpy( pBuf, pElement->meta()->class_name().c_str(), bufSize);

		// 문자열 이후부터 오브젝터 저장 크기만큼 저장
		pStream->Open( (const char *) (pBuf + 64), STREAM_READ | STREAM_SHAREREAD | STREAM_WRITE, bufSize);

		file.setKeyObject( pElement);
		file.Save( pStream);

		I3_SAFE_RELEASE( pStream);
	}

	return bufSize;
}

i3LevelElement * i3LevelUndoInfoEdit::__LoadDataBuf( char * pBuf, UINT32 size)
{
	char szMeta[64];
	i3::string_ncopy_nullpad( szMeta, (char*)pBuf, 64);

	i3MemStream * pStream = i3MemStream::new_object();
	i3ResourceFile File;
	UINT32 rc;

	pStream->Open( pBuf + 64, STREAM_READ | STREAM_SHAREREAD, size - 64);
	rc = File.Load( pStream);
	if( rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_NOTICE, "이전 데이터 읽어들이기 실패");
		return nullptr;
	}

	i3LevelElement * pElm = (i3LevelElement*)File.getKeyObject();
	I3_MUST_ADDREF(pElm);

	pElm->CopyTo( m_pElement, I3_COPY_INSTANCE);

	I3_SAFE_RELEASE( pElm);
	I3_SAFE_RELEASE( pStream);

	return nullptr;
}

void i3LevelUndoInfoEdit::setObject( i3LevelElement * pElement)
{
	I3_REF_CHANGE(m_pElement, pElement);

	if( m_pElement == nullptr)	return;

	m_nBufSize = __MakeDataBuf( m_pElement, &m_pObjBuf);			//복원할 데이터를 저장
}

bool i3LevelUndoInfoEdit::Undo(i3LevelScene * pScene)
{
	char * pBuf = m_pObjBuf;
	UINT32 size = m_nBufSize;

	m_nBufSize = __MakeDataBuf( m_pElement, &m_pObjBuf);			// 현재 데이터 저장

	__LoadDataBuf( pBuf, size);										// 이전 데이터 복원

	I3MEM_SAFE_FREE( pBuf);											// 이전 데이터 버퍼 삭제

	i3LevelUndoInfo::Undo(pScene);
	return true;
}

bool i3LevelUndoInfoEdit::Redo(i3LevelScene * pScene)
{
	char * pBuf = m_pObjBuf;
	UINT32 size = m_nBufSize;

	m_nBufSize = __MakeDataBuf( m_pElement, &m_pObjBuf);			// 현재 데이터 저장

	__LoadDataBuf( pBuf, size);										// 이전 데이터 복원

	I3MEM_SAFE_FREE( pBuf);											// 이전 데이터 버퍼 삭제

	i3LevelUndoInfo::Redo( pScene);

	return false;
}