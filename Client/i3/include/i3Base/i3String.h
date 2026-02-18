// i3String.h

#if !defined( __I3_STRINGTOOLS_H)
#define __I3_STRINGTOOLS_H


struct I3_SYMBOLTABLE_INFO
{
	char	m_szSymbol[64];
	char	m_szValue[256];
} ;

namespace i3String
{
	////////////////////////////////////////////////////////////////////////////////////////
	I3_EXPORT_BASE void	PreprocessMacro( char * pszDest, INT32 Length, const char * pszSrc, I3_SYMBOLTABLE_INFO * pTable, INT32 symbolCount);
	///	ANSI 문자열에서 바이트 단위가 아닌 문자 단위로 포인터가 이동합니다. komet 2008.8.31
	//	nCodePage는 국가 코드 페이지를 나타내며 자세한 것은 MSDN 참조
	I3_EXPORT_BASE const char *	CharNext(INT32 nCodePage, const char * pszStr);
	I3_EXPORT_BASE const char *	CharPrev(INT32 nCodePage, const char * pszStart, const char * pszCurrent);
	I3_EXPORT_BASE INT32 LengthChar(INT32 nCodePage, const char * pszStr);
	I3_EXPORT_BASE bool IsComposite(INT32 nCodePage, const char prev, const char curr, INT32 mode);

	I3_EXPORT_BASE int MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
	I3_EXPORT_BASE int WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);

	I3_EXPORT_BASE void ComposeString(wchar_t * pwStr, wchar_t * dest);
	I3_EXPORT_BASE int isTone(wchar_t input);
	I3_EXPORT_BASE wchar_t ComposeTone(wchar_t original, wchar_t tone, int toneIndex);

	I3_EXPORT_BASE void DecomposeString(const wchar_t * pwStr, wchar_t * dest);
	I3_EXPORT_BASE bool	HasTone(wchar_t input, int * original, int * tone);
	I3_EXPORT_BASE wchar_t GetLetter(int * original);
	I3_EXPORT_BASE wchar_t GetTone(int * tone);
}

#endif
