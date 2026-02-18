#include "i3CommonType.h"
#include "i3String.h"
#include "i3Language.h"

#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#if !defined( I3_WIPI_SKT_SV360)
#define USE_C_LIBRARY_FUNCS
#endif

#if defined(I3_LANG_CODE_VIETNAMESE)

#pragma warning(push)
#pragma warning(disable : 4428)				// 소스에 유니버설 문자 이름('\uXXX')이 있으면 나오는 4레벨 경고로 쓸모가 없어서 경고 제거블록 설정..(14.07.03.수빈)

const wchar_t Vietnamese_Charset_Table[24][5] =
{
	{ L'\u00C1', L'\u00C0', L'\u1EA2', L'\u00C3', L'\u1EA0' },
	{ L'\u00E1', L'\u00E0', L'\u1EA3', L'\u00E3', L'\u1EA1' },
	{ L'\u1EAE', L'\u1EB0', L'\u1EB2', L'\u1EB4', L'\u1EB6' },
	{ L'\u1EAF', L'\u1EB1', L'\u1EB3', L'\u1EB5', L'\u1EB7' },
	{ L'\u1EA4', L'\u1EA6', L'\u1EA8', L'\u1EAA', L'\u1EAC' },
	{ L'\u1EA5', L'\u1EA7', L'\u1EA9', L'\u1EAB', L'\u1EAD' },
	{ L'\u00C9', L'\u00C8', L'\u1EBA', L'\u1EBC', L'\u1EB8' },
	{ L'\u00E9', L'\u00E8', L'\u1EBB', L'\u1EBD', L'\u1EB9' },
	{ L'\u1EBE', L'\u1EC0', L'\u1EC2', L'\u1EC4', L'\u1EC6' },
	{ L'\u1EBF', L'\u1EC1', L'\u1EC3', L'\u1EC5', L'\u1EC7' },
	{ L'\u00CD', L'\u00CC', L'\u1EC8', L'\u0128', L'\u1ECA' },
	{ L'\u00ED', L'\u00EC', L'\u1EC9', L'\u0129', L'\u1ECB' },
	{ L'\u00D3', L'\u00D2', L'\u1ECE', L'\u00D5', L'\u1ECC' },
	{ L'\u00F3', L'\u00F2', L'\u1ECF', L'\u00F5', L'\u1ECD' },
	{ L'\u1ED0', L'\u1ED2', L'\u1ED4', L'\u1ED6', L'\u1ED8' },
	{ L'\u1ED1', L'\u1ED3', L'\u1ED5', L'\u1ED7', L'\u1ED9' },
	{ L'\u1EDA', L'\u1EDC', L'\u1EDE', L'\u1EE0', L'\u1EE2' },
	{ L'\u1EDB', L'\u1EDD', L'\u1EDF', L'\u1EE1', L'\u1EE3' },
	{ L'\u00DA', L'\u00D9', L'\u1EE6', L'\u0168', L'\u1EE4' },
	{ L'\u00FA', L'\u00F9', L'\u1EE7', L'\u0169', L'\u1EE5' },
	{ L'\u1EE8', L'\u1EEA', L'\u1EEC', L'\u1EEE', L'\u1EF0' },
	{ L'\u1EE9', L'\u1EEB', L'\u1EED', L'\u1EEF', L'\u1EF1' },
	{ L'\u1EF2', L'\u00DD', L'\u1EF6', L'\u1EF8', L'\u1EF4' },
	{ L'\u1EF3', L'\u00FD', L'\u1EF7', L'\u1EF9', L'\u1EF5' },
};

const wchar_t Vietnamese_Charset_Original[24] =
{
	L'A', L'a', L'\u0102', L'\u0103',
	L'\u00C2', L'\u00E2', L'E', L'e',
	L'\u00CA', L'\u00EA', L'I', L'i',
	L'O', L'o', L'\u00D4', L'\u00F4',
	L'\u01A0', L'\u01A1', L'U', L'u',
	L'\u01AF', L'\u01B0', L'Y', L'y',
};

const wchar_t Vietnamese_Charset_Tone_Table[5] =
{
	L'\u0300',
	L'\u0301',
	L'\u0309',
	L'\u0303',
	L'\u0323',
};

#pragma warning(pop)

#endif

static I3_SYMBOLTABLE_INFO *	_FindSymbol( I3_SYMBOLTABLE_INFO * pTable, INT32 symbolCount, char * pszStr)
{
	INT32 i;

	for( i = 0; i < symbolCount; i++)
	{
		if( i3::generic_is_iequal( pTable[i].m_szSymbol, pszStr) )
		{
			return & pTable[i];
		}
	}

	return nullptr;
}

I3_EXPORT_BASE 
void	i3String::PreprocessMacro( char * pszDest, INT32 /*Length*/, const char * pszSrc, I3_SYMBOLTABLE_INFO * pTable, INT32 symbolCount)
{
	char * pszTemp = (char *) pszSrc;
	char szSym[65];
	INT32 symAccm = 0;
	bool bInSymbol = false;
	I3_SYMBOLTABLE_INFO * pSymbol;

	while( *pszTemp != 0)
	{
		switch( *pszTemp)
		{
			case '%' :
				if( bInSymbol)
				{
					if( symAccm == 0)
					{
						// %% 의 경우
						*pszDest = '%';
						pszDest++;
					}
					else
					{
						szSym[ symAccm] = 0;
						symAccm = 0;

						pSymbol = _FindSymbol( pTable, symbolCount, szSym);

						if( pSymbol != nullptr)
						{
							INT32 len = strlen( pSymbol->m_szValue);

							memcpy( pszDest, pSymbol->m_szValue, len);
							pszDest += len;
						}
						else
						{
							I3TRACE( "올바르지 않은 Symbol : %s\n", szSym);
						}
					}

					bInSymbol = false;
				}
				else
				{
					bInSymbol = true;
					symAccm = 0;
				}
				break;

			default :
				if( bInSymbol)
				{
					szSym[ symAccm ] = *pszTemp;
					symAccm ++;
				}
				else
				{
					*pszDest = *pszTemp;
					pszDest++;
				}
				break;
		}

		pszTemp++;
	}

	*pszDest = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	THAI language -- 소스는 gpgstudy에서 가져옴 (2008.7.31 komet)
/////////////////////////////////////////////////////////////////////////////////////////////

// Unicode of thai : 0x0E00~0x0E7F (Direct Mapping)
// Thai Character -> Unicode    (char+0x0E00)-0xA0 =    char+0x0D60
// Unicode -> Thai Character (wchar_t-0x0E00)+0xA0 = wchar_t-0x0D60

enum THAI_CLASS		//	Thai character classification
{	
	CTRL= 0,		//	Control characters
	NON= 1,			//	Non-composibles
	CONS= 2,		//	Consonants
	LV= 3,			//	Leading Vowels
	FV1= 4,			//	Following Vowels
	FV2= 5,
	FV3= 6,
	BV1= 7,			//	Below Vowels
	BV2= 8,
	BD= 9,			//	Below Diacritic
	TONE=10,		//	Tonemarks
	AD1=11,			//	Above Diacritics
	AD2=12,
	AD3=13,
	AV1=14,			//	Above Vowels
	AV2=15,
	AV3=16,
};

THAI_CLASS 
THAI_CLASS_TABLE[256] = 
{
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, CTRL, 
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
	NON, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, 
	CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, 
	CONS, CONS, CONS, CONS,  FV3, CONS,  FV3, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS,  NON, 
	FV1,  AV2,  FV1,  FV1,  AV1,  AV3,  AV2,  AV3,  BV1,  BV2,   BD,  NON,  NON,  NON,  NON,  NON, 
	LV,   LV,   LV,   LV,   LV,  FV2,  NON,  AD2, TONE, TONE, TONE, TONE,  AD1,  AD1,  AD3,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, CTRL, 
};

enum THAI_INPUT_TYPE { IA, IC, IS, IR, IX };

bool THAI_INPUT_COMPOSIBLE[3][5] = {
	{ true, true, true, true, true },
	{ true, true, true,false, true },
	{ true, true,false,false, true },
};

THAI_INPUT_TYPE 
THAI_INPUT_STATE_CHECK[17][17] = {
	{ IX,IA,IA,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IA,IS,IA,IC,IC,IC,IC,IC,IC,IC,IC,IC,IC },
	{ IX,IS,IA,IS,IS,IS,IS,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IS,IA,IS,IA,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IA,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IA,IS,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IA,IS,IA,IR,IR,IR,IC,IC,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IC,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IA,IA,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IC,IC,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IC,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IC,IR,IC,IR,IR,IR,IR },
};

enum THAI_OUTPUT_TYPE { ON, OC, OX };

THAI_OUTPUT_TYPE 
THAI_OUTPUT_STATE_CHECK[17][17] = {
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,OC,OC,OC,OC,OC,OC,OC,OC,OC,OC,OC,OC,OC },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,OC,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,OC,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,ON,OC,ON,ON,ON,ON },
};

I3_EXPORT_BASE
bool i3String::IsComposite(INT32 nCodePage, const char prev, const char curr, INT32 mode)
{
	switch( nCodePage)
	{
	case I3_LANG_CODE_THAI:	//	THAI
		{
			if( mode > 2)	
				return false;
			else 
			{
				BYTE prev_stream = (BYTE)prev;

				BYTE curr_stream = (BYTE)curr;

				THAI_CLASS prev_class = THAI_CLASS_TABLE[prev_stream];
				I3ASSERT( prev_class >= 0 && prev_class < 17);

				THAI_CLASS curr_class = THAI_CLASS_TABLE[curr_stream];
				I3ASSERT( curr_class >= 0 && curr_class < 17);

				THAI_INPUT_TYPE state_type = THAI_INPUT_STATE_CHECK[prev_class][curr_class];

				return THAI_INPUT_COMPOSIBLE[mode][state_type];
			}
		}
		break;
	default:
		I3ASSERT_0;
		break;
	}

	return false;
}

I3_EXPORT_BASE
INT32 i3String::LengthChar(INT32 nCodePage, const char * pszStr)
{
	I3ASSERT( pszStr != nullptr && nCodePage != 0 && nCodePage != 1);

	INT32 nCount = 0;
	const char * pszCurr = pszStr;
	while( *pszCurr)
	{
		pszCurr = CharNext(nCodePage, pszCurr);
		nCount++;
	}	

	return nCount;
}

I3_EXPORT_BASE
const char * i3String::CharNext(INT32 nCodePage, const char * pszStr)
{
	I3ASSERT( pszStr != nullptr && nCodePage != 0 && nCodePage != 1);

	switch( nCodePage)
	{
	case I3_LANG_CODE_THAI:		//	THAI			태국은 조합형 MBCS 체계 (1~4바이트 가변 바이트 ) 주의! 태국어는 2바이트가 아니다!
		{
			const BYTE * stream = (const BYTE*) pszStr;
			
			for(;;)
			{
				THAI_CLASS c1 = THAI_CLASS_TABLE[ stream[0]];
				I3ASSERT( c1 >= 0 && c1 < 17);

				THAI_CLASS c2 = THAI_CLASS_TABLE[ stream[1]];			
				I3ASSERT( c2 >= 0 && c2 < 17);

				if( THAI_OUTPUT_STATE_CHECK[c1][c2] == OC)
					++stream;
				else
					break;
			}

			return (const char*)(stream+1);
		}
		break;
	case I3_LANG_CODE_KOREAN:					//	HANGUL			한글 
	case I3_LANG_CODE_JAPANESE:					//	SHIFTJIS		일본어
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:		//	GB2312			중국어 간체
	case I3_LANG_CODE_CHINESE_TRADITIONAL:		//	CHINESEBIG5		중국어 번체
	case I3_LANG_CODE_VIETNAMESE:				//	VIETNAMESE
		{
#if I3_WINDOWS
			return ::CharNextExA((WORD) nCodePage, pszStr, 0);
#else
			//	CJK(한중일) 언어는 DBCS 시스템 (2바이트) 
			if( *pszStr & 0x80)		//	lead byte		ascii 영역은 0 ~ 127(0x7E) 이며 0x80 부터 극동 언어(DBCS)로 인식된다.
			{
				pszStr++;
			}

			return (*pszStr != 0 ? pszStr++ : pszStr);
#endif
		}
		break;
	default:
		{
#if I3_WINDOWS
			return ::CharNextExA((WORD) nCodePage, pszStr, 0);
#else
			return pszStr++;		//	코드 페이지가 정의되지 않았다.
#endif
		}
		break;
	}
}

I3_EXPORT_BASE
const char * i3String::CharPrev(INT32 nCodePage, const char * pszStart, const char * pszCurrent)
{
	I3ASSERT( pszStart != nullptr && pszCurrent != nullptr && nCodePage != 0 && nCodePage != 1);

	switch( nCodePage)
	{
	case I3_LANG_CODE_THAI:		//	THAI			태국은 조합형 MBCS 체계 (1~4바이트 가변 바이트 )
		{
			while( pszStart < pszCurrent)
			{
				const char * pszNext = CharNext(nCodePage, pszStart);

				if( pszNext == pszCurrent)
					return pszStart;
				else if( pszNext < pszCurrent)
					pszStart = pszNext;
				else
					break;
			}

			return pszStart;
		}
		break;
	case I3_LANG_CODE_KOREAN:					//	HANGUL			한글 
	case I3_LANG_CODE_JAPANESE:					//	SHIFTJIS		일본어
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:		//	GB2312			중국어 간체
	case I3_LANG_CODE_CHINESE_TRADITIONAL:		//	CHINESEBIG5		중국어 번체
	case I3_LANG_CODE_VIETNAMESE:				//	VIETNAMESE
		{
#if I3_WINDOWS
			return ::CharPrevExA((WORD) nCodePage, pszStart, pszCurrent, 0);
#else
			while( pszStart < pszCurrent)
			{
				const char * pszNext = CharNext(nCodePage, pszStart);

				if( pszNext < pszCurrent) 
					pszStart = pszNext;
				else
					return pszNext;
			}

			return pszStart;
#endif
		}
		break;
	default:
		{
#if I3_WINDOWS
			return ::CharPrevExA((WORD) nCodePage, pszStart, pszCurrent, 0);
#else
			return pszStr;		//	코드 페이지가 정의되지 않았다.
#endif
		}
		break;
	}
}


I3_EXPORT_BASE 
int i3String::MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)	// 멀티바이트 -> 조합형 유니코드 변환
{	
	// 조합형 유니코드 -> 완성형 유니코드 과정 필요
	wchar_t * dest = nullptr;
	int len = 0;

	switch(CodePage)
	{
	case I3_LANG_CODE_VIETNAMESE:
		{
			dest = (wchar_t *)i3MemAlloc(sizeof(wchar_t) * 2048);
			len = ::MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, dest, cchWideChar);
			if(len != 0)
			{
				ComposeString(dest, lpWideCharStr);
				if(lpWideCharStr != nullptr)
				{
					len = wcslen(lpWideCharStr);
				}
			}
			I3MEM_SAFE_FREE(dest);
		}
		break;
	default: 
		{
			len = ::MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
		}
		break;
	}
	return len;
}

I3_EXPORT_BASE
void i3String::ComposeString(wchar_t * pwStr, wchar_t * dest)
{
	while(*pwStr != 0)
	{
		*dest = *pwStr;
		++pwStr;

		int toneIndex = isTone(*pwStr);

		if(toneIndex != -1)
		{
			*dest = ComposeTone(*dest, *pwStr, toneIndex);
			++pwStr;
			++dest;
		}
		else
		{
			++dest;
		}
	}
}

I3_EXPORT_BASE 
int i3String::isTone(wchar_t input)
{
	for(int i = 0 ; i < 5; i++)
	{
		if(Vietnamese_Charset_Tone_Table[i] == input)
		{
			return i;
		}
	}
	return -1;
}

I3_EXPORT_BASE 
wchar_t i3String::ComposeTone(wchar_t original, wchar_t tone, int toneIndex)
{
	for(int i = 0; i < 24; i++)
	{
		if(Vietnamese_Charset_Original[i] == original)
		{
			return Vietnamese_Charset_Table[i][toneIndex];
		}
	}

	return 0;
}

I3_EXPORT_BASE 
int i3String::WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)	// 조합형 유니코드 -> 멀티바이트 변환
{
	// 완성형 유니코드 -> 조합형 유니코드 과정 필요
	wchar_t * dest = nullptr;
	int len = 0;

	switch(CodePage)
	{
	case I3_LANG_CODE_VIETNAMESE:
		{
			dest = (wchar_t *)i3MemAlloc(sizeof(wchar_t) * 2048);
			DecomposeString(lpWideCharStr, dest);
			len = ::WideCharToMultiByte(CodePage, dwFlags, dest, wcslen(dest), lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);

			I3MEM_SAFE_FREE(dest);
		}
		break;
	default:
		{
			len = ::WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
		}
		break;
	}

	return len;
}

I3_EXPORT_BASE 
void i3String::DecomposeString(const wchar_t * pwStr, wchar_t * dest)
{
	int toneIndex = -1;
	int originalIndex = -1;

	while(*pwStr != 0)
	{
		if(HasTone(*pwStr, &originalIndex, &toneIndex))
		{
			*dest = GetLetter(&originalIndex);
			++dest;
			*dest = GetTone(&toneIndex);
		}
		else
		{
			*dest = *pwStr;
		}

		++pwStr;
		++dest;
	}
}

I3_EXPORT_BASE 
wchar_t i3String::GetLetter(int * original)
{
	return Vietnamese_Charset_Original[*original];	
}

I3_EXPORT_BASE 
wchar_t i3String::GetTone(int * tone)
{
	return Vietnamese_Charset_Tone_Table[*tone];
}

I3_EXPORT_BASE 
bool	i3String::HasTone(wchar_t input, int * original, int * tone)
{
	for(int i = 0; i < 24; i++)
	{
		for(int j = 0; j < 5; j++)
		{
			if(Vietnamese_Charset_Table[i][j] == input)
			{
				*original = i;
				*tone = j;
				return true;
			}
		}
	}

	return false;
}