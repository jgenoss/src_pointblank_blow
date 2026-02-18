//	i3Language.h

#if !defined ( __I3_LANGUAGE_H)
#define __I3_LANGUAGE_H

#include "i3CommonType.h"

//////////////////////////////////////////////////////////////////////////////////////
// 작성자			: 백성민
// 작성일자			: 2008.10
//////////////////////////////////////////////////////////////////////////////////////

//	각국 언어별 코드 페이지 (MSDN의 코드 페이지와 일치합니다.)
typedef INT32 I3_LANG_CODE;
#define I3_LANG_CODE_UNKNOWN				-1
#define I3_LANG_CODE_ANSI					0		//	윈도우 기본 설정 언어 (한글 OS = 한글, 영문 OS = 영어)
#define I3_LANG_CODE_KOREAN					949		//	한국어		Korean (Unified Hangeul Code) 
#define I3_LANG_CODE_JAPANESE				932		//	일본어		Japanese, Shift-JIS 
#define I3_LANG_CODE_CHINESE_SIMPLIFIED		936		//	중국어 간체	Simplified Chinese (PRC, Singapore)
#define I3_LANG_CODE_CHINESE_TRADITIONAL	950		//	중국어 번체	Traditional Chinese (Taiwan; Hong Kong SAR, PRC)
#define I3_LANG_CODE_THAI					874		//	태국어		Thai (same as 28605, ISO 8859-15)
#define I3_LANG_CODE_RUSSIA					1251	//	러시아어 (키릴 자모)
#define I3_LANG_CODE_ENGLISH_US				1252	//	영어 (영문 윈도우)
#define I3_LANG_CODE_GREEK					1253	//	그리스어	Greek 
#define I3_LANG_CODE_TURKISH				1254	//	터키어		Turkish
#define I3_LANG_CODE_HEBREW					1255	//	히브리어	Hebrew	
#define I3_LANG_CODE_ARABIC					1256	//	아랍어		Arabic 
#define I3_LANG_CODE_BALTIC					1257	//	발트어?		Baltic
#define I3_LANG_CODE_EASTEUROPE				1250	//	동유럽어?	(Czech, Hungarian, Polish)
#define I3_LANG_CODE_VIETNAMESE				1258	//	베트남어	Vietnamese 	
#define I3_LANG_CODE_WESTEUROPE				1252	//	서유럽어	(English(?), Portuguese, Spanish, Italian, French, German, Norwegian, Swedish, Danish, Dutch, Finnish)


namespace i3Language
{
	//	코드페이지에 해당하는 국가 이름을 반환
	I3_EXPORT_BASE const char * GetLanguage(UINT32 nCodePage);		

	//	각 국가별 한 문자의 BYTE 크기를 반환	
	I3_EXPORT_BASE INT32 GetCharByte(UINT32 nCodePage);

	//	버퍼에 들어갈수 있는 각 국가별 최대 문자 갯수 반환
	I3_EXPORT_BASE INT32 GetCharMaxCount(UINT32 nCodePage, INT32 nBufSize);

	I3_EXPORT_BASE void SetCurrentCodePage(UINT32 CodePage);

	//	현재 설정된 OS의 언어 코드 페이지를 반환
	I3_EXPORT_BASE UINT32 GetCurrentCodePage(void);

	//	각국가별 기본 폰트 이름
	I3_EXPORT_BASE const char * GetDefaultFontName(UINT32 nCodePage);

#if defined (I3_WINDOWS)
	I3_EXPORT_BASE INT32 GetCharsetFromLang( LANGID langid );

	I3_EXPORT_BASE INT32 GetCharasetFromCodePage( UINT32 nCodePage);

	I3_EXPORT_BASE INT32 GetCodePageFromCharset( INT32 charset );

	I3_EXPORT_BASE INT32 GetCodePageFromLang( LANGID langid );

	I3_EXPORT_BASE INT32 GetCodepageFromLocaleId( UINT32 lcid);
#endif
};

#endif	//	__I3_LANGUAGE_H