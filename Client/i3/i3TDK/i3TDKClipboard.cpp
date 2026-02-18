#include "stdafx.h"
#include "i3TDKClipboard.h"

#define	I3_CLIPBOARDFORMAT			_T("i3Engine_Clipboard")

static UINT		s_idFormat = 0;

void i3TDKClipboard::Copy( HWND hwnd, i3PersistantElement * pObj)
{
	HANDLE hMem;
	UINT32 Size;

	// pObj가 할당 해제되는 것을 방지하기 위해...
	I3_MUST_ADDREF( pObj);

	// 저장할 Memory의 크기를 구한다.
	{
		i3NullStream strm;
		i3ResourceFile file;
		
		// Null Stream을 이용해, 파일에 저장하는 흉내를 내며
		// 크기를 구한다.
		file.setKeyObject( pObj); 
		file.Save( &strm);	// 파일명은

		Size = strm.GetSize() + 64;		// Class Meta Name을 저장할 여유 공간(64 bytes)를 추가한다.
	}

	// 실제 메모리를 할당하고 저장한다.
	{
		i3MemStream strm;
		i3ResourceFile file;
		char * pBuf;

		hMem = GlobalAlloc( GMEM_DDESHARE, Size);
		I3ASSERT( hMem != INVALID_HANDLE_VALUE);

		pBuf = (char *) GlobalLock( hMem);

		// Key Object의 Meta Name을 문자열로 저장해 둠.
		strncpy( pBuf, pObj->meta()->class_name().c_str(), Size);

		strm.Open( (const char *) (pBuf + 64), STREAM_READ | STREAM_SHAREREAD | STREAM_WRITE, Size);
		
		file.setKeyObject( pObj);
		file.Save( &strm);

		GlobalUnlock( hMem);
	}

	if( OpenClipboard( hwnd) == FALSE)
	{
		GlobalFree( hMem);
		I3_MUST_RELEASE( pObj);
		return;
	}

	EmptyClipboard();

	//if( s_idFormat == nullptr)
	{
		s_idFormat = RegisterClipboardFormat( I3_CLIPBOARDFORMAT);
		if( s_idFormat == 0)
		{
			GlobalFree( hMem);
			goto ExitPart;
		}
	}

	if( SetClipboardData( s_idFormat, hMem) == nullptr)
	{
		DebugBox( nullptr, "Could not set clipboard data.\n");
	}

ExitPart:
	I3_MUST_RELEASE( pObj);

	CloseClipboard();
}

bool i3TDKClipboard::IsObject( i3ClassMeta * pMetaArg)
{
	HANDLE hMem = nullptr;
	bool bResult = false;

	// Clipboard에서 데이터를 읽어 온다.
	{
		if( OpenClipboard( nullptr) == FALSE)
			return false;

		if( s_idFormat == 0)
		{
			s_idFormat = RegisterClipboardFormat( I3_CLIPBOARDFORMAT);
			if( s_idFormat == 0)
				goto ExitPart;
		}

		hMem = GetClipboardData( s_idFormat);
		if( hMem == nullptr)
			goto ExitPart;

		{
			char * pBuf = (char *) GlobalLock( hMem);
			i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(pBuf);
			if( pMeta == nullptr)
				goto ExitPart;

			if( pMeta->kind_of(pMetaArg))
				bResult = true;
		}
	}

ExitPart:
	if( hMem != nullptr)
		GlobalUnlock( hMem);

	CloseClipboard();

	return bResult;
}

i3PersistantElement * i3TDKClipboard::Paste( HWND hwnd)
{
	i3ResourceFile file;
//	List ShapeList;						쓰이지 않아 주석..
	i3PersistantElement * pObj = nullptr;

	// Clipboard에서 데이터를 읽어 온다.
	{
		HANDLE hMem;
		UINT32 rc;

		if( OpenClipboard( hwnd) == FALSE)
			return nullptr;

		s_idFormat = RegisterClipboardFormat( I3_CLIPBOARDFORMAT);
		if( s_idFormat == 0)
		{
			goto ExitPart;
		}

		hMem = GetClipboardData( s_idFormat);
		if( hMem == nullptr)
		{
			DebugBox( nullptr, "Clipboard에 데이터가 없습니다.");
			goto ExitPart;
		}

		{
			char * pBuf = (char *) GlobalLock( hMem);
			i3MemStream strm;

			strm.AddRef();

			strm.Open( (pBuf + 64), STREAM_READ | STREAM_SHAREREAD, 0x7FFFFFFF);

			rc = file.Load( &strm);
			if( rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Clipboard에서 데이터를 읽어오지 못했습니다.");
			}
			else
			{
                pObj = file.getKeyObject();
				I3_MUST_ADDREF(pObj);
			}

			GlobalUnlock( hMem);
		}
	}

ExitPart:

	CloseClipboard();

	return pObj;
}
