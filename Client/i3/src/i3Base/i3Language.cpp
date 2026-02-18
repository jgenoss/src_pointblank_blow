#include "i3CommonType.h"
#include "i3Debug.h"
#include "i3ErrorHandler.h"
#include "i3Language.h"

I3_EXPORT_BASE
const char * i3Language::GetLanguage( UINT32 nCodePage)
{
	char * pszLang = "UNKNOWN";

	switch( nCodePage)
	{
	case I3_LANG_CODE_ANSI:						pszLang = "Windows ANSI";			break;
	case I3_LANG_CODE_KOREAN:					pszLang = "KOREAN";					break;		//	HANGUL_CHARSET
	case I3_LANG_CODE_JAPANESE:					pszLang = "JAPANESE";				break;		//	SHIFTJIS_CHARSET
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:		pszLang = "CHINESE_SIMPLIFIED";		break;		//	GB2312_CHARSET
	case I3_LANG_CODE_CHINESE_TRADITIONAL:		pszLang = "CHINESE_TRADITIONAL";	break;		//	CHINESEBIG5_CHARSET
	case I3_LANG_CODE_THAI:						pszLang = "THAI";					break;		//	THAI_CHARSET
	case I3_LANG_CODE_RUSSIA:					pszLang = "RUSSIA";					break;		//	RUSSIA_CHARSET
	case I3_LANG_CODE_GREEK:					pszLang = "GREEK";					break;		//	GREEK_CHARSET
	case I3_LANG_CODE_TURKISH:					pszLang = "TURKISH";				break;		//	TURKISH_CHARSET
	case I3_LANG_CODE_HEBREW:					pszLang = "HEBREW";					break;		//	HEBREW_CHARSET
	case I3_LANG_CODE_ARABIC:					pszLang = "ARABIC";					break;		//	ARABIC_CHARSET
	case I3_LANG_CODE_BALTIC:					pszLang = "BALTIC";					break;		//	BALTIC_CHARSET	
	case I3_LANG_CODE_EASTEUROPE:				pszLang = "EASTEUROPE";				break;		//	EASTEUROPE_CHARSET
	case I3_LANG_CODE_VIETNAMESE:				pszLang = "VIETNAMESE";				break;		//	VIETNAMESE_CHARSET
	case I3_LANG_CODE_WESTEUROPE:				pszLang = "WEST EUROPE";			break;
	}

	return pszLang;
}

I3_EXPORT_BASE
INT32 i3Language::GetCharByte(UINT32 nCodePage)
{
#if defined (I3_WINDOWS)
	switch( nCodePage)
	{
	case I3_LANG_CODE_KOREAN:				return 2;	//	Korean (Unified Hangeul Code) 	
	case I3_LANG_CODE_JAPANESE:				return 2;	//	Japanese, Shift-JIS
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:	return 2;	//	GB2312			중국어 간체
	case I3_LANG_CODE_CHINESE_TRADITIONAL:	return 2;	//	CHINESEBIG5		중국어 번체
	case I3_LANG_CODE_VIETNAMESE:			return 2;	//	Vietnamese(Windows 1258)
	case I3_LANG_CODE_THAI:					return 4;	//	Thai (same as 28605, ISO 8859-15)
	case I3_LANG_CODE_UNKNOWN:				I3ASSERT_0;
	default:	return 1;
	}
#else
	return 1;
#endif
}

I3_EXPORT_BASE
INT32 i3Language::GetCharMaxCount(UINT32 nCodePage, INT32 nBufSize)
{
	INT32 charSize = GetCharByte( nCodePage);

	if( charSize > 0)
		return (nBufSize / charSize);
	else 
		return 0;
}

static UINT32 g_CustomCP = (UINT32) I3_LANG_CODE_UNKNOWN;

I3_EXPORT_BASE
void i3Language::SetCurrentCodePage(UINT32 CodePage)
{
	g_CustomCP = CodePage;
}

I3_EXPORT_BASE
UINT32 i3Language::GetCurrentCodePage(void)
{
#if defined (I3_WINDOWS)
	if ( g_CustomCP != I3_LANG_CODE_UNKNOWN )
		return g_CustomCP;
	return GetACP();
#else
	return 0;
#endif
}

I3_EXPORT_BASE
const char * i3Language::GetDefaultFontName(UINT32 nCodePage)
{
	switch( nCodePage)
	{
	case I3_LANG_CODE_KOREAN:		return "돋음";				//	Korean (Unified Hangeul Code) 
	case I3_LANG_CODE_THAI:			return "Tahoma";			//	Thai (same as 28605, ISO 8859-15)
	case I3_LANG_CODE_JAPANESE:		return "MS Gothic";
	case I3_LANG_CODE_VIETNAMESE:	return "Courier New";
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:	return "SimSun";
	case I3_LANG_CODE_CHINESE_TRADITIONAL:	return "MingLiU";
	case I3_LANG_CODE_WESTEUROPE:	return "Arial";				//	"Arial", "Times New Roman", "Courier New"
	case I3_LANG_CODE_RUSSIA:		return "Arial";				//	Cyrillic
	case I3_LANG_CODE_TURKISH:		return "Calibri";
	default:	return "Arial";
	}
}

#if defined (I3_WINDOWS)

INT32 i3Language::GetCharsetFromLang( LANGID langid )
{
	switch( PRIMARYLANGID(langid) )
	{
	case LANG_JAPANESE:
		return SHIFTJIS_CHARSET;
	case LANG_KOREAN:
		return HANGEUL_CHARSET;
	case LANG_CHINESE:
		switch( SUBLANGID(langid) )
		{
		case SUBLANG_CHINESE_SIMPLIFIED:
			return GB2312_CHARSET;
		case SUBLANG_CHINESE_TRADITIONAL:
			return CHINESEBIG5_CHARSET;
		default:
			return ANSI_CHARSET;
		}
	case LANG_GREEK:
		return GREEK_CHARSET;
	case LANG_TURKISH:
		return TURKISH_CHARSET;
	case LANG_HEBREW:
		return HEBREW_CHARSET;
	case LANG_ARABIC:
		return ARABIC_CHARSET;
	case LANG_ESTONIAN:
	case LANG_LATVIAN:
	case LANG_LITHUANIAN:
		return BALTIC_CHARSET;
	case LANG_THAI:
		return THAI_CHARSET;
	case LANG_CZECH:
	case LANG_HUNGARIAN:
	case LANG_POLISH:
	case LANG_CROATIAN:
	case LANG_MACEDONIAN:
	case LANG_ROMANIAN:
	case LANG_SLOVAK:
	case LANG_SLOVENIAN:
		return EASTEUROPE_CHARSET;
	case LANG_RUSSIAN:
	case LANG_BELARUSIAN:
	case LANG_BULGARIAN:
	case LANG_UKRAINIAN:
		return RUSSIAN_CHARSET;
	case LANG_VIETNAMESE:
		return VIETNAMESE_CHARSET;
	case LANG_PORTUGUESE:
	case LANG_SPANISH:
	case LANG_ITALIAN:
		return ANSI_CHARSET;
	default:
		return ANSI_CHARSET;
	}
}

INT32 i3Language::GetCodePageFromCharset( INT32 charset )
{
	switch( charset )
	{
	case SHIFTJIS_CHARSET:
		return I3_LANG_CODE_JAPANESE;
	case HANGUL_CHARSET:
		return I3_LANG_CODE_KOREAN;
	case GB2312_CHARSET:
		return I3_LANG_CODE_CHINESE_SIMPLIFIED;
	case CHINESEBIG5_CHARSET:
		return I3_LANG_CODE_CHINESE_TRADITIONAL;
	case GREEK_CHARSET:
		return I3_LANG_CODE_GREEK;
	case TURKISH_CHARSET:
		return I3_LANG_CODE_TURKISH;
	case HEBREW_CHARSET:
		return I3_LANG_CODE_HEBREW;
	case ARABIC_CHARSET:
		return I3_LANG_CODE_ARABIC;
	case BALTIC_CHARSET:
		return I3_LANG_CODE_BALTIC;
	case THAI_CHARSET:
		return I3_LANG_CODE_THAI;
	case RUSSIAN_CHARSET:
		return I3_LANG_CODE_RUSSIA;
	case EASTEUROPE_CHARSET:
		return I3_LANG_CODE_EASTEUROPE;
	case VIETNAMESE_CHARSET:
		return I3_LANG_CODE_VIETNAMESE;
	case ANSI_CHARSET:
		return 1252;
	default:
		{
			I3PRINTLOG(I3LOG_WARN,  "Unknown charset. Please check!");

			return I3_LANG_CODE_UNKNOWN;
		}
		break;
	}
}

INT32 i3Language::GetCodePageFromLang( LANGID langid )
{
	//	서유럽어 또는 미국 국가코드가 1252로 동일
	if( PRIMARYLANGID( langid) == LANG_PORTUGUESE ||
		PRIMARYLANGID( langid) == LANG_SPANISH ||
		PRIMARYLANGID( langid) == LANG_ITALIAN ||
		PRIMARYLANGID( langid) == LANG_ENGLISH)
	{
		return 1252;
	}
	
	return GetCodePageFromCharset(GetCharsetFromLang(langid));
}

INT32 i3Language::GetCharasetFromCodePage( UINT32 nCodePage)
{
	switch( nCodePage )
	{
	case I3_LANG_CODE_JAPANESE:
		return SHIFTJIS_CHARSET;
	case I3_LANG_CODE_KOREAN:
		return HANGUL_CHARSET;
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:
		return GB2312_CHARSET;
	case I3_LANG_CODE_CHINESE_TRADITIONAL:
		return CHINESEBIG5_CHARSET;
	case I3_LANG_CODE_GREEK:
		return GREEK_CHARSET;
	case I3_LANG_CODE_TURKISH:
		return TURKISH_CHARSET;
	case I3_LANG_CODE_HEBREW:
		return HEBREW_CHARSET;
	case I3_LANG_CODE_ARABIC:
		return ARABIC_CHARSET;
	case I3_LANG_CODE_BALTIC:
		return BALTIC_CHARSET;
	case I3_LANG_CODE_THAI:
		return THAI_CHARSET;
	case I3_LANG_CODE_RUSSIA:
		return RUSSIAN_CHARSET;
	case I3_LANG_CODE_EASTEUROPE:
		return EASTEUROPE_CHARSET;
	case I3_LANG_CODE_VIETNAMESE:
		return VIETNAMESE_CHARSET;
	case I3_LANG_CODE_WESTEUROPE:
		return ANSI_CHARSET;
	default:
		{
			I3PRINTLOG(I3LOG_WARN,  "Unknown code page. Please check!");

			return ANSI_CHARSET;
		}
	}
}

// Get the codepage from the locale language id 
// Based on the table from http://www.science.co.il/Language/Locale-Codes.asp?s=decimal
INT32 i3Language::GetCodepageFromLocaleId( UINT32 lcid)
{
    switch ( lcid )
    {
        case 1098:  // Telugu
        case 1095:  // Gujarati
        case 1094:  // Punjabi
        case 1103:  // Sanskrit
        case 1111:  // Konkani
        case 1114:  // Syriac
        case 1099:  // Kannada
        case 1102:  // Marathi
        case 1125:  // Divehi
        case 1067:  // Armenian
        case 1081:  // Hindi
        case 1079:  // Georgian
        case 1097:  // Tamil
            return 0;
        case 1054:  // Thai
            return 874;
        case 1041:  // Japanese
            return 932;
        case 2052:  // Chinese (PRC)
        case 4100:  // Chinese (Singapore)
            return 936;
        case 1042:  // Korean
            return 949;
        case 5124:  // Chinese (Macau S.A.R.)
        case 3076:  // Chinese (Hong Kong S.A.R.)
        case 1028:  // Chinese (Taiwan)
            return 950;
        case 1048:  // Romanian
        case 1060:  // Slovenian
        case 1038:  // Hungarian
        case 1051:  // Slovak
        case 1045:  // Polish
        case 1052:  // Albanian
        case 2074:  // Serbian (Latin)
        case 1050:  // Croatian
        case 1029:  // Czech
            return 1250;
        case 1104:  // Mongolian (Cyrillic)
        case 1071:  // FYRO Macedonian
        case 2115:  // Uzbek (Cyrillic)
        case 1058:  // Ukrainian
        case 2092:  // Azeri (Cyrillic)
        case 1092:  // Tatar
        case 1087:  // Kazakh
        case 1059:  // Belarusian
        case 1088:  // Kyrgyz (Cyrillic)
        case 1026:  // Bulgarian
        case 3098:  // Serbian (Cyrillic)
        case 1049:  // Russian
            return 1251;
        case 8201:  // English (Jamaica)
        case 3084:  // French (Canada)
        case 1036:  // French (France)
        case 5132:  // French (Luxembourg)
        case 5129:  // English (New Zealand)
        case 6153:  // English (Ireland)
        case 1043:  // Dutch (Netherlands)
        case 9225:  // English (Caribbean)
        case 4108:  // French (Switzerland)
        case 4105:  // English (Canada)
        case 1110:  // Galician
        case 10249:  // English (Belize)
        case 3079:  // German (Austria)
        case 6156:  // French (Monaco)
        case 12297:  // English (Zimbabwe)
        case 1069:  // Basque
        case 2067:  // Dutch (Belgium)
        case 2060:  // French (Belgium)
        case 1035:  // Finnish
        case 1080:  // Faroese
        case 1031:  // German (Germany)
        case 3081:  // English (Australia)
        case 1033:  // English (United States)
        case 2057:  // English (United Kingdom)
        case 1027:  // Catalan
        case 11273:  // English (Trinidad)
        case 7177:  // English (South Africa)
        case 1030:  // Danish
        case 13321:  // English (Philippines)
        case 15370:  // Spanish (Paraguay)
        case 9226:  // Spanish (Colombia)
        case 5130:  // Spanish (Costa Rica)
        case 7178:  // Spanish (Dominican Republic)
        case 12298:  // Spanish (Ecuador)
        case 17418:  // Spanish (El Salvador)
        case 4106:  // Spanish (Guatemala)
        case 18442:  // Spanish (Honduras)
        case 3082:  // Spanish (International Sort)
        case 13322:  // Spanish (Chile)
        case 19466:  // Spanish (Nicaragua)
        case 2058:  // Spanish (Mexico)
        case 10250:  // Spanish (Peru)
        case 20490:  // Spanish (Puerto Rico)
        case 1034:  // Spanish (Traditional Sort)
        case 14346:  // Spanish (Uruguay)
        case 8202:  // Spanish (Venezuela)
        case 1089:  // Swahili
        case 1053:  // Swedish
        case 2077:  // Swedish (Finland)
        case 5127:  // German (Liechtenstein)
        case 1078:  // Afrikaans
        case 6154:  // Spanish (Panama)
        case 4103:  // German (Luxembourg)
        case 16394:  // Spanish (Bolivia)
        case 2055:  // German (Switzerland)
        case 1039:  // Icelandic
        case 1057:  // Indonesian
        case 1040:  // Italian (Italy)
        case 2064:  // Italian (Switzerland)
        case 2068:  // Norwegian (Nynorsk)
        case 11274:  // Spanish (Argentina)
        case 1046:  // Portuguese (Brazil)
        case 1044:  // Norwegian (Bokmal)
        case 1086:  // Malay (Malaysia)
        case 2110:  // Malay (Brunei Darussalam)
        case 2070:  // Portuguese (Portugal)
            return 1252;
        case 1032:  // Greek
            return 1253;
        case 1091:  // Uzbek (Latin)
        case 1068:  // Azeri (Latin)
        case 1055:  // Turkish
            return 1254;
        case 1037:  // Hebrew
            return 1255;
        case 5121:  // Arabic (Algeria)
        case 15361:  // Arabic (Bahrain)
        case 9217:  // Arabic (Yemen)
        case 3073:  // Arabic (Egypt)
        case 2049:  // Arabic (Iraq)
        case 11265:  // Arabic (Jordan)
        case 13313:  // Arabic (Kuwait)
        case 12289:  // Arabic (Lebanon)
        case 4097:  // Arabic (Libya)
        case 6145:  // Arabic (Morocco)
        case 8193:  // Arabic (Oman)
        case 16385:  // Arabic (Qatar)
        case 1025:  // Arabic (Saudi Arabia)
        case 10241:  // Arabic (Syria)
        case 14337:  // Arabic (U.A.E.)
        case 1065:  // Farsi
        case 1056:  // Urdu
        case 7169:  // Arabic (Tunisia)
            return 1256;
        case 1061:  // Estonian
        case 1062:  // Latvian
        case 1063:  // Lithuanian
            return 1257;
        case 1066:  // Vietnamese
            return 1258;
    }
    return 65001;   // utf-8
} 

#endif
