#include "StdAfx.h"
#include "Encrypter.h"

CEncrypter::CEncrypter()
{
}

CEncrypter::~CEncrypter()
{
}

bool	CEncrypter::Load( const std::wstring& path, bool bEncryptFile )
{
	if( path.empty() )
	{
		return false;
	}

	HANDLE hFile = NULL;
	hFile = ::CreateFile( path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( NULL == hFile )
	{
		return false;
	}

	UINT8* pBuffer = NULL;

	DWORD iFileSize;
	DWORD dwSizeLow;
	DWORD dwSizeHigh;
	dwSizeLow = ::GetFileSize( hFile, &dwSizeHigh );
	iFileSize = dwSizeHigh << 16 | dwSizeLow;

	pBuffer = new UINT8[ iFileSize ];

	::ReadFile( hFile, pBuffer, iFileSize, (LPDWORD)&iFileSize, NULL ); 
	::CloseHandle(hFile);

	if( bEncryptFile )
		_BitRotateDecrypt( pBuffer, iFileSize, 7 );

	m_txt.clear();
	for( unsigned int i = 0; i < iFileSize; ++i )
	{
		m_txt += static_cast< char >( pBuffer[i] );
	}

	delete [] pBuffer;
	return true;
}

bool	CEncrypter::Save( const std::wstring& path, bool bEncryptFile )
{
	if( path.empty() )
	{
		return false;
	}

	HANDLE hFile = NULL;
	hFile = ::CreateFile( path.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( NULL == hFile )
	{
		return false;
	}

	unsigned int iFileSize = m_txt.length();	
	UINT8* pBuffer = new UINT8[ iFileSize ];

	for( unsigned int i = 0; i < iFileSize; ++i )
	{
		pBuffer[i] = static_cast< UINT8 >( m_txt.at(i) );
	}

	if( bEncryptFile )
		_BitRotateEncrypt( pBuffer, iFileSize, 7 );

	::WriteFile( hFile, pBuffer, iFileSize, (LPDWORD)&iFileSize, NULL); 
	::CloseHandle(hFile);

	delete [] pBuffer;

	return true;
}

void	CEncrypter::_BitRotateEncrypt( UINT8 * pBuf, UINT32 Length, UINT32 c )
{
	UINT8 start = * pBuf, d1, d2;
	INT32 i, rc;

	rc = 8 - c;

	for( i = 0; i < (INT32) Length; i++)
	{
		if( i < (INT32) (Length - 1))
		{
			d1 = pBuf[i];	d2 = pBuf[ i + 1];
		}
		else
		{
			d1 = pBuf[i]; d2 = start;
		}

		d1 = (UINT8)((d1 << c) & 0xFF);
		d2 = (UINT8)((d2 >> rc) & 0xFF);

		pBuf[i] = (UINT8)(( d1 | d2) & 0xFF);
	}
}

void	CEncrypter::_BitRotateDecrypt( UINT8 * pBuf, UINT32 Length, UINT32 c )
{
	UINT8 d1, d2, last;
	INT32 i, rc;

	rc = 8 - c;

	last = pBuf[ Length - 1];

	for( i = (INT32)Length - 1; i >= 0; i--)
	{
		if( i > 0)
		{
			d1 = pBuf[i];	d2 = pBuf[ i - 1];
		}
		else
		{
			d1 = pBuf[i]; d2 = last;
		}

		d1 = (UINT8)((d1 >> c) & 0xFF);
		d2 = (UINT8)((d2 << rc) & 0xFF);
		pBuf[i] = (UINT8)((d1 | d2) & 0xFF);

	}
}

const wchar_t* CEncrypter::GetText() const
{
	if( m_txt.empty() )
	{
		return L"";
	}

	return _bstr_t( m_txt.c_str() );
}

void CEncrypter::SetText( const std::wstring& wsz )
{
	if( wsz.empty() )
	{
		m_txt = "";
	}
	else
	{
		m_txt = _bstr_t( wsz.c_str() );
	}
}
