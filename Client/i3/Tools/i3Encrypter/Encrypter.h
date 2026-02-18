#pragma once

#include <string>

class CEncrypter
{
public:
	CEncrypter();
	~CEncrypter();

public:
	const wchar_t*		GetText() const;
	void				SetText( const std::wstring& wsz );

public:
	bool	Load( const std::wstring& path, bool bEncryptFile = false );
	bool	Save( const std::wstring& path, bool bEncryptFile = false );

private:
	void	_BitRotateEncrypt( UINT8 * pBuf, UINT32 Length, UINT32 c );
	void	_BitRotateDecrypt( UINT8 * pBuf, UINT32 Length, UINT32 c );

private:
	std::string	m_txt;
};
