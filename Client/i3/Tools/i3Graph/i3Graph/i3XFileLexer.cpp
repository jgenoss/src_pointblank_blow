#include "stdafx.h"
#include "i3XFileLexer.h"



// template 예약어 모두 추가되지 않았음.
const char*	i3XFileLexer::s_pTemplateIdenStrings[] =
{
	// template list
	"Animation",
	"AnimationKey",
	"AnimationOptions",
	"AnimationSet",
	"Boolean",
	"Boolean2d",
	"ColorRGB",
	"ColorRGBA",
	"CompressedAnimationSet",
	"Coords2d",
	"DeclData",
	"EffectDWord",
	"EffectFloats",
	"EffectInstance",
	"EffectParamDWord",
	"EffectParamFloats",
	"EffectParamString",
	"EffectString",
	"FaceAdjacency",
	"FloatKeys",
	"Frame",
	"FrameTransformMatrix",
	"FVFData",
	"Guid",
	"IndexedColor",
	"Material",
	"MaterialWrap",
	"Matrix4x4",
	"Mesh",
	"MeshFace",
	"MeshFaceWraps",
	"MeshMaterialList",
	"MeshNormals",
	"MeshTextureCoords",
	"MeshVertexColors",
	"Patch",
	"PatchMesh",
	"PatchMesh9",
	"PMAttributeRange",
	"PMInfo",
	"PMVSplitRecord",
	"SkinWeights",
	"TextureFileName",
	"TimedFloatKeys",
	"Vector",
	"VertexDuplicationIndices",
	"VertexElement",
	"XSkinMeshHeader",

	// etc tamplate
	"Header",	// 구버전 템플릿

	// 
	"template",		// template form 예약어
	"//",			// 주석
};

INT32		i3XFileLexer::s_NumTemplateIden	= sizeof(s_pTemplateIdenStrings) / sizeof(char*);
char		i3XFileLexer::s_Separator[]	= ";, \t\r\n\b\0";




i3XFileLexer::i3XFileLexer()
: m_pStream(NULL)
{
}


i3XFileLexer::~i3XFileLexer()
{
	I3_SAFE_RELEASE(m_pStream)
}


UINT32	i3XFileLexer::GetToken()
{
	char	TokenBuffer[TOKEN_BUF_SIZE];
	return GetToken(TokenBuffer);
}


INT32	i3XFileLexer::GetTokenInt(char* pTokenBuffer)
{
	UINT32 TokenNumber = GetToken(pTokenBuffer);
	I3ASSERT(TokenNumber == XTOKEN_NUMBER);
	return atoi(pTokenBuffer);
}


REAL32	i3XFileLexer::GetTokenReal(char* pTokenBuffer)
{
	UINT32 TokenNumber = GetToken(pTokenBuffer);
	I3ASSERT(TokenNumber == XTOKEN_NUMBER);
	return (REAL32)atof(pTokenBuffer);
}


BOOL	i3XFileLexer::FindToken(UINT32 DestTokenNumber)
{
	char	TokenBuffer[TOKEN_BUF_SIZE];
	UINT32	TokenNumber = GetToken(TokenBuffer);
	while(DestTokenNumber != TokenNumber)
	{
		if ( TokenNumber == XTOKEN_FILE_END )
			return FALSE;
		TokenNumber = GetToken(TokenBuffer);
	}

	return TRUE;
}


void	i3XFileLexer::SkipCurrentBlock()
{
	char	TokenBuffer[TOKEN_BUF_SIZE];
	UINT32	TokenNumber;
	INT32	BlockEndcount = 1;

	while (BlockEndcount > 0)
	{
		TokenNumber = GetToken(TokenBuffer);
		if (TokenNumber == XTOKEN_BLOCK_BEGIN)
			++BlockEndcount;
		else if (TokenNumber == XTOKEN_BLOCK_END)
			--BlockEndcount;
	}
}


void	i3XFileLexer::SetStream(i3FileStream * pStream)
{
	I3ASSERT(pStream != NULL);
	I3_REF_CHANGE(m_pStream, pStream);
}


//static
const char *	i3XFileLexer::GetIdenString(UINT32 IdenNumber)
{
	if (IdenNumber >= (UINT32)GetNumTokenIdentifier())
		return NULL;
	return s_pTemplateIdenStrings[IdenNumber];
}


//static
UINT32	i3XFileLexer::GetIdenNumber(char* pToken)
{
	
	char c = pToken[0];
	if (c == '"')
	{
		// "를 제거해서 내보낸다.
		INT32	Length = i3String::Length(pToken);
		for (INT32 i = 0 ; i < Length-1 ; ++i)
			pToken[i] = pToken[i+1];
		pToken[Length-2] = '\0';

		return XTOKEN_STRING;
	}
	else if (c >= '0' && c <= '9')
		return XTOKEN_NUMBER;
	else if (c == '-' && (pToken[1] >= '0' && pToken[1] <= '9'))
		return XTOKEN_NUMBER;
	else if (c == '{')
		return XTOKEN_BLOCK_BEGIN;
	else if (c == '}')
		return XTOKEN_BLOCK_END;

	UINT32	TokenNumber = XTOKEN_INVALID;
	INT32	NumTemplateRW = GetNumTokenIdentifier();


	for ( INT32 i = 0 ; i < NumTemplateRW ; ++i )
	{
		if ( 0 == i3String::Compare(pToken, s_pTemplateIdenStrings[i]) )
		{
			TokenNumber = (UINT32)i;
			return TokenNumber;
		}
	}

	if ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') )
		return XTOKEN_STRING;


	return TokenNumber;
}


//static
INT32			i3XFileLexer::GetNumTokenIdentifier()
{
	return s_NumTemplateIden;
}


//static
BOOL			i3XFileLexer::IsTemplateIden(UINT32 IdenNumber)
{
	if (IdenNumber >= XTOKEN_ANIMATION && IdenNumber <= XTOKEN_XSKINMESHHEADER)
	{
		return TRUE;
	}
	return FALSE;
}





//---------------------------------------------------------------------------------------------------
//		i3XFileTextLexer
//---------------------------------------------------------------------------------------------------
BOOL	IsWhiteChar(char c, char* pWhites)
{
	if ( c == NULL )
		return TRUE;
	while ( *pWhites != NULL )
	{
		if ( c == *pWhites )
			return TRUE;
		pWhites++;
	}
	return FALSE;
}


i3XFileTextLexer::i3XFileTextLexer()
: i3XFileLexer()
, m_StringLength(0)
, m_CurCharIndex(0)
, m_LineCount(-1)
{
	m_StringBuffer[0] = NULL;
}


i3XFileTextLexer::~i3XFileTextLexer()
{
}


UINT32	i3XFileTextLexer::GetToken(char* pTokenBuffer)
{
	while (true)
	{
		//
		// 앞에 공백 문자 제거
		//
		char c;
		BOOL	IsFileEnd = FALSE;
		do
		{
			c = GetNextChar(&IsFileEnd);
			if (IsFileEnd)
				return XTOKEN_FILE_END;
		} while(IsWhiteChar(c, s_Separator));

		//
		// 토큰화 시키기
		//
		XFILE_TOKENS TokenType = XTOKEN_INVALID;
		INT32 TokenLength = 0;

		while (true)
		{
			pTokenBuffer[TokenLength++] = c;
			c = GetNextChar(&IsFileEnd);
			if ((TRUE == IsWhiteChar(c, s_Separator))
				|| (c == ',') || (c == ';'))
			{
				break;
			}

			// 문자열과 중괄호 사이에 공백이 없는 경우가 있다.
			// 그래서 두 토큰이 하나의 토큰으로 처리가 되어서
			// 문제가 발생할 수 있기 때문에 문자가 중괄호면
			// 빠져나간다.
			if ((c == '{') || (c == '}'))
			{
				--m_CurCharIndex;
				break;
			}
		}

		pTokenBuffer[TokenLength] = NULL;

		// 주석일 경우 다시 토큰을 읽는다.
		if (0 == i3String::NCompare(pTokenBuffer, "//", 2))
		{
			m_StringLength = ReadLine();
			if (m_StringLength == 0)
				return XTOKEN_FILE_END;
			continue;
		}

		break;
	}

	return GetIdenNumber(pTokenBuffer);
}


void	i3XFileTextLexer::SetStream(i3FileStream* pStream)
{
	i3XFileLexer::SetStream(pStream);

	if (m_pStream)
	{
		m_StringLength = ReadLine();
	}
	m_Lexer.SetMasterString(m_StringBuffer);
}


char	i3XFileTextLexer::GetNextChar(BOOL * pIsFileEnd)
{
	if (m_CurCharIndex >= m_StringLength)
	{
		m_StringLength = ReadLine();
		if (m_StringLength == 0)
			*pIsFileEnd = TRUE;
	}

	return m_StringBuffer[m_CurCharIndex++];
}


INT32	i3XFileTextLexer::ReadLine()
{
	INT32 Size = m_pStream->ReadLine(m_StringBuffer, STRING_BUF_SIZE);
	m_LineCount++;
	m_CurCharIndex = 0;
	return Size;
}


//---------------------------------------------------------------------------------------------------
//		i3XFileBinLexer
//---------------------------------------------------------------------------------------------------


enum{
	DATA_BUFFER_SIZE	= 1024,
};

// The record-bearing tokens are defined as follows.

#define TOKEN_NAME         1
#define TOKEN_STRING       2
#define TOKEN_INTEGER      3
#define TOKEN_GUID         5
#define TOKEN_INTEGER_LIST 6
#define TOKEN_FLOAT_LIST   7

// The stand-alone tokens are defined as follows.

#define TOKEN_OBRACE      10	//{
#define TOKEN_CBRACE      11	//}
#define TOKEN_OPAREN      12	//(
#define TOKEN_CPAREN      13	//)
#define TOKEN_OBRACKET    14	//[
#define TOKEN_CBRACKET    15	//]
#define TOKEN_OANGLE      16	//
#define TOKEN_CANGLE      17
#define TOKEN_DOT         18	//.
#define TOKEN_COMMA       19	//,
#define TOKEN_SEMICOLON   20	//;
#define TOKEN_TEMPLATE    31	//template
#define TOKEN_WORD        40
#define TOKEN_DWORD       41
#define TOKEN_FLOAT       42
#define TOKEN_DOUBLE      43
#define TOKEN_CHAR        44
#define TOKEN_UCHAR       45
#define TOKEN_SWORD       46
#define TOKEN_SDWORD      47
#define TOKEN_VOID        48
#define TOKEN_LPSTR       49
#define TOKEN_UNICODE     50
#define TOKEN_CSTRING     51
#define TOKEN_ARRAY       52



i3XFileBinLexer::i3XFileBinLexer()
: m_pDataBuffer(NULL)
, m_DataCurPointer(0)
, m_DataReadedSize(0)
{
}


i3XFileBinLexer::~i3XFileBinLexer()
{
	I3MEM_SAFE_FREE(m_pDataBuffer);
}


UINT32	i3XFileBinLexer::GetToken(char* pTokenBuffer)
{
	m_IntList.clear();
	m_RealList.clear();

	UINT16 Token = 0;
	while(true)
	{
		Token = GetNextToken();
		if (Token == 0)
			return XTOKEN_FILE_END;

		if (Token == TOKEN_COMMA || Token == TOKEN_SEMICOLON)
			continue;

		break;
	}

	if ( !GetBinTokenToTextToken(Token, pTokenBuffer ) )
	{
		return XTOKEN_INVALID;
	}

	if (pTokenBuffer[0] == NULL)
		return XTOKEN_STRING;

	return GetIdenNumber(pTokenBuffer);
}


INT32	i3XFileBinLexer::GetTokenInt(char* pTokenBuffer)
{
	if ( m_IntList.empty() )
	{
		UINT16 Token = GetNextToken();
		I3ASSERT(Token == TOKEN_INTEGER_LIST);
		GetIntegerListToken(pTokenBuffer);
	}

	I3ASSERT(m_IntList.empty() != true);

	INT32	Value = m_IntList.front();
	m_IntList.pop_front();
	return Value;
}


REAL32	i3XFileBinLexer::GetTokenReal(char* pTokenBuffer)
{
	if ( m_RealList.empty() )
	{
		UINT16 Token = GetNextToken();
		I3ASSERT(Token == TOKEN_FLOAT_LIST);
		GetFloatListToken(pTokenBuffer);
	}

	I3ASSERT(m_RealList.empty() != true);

	REAL32	Value = m_RealList.front();
	m_RealList.pop_front();
	return Value;
}


void	i3XFileBinLexer::SetStream(i3FileStream* pStream)
{
	i3XFileLexer::SetStream(pStream);
	if (m_pDataBuffer == NULL)
	{
		m_pDataBuffer = (UINT8*)i3MemAlloc(DATA_BUFFER_SIZE);
	}

	m_DataReadedSize = _ReadData(m_pDataBuffer, DATA_BUFFER_SIZE);
}


INT32	i3XFileBinLexer::ReadData(void* pBuffer, UINT32 Size)
{
	INT32	ReadedSize = 0;
	if (m_DataCurPointer+Size <= m_DataReadedSize)
	{
		UINT8 * pDestBuffer = (UINT8*)pBuffer;
		for (UINT32 i = 0 ; i < Size ; ++i)
			pDestBuffer[i] = m_pDataBuffer[m_DataCurPointer+i];

		m_DataCurPointer += Size;
		return (INT32)Size;
	}
	// 읽을 데이터가 부족하지만 버퍼에 잔여 데이터가 남아있을때
	// 잔여 데이터를 먼저 복사한다.
	else if (m_DataCurPointer < m_DataReadedSize)
	{
		UINT8 * pDestBuffer = (UINT8*)pBuffer;
		UINT32 PreWriteSize = m_DataReadedSize - m_DataCurPointer;
		for (UINT32 i = 0 ; i < PreWriteSize ; ++i)
			pDestBuffer[i] = m_pDataBuffer[m_DataCurPointer+i];
		pBuffer = pDestBuffer+PreWriteSize;

		m_DataCurPointer += PreWriteSize;
		Size		-= PreWriteSize;
		ReadedSize	+= PreWriteSize;
		I3ASSERT(m_DataCurPointer == m_DataReadedSize);
	}

	if (_IsEOS() != TRUE)
	{
		m_DataCurPointer = 0;
		UINT32	CurFilePos = m_pStream->GetPosition();
		UINT32	EndFilePos = m_pStream->SetPosition(0, STREAM_END);
		UINT32	RemainderSize = EndFilePos - CurFilePos;
		m_pStream->SetPosition(CurFilePos, STREAM_BEGIN);

		INT32 ret = _ReadData(m_pDataBuffer, DATA_BUFFER_SIZE);

		if (ret != STREAM_ERR)
		{
			m_DataReadedSize = ret;
		}
		else if (RemainderSize > 0)
		{
			m_DataReadedSize = RemainderSize;
		}

		if (m_DataReadedSize > 0)
			return ReadData(pBuffer, Size) + ReadedSize;
	}

	// 여기까지 온다면 파일을 다 읽었다는 뜻
	return -1;
}


BOOL	i3XFileBinLexer::_IsEOS()
{
	return m_pStream->GetLastError() == i3FileStream::STREAM_EOS;
}


INT32	i3XFileBinLexer::_ReadData(void* pBuffer, UINT32 Size)
{
	return m_pStream->Read(pBuffer, Size);
}


UINT16	i3XFileBinLexer::GetNextToken()
{
	UINT8 TokenLow =0, TokenHigh =0;
	UINT16 Token =0;
	ReadData(&TokenLow, sizeof(UINT8));
	ReadData(&TokenHigh, sizeof(UINT8));
	Token = TokenLow;
	Token |= TokenHigh << 8;
	return Token;
}


BOOL	i3XFileBinLexer::GetBinTokenToTextToken(UINT16 Token, char* TextBuffer)
{
	switch(Token)
	{
	case TOKEN_NAME:			GetNameToken(TextBuffer);			break;
	case TOKEN_STRING:			GetStringToken(TextBuffer);			break;
	case TOKEN_INTEGER:			GetIntegerToken(TextBuffer);		break;
	case TOKEN_GUID:			GetGUIDToken(TextBuffer);			break;
	case TOKEN_INTEGER_LIST:	GetIntegerListToken(TextBuffer);	break;
	case TOKEN_FLOAT_LIST:		GetFloatListToken(TextBuffer);		break;

	case TOKEN_OBRACE:		i3String::Copy(TextBuffer, "{", TOKEN_BUF_SIZE );			break;
	case TOKEN_CBRACE:		i3String::Copy(TextBuffer, "}", TOKEN_BUF_SIZE );			break;
	case TOKEN_OPAREN:		i3String::Copy(TextBuffer, "(", TOKEN_BUF_SIZE );			break;
	case TOKEN_CPAREN:		i3String::Copy(TextBuffer, ")", TOKEN_BUF_SIZE );			break;
	case TOKEN_OBRACKET:	i3String::Copy(TextBuffer, "[", TOKEN_BUF_SIZE );			break;
	case TOKEN_CBRACKET:	i3String::Copy(TextBuffer, "]", TOKEN_BUF_SIZE );			break;
	case TOKEN_OANGLE:
	case TOKEN_CANGLE:
		I3ASSERT_0;
		break;
	case TOKEN_DOT:			i3String::Copy(TextBuffer, ".", TOKEN_BUF_SIZE );			break;
	case TOKEN_COMMA:		i3String::Copy(TextBuffer, ",", TOKEN_BUF_SIZE );			break;
	case TOKEN_SEMICOLON:	i3String::Copy(TextBuffer, ";", TOKEN_BUF_SIZE );			break;
	case TOKEN_TEMPLATE:	i3String::Copy(TextBuffer, "template", TOKEN_BUF_SIZE );	break;

	case TOKEN_WORD:		i3String::Copy(TextBuffer, "WORD", TOKEN_BUF_SIZE );		break;
	case TOKEN_DWORD:		i3String::Copy(TextBuffer, "DWORD", TOKEN_BUF_SIZE );		break;
	case TOKEN_FLOAT:		i3String::Copy(TextBuffer, "FLOAT", TOKEN_BUF_SIZE );		break;
	case TOKEN_DOUBLE:		i3String::Copy(TextBuffer, "DOUBLE", TOKEN_BUF_SIZE );		break;
	case TOKEN_CHAR:		i3String::Copy(TextBuffer, "CHAR", TOKEN_BUF_SIZE );		break;
	case TOKEN_UCHAR:		i3String::Copy(TextBuffer, "UCHAR", TOKEN_BUF_SIZE );		break;
	case TOKEN_SWORD:		i3String::Copy(TextBuffer, "SWORD", TOKEN_BUF_SIZE );		break;
	case TOKEN_SDWORD:		i3String::Copy(TextBuffer, "SDWORD", TOKEN_BUF_SIZE );		break;
	case TOKEN_VOID:		i3String::Copy(TextBuffer, "VOID", TOKEN_BUF_SIZE );		break;
	case TOKEN_LPSTR:		i3String::Copy(TextBuffer, "LPSTR", TOKEN_BUF_SIZE );		break;
	case TOKEN_UNICODE:		i3String::Copy(TextBuffer, "UNICODE", TOKEN_BUF_SIZE );		break;
	case TOKEN_CSTRING:		i3String::Copy(TextBuffer, "CSTRING", TOKEN_BUF_SIZE );		break;
	case TOKEN_ARRAY:		i3String::Copy(TextBuffer, "ARRAY", TOKEN_BUF_SIZE );		break;
	default:		return FALSE;
	}

	return TRUE;
}


BOOL	i3XFileBinLexer::GetNameToken(char * pBuffer)
{
	UINT32 NameLength = 0;
	if ( -1 == ReadData(&NameLength, sizeof(UINT32)) )
		return FALSE;

	if (NameLength > 0)
	{
		if (-1 == ReadData(pBuffer, NameLength))
			return FALSE;
	}
	pBuffer[NameLength] = NULL;

	return TRUE;
}


BOOL	i3XFileBinLexer::GetStringToken(char * pBuffer)
{
	UINT32 Length = 0;
	if ( -1 == ReadData(&Length, sizeof(UINT32)) )
		return FALSE;

	if (-1 == ReadData(pBuffer, Length))
		return FALSE;
	pBuffer[Length] = NULL;
	return TRUE;
}


BOOL	i3XFileBinLexer::GetIntegerToken(char * pBuffer)
{
	UINT32	Value = 0;
	if (-1 == ReadData(&Value, sizeof(UINT32)))
		return FALSE;

	itoa(Value, pBuffer, MAX_PATH);
	return TRUE;
}


BOOL	i3XFileBinLexer::GetGUIDToken(char * pBuffer)
{
	UINT32	Field1 = 0;
	UINT16	Field2 = 0, Field3 = 0;
	UINT8	Field4[8] = {};


	if (-1 == ReadData(&Field1, sizeof(UINT32)))
		return FALSE;

	if (-1 == ReadData(&Field2, sizeof(UINT16)))
		return FALSE;

	if (-1 == ReadData(&Field3, sizeof(UINT16)))
		return FALSE;

	if (-1 == ReadData(Field4, sizeof(Field4)))
		return FALSE;

	i3String::Format(pBuffer, MAX_PATH, "<%x-%x-%x-%x%x-%x%x%x%x%x%x>", Field1, Field2, Field3,
		Field4[0], Field4[1], Field4[2], Field4[3],
		Field4[4], Field4[5], Field4[6], Field4[7]);

	return TRUE;
}


BOOL	i3XFileBinLexer::GetIntegerListToken(char * pBuffer)
{
	UINT32	Count = 0;
	if (-1 == ReadData(&Count, sizeof(UINT32)))
		return FALSE;

	m_IntList.resize(Count);

	INT32 read_cnt = 0;
	for (ListInt32::iterator iter = m_IntList.begin() ;
		iter != m_IntList.end() ; ++iter)
	{
		UINT32	value;
		if (-1 == ReadData(&value, sizeof(UINT32)))
			return FALSE;
		*iter = (INT32)value;
		read_cnt++;
	}


	return TRUE;
}


BOOL	i3XFileBinLexer::GetFloatListToken(char * pBuffer)
{
	UINT32	Count = 0;
	if (-1 == ReadData(&Count, sizeof(UINT32)))
		return FALSE;

	m_RealList.resize(Count);

	INT32 read_cnt = 0;
	for (ListReal32::iterator iter = m_RealList.begin() ;
		iter != m_RealList.end() ; ++iter)
	{
		REAL32	value;
		if (-1 == ReadData(&value, sizeof(REAL32)))
			return FALSE;
		*iter = value;
		read_cnt++;
	}

	return TRUE;
}


