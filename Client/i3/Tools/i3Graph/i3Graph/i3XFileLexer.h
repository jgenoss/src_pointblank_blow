#pragma once


#include <list>

using std::list;

enum	XFILE_TOKENS
{
	// template list
	XTOKEN_ANIMATION,
	XTOKEN_ANIMATIONKEY,
	XTOKEN_ANIMATIONOPTIONS,
	XTOKEN_ANIMATIONSET,
	XTOKEN_BOOLEAN,
	XTOKEN_BOOLEAN2D,
	XTOKEN_COLORRGB,
	XTOKEN_COLORRGBA,
	XTOKEN_COMPRESSEDANIMATIONSET,
	XTOKEN_COORDS2D,
	XTOKEN_DECLDATA,
	XTOKEN_EFFECTDWORD,
	XTOKEN_EFFECTFLOATS,
	XTOKEN_EFFECTINSTANCE,
	XTOKEN_EFFECTPARAMDWORD,
	XTOKEN_EFFECTPARAMFLOATS,
	XTOKEN_EFFECTPARAMSTRING,
	XTOKEN_EFFECTSTRING,
	XTOKEN_FACEADJACENCY,
	XTOKEN_FLOATKEYS,
	XTOKEN_FRAME,
	XTOKEN_FRAMETRANSFORMMATRIX,
	XTOKEN_FVFDATA,
	XTOKEN_GUID,
	XTOKEN_INDEXEDCOLOR,
	XTOKEN_MATERIAL,
	XTOKEN_MATERIALWRAP,
	XTOKEN_MATRIX4X4,
	XTOKEN_MESH,
	XTOKEN_MESHFACE,
	XTOKEN_MESHFACEWRAPS,
	XTOKEN_MESHMATERIALLIST,
	XTOKEN_MESHNORMALS,
	XTOKEN_MESHTEXTURECOORDS,
	XTOKEN_MESHVERTEXCOLORS,
	XTOKEN_PATCH,
	XTOKEN_PATCHMESH,
	XTOKEN_PATCHMESH9,
	XTOKEN_PMATTRIBUTERANGE,
	XTOKEN_PMINFO,
	XTOKEN_PMVSPLITRECORD,
	XTOKEN_SKINWEIGHTS,
	XTOKEN_TEXTUREFILENAME,
	XTOKEN_TIMEDFLOATKEYS,
	XTOKEN_VECTOR,
	XTOKEN_VERTEXDUPLICATIONINDICES,
	XTOKEN_VERTEXELEMENT,
	XTOKEN_XSKINMESHHEADER,

	// etc tamplate
	XTOKEN_HEADER,	// 구버전 템플릿

	XTOKEN_TEMPLATE,		// template form 예약어
	XTOKEN_COMMENT,			// 주석

	XTOKEN_STRING,			// 문자열
	XTOKEN_NUMBER,			// 숫자

	XTOKEN_BLOCK_BEGIN,
	XTOKEN_BLOCK_END,

	XTOKEN_FILE_END,			// 더이상 읽을 토큰이 없음을 의미한다.
	XTOKEN_INVALID,		// 예기치 않은 문제를 발생했다.
};

enum
{
	TOKEN_BUF_SIZE	=	256,
};



class	i3XFileLexer
{
public:
	i3XFileLexer();
	virtual	~i3XFileLexer();

public:
	virtual	UINT32	GetToken(char* pTokenBuffer) =0;
	virtual	UINT32	GetToken();
	
	virtual	INT32	GetTokenInt(char* pTokenBuffer);
	virtual	REAL32	GetTokenReal(char* pTokenBuffer);

	BOOL	FindToken(UINT32 DestTokenNumber);
	void	SkipCurrentBlock();

public:
	virtual	void	SetStream(i3FileStream* pStream);

public:
	static	const char *	GetIdenString(UINT32 IdenNumber);
	static	UINT32			GetIdenNumber(char* pToken);
	static	INT32			GetNumTokenIdentifier();
	static	BOOL			IsTemplateIden(UINT32 IdenNumber);

protected:
	i3FileStream *		m_pStream;

	static const char*	s_pTemplateIdenStrings[];
	static	INT32		s_NumTemplateIden;
	static	char		s_Separator[];
};



class	i3XFileTextLexer : public i3XFileLexer
{
public:
	i3XFileTextLexer();
	virtual	~i3XFileTextLexer();

public:
	virtual	UINT32	GetToken(char* pTokenBuffer);

public:
	virtual	void	SetStream(i3FileStream* pStream);

private:
	char	GetNextChar(BOOL * pIsFileEnd);
	INT32	ReadLine();

private:
	enum	{STRING_BUF_SIZE = 512};
	i3LineLexer		m_Lexer;
	char			m_StringBuffer[STRING_BUF_SIZE];
	INT32			m_StringLength;
	INT32			m_CurCharIndex;
	INT32			m_LineCount;
};



class	i3XFileBinLexer : public i3XFileLexer
{
public:
	i3XFileBinLexer();
	virtual	~i3XFileBinLexer();

public:
	virtual	UINT32	GetToken(char* pTokenBuffer);

	virtual	INT32	GetTokenInt(char* pTokenBuffer);
	virtual	REAL32	GetTokenReal(char* pTokenBuffer);

public:
	virtual	void	SetStream(i3FileStream* pStream);

protected:
	INT32	ReadData(void* pBuffer, UINT32 Size);
	virtual	INT32	_ReadData(void* pBuffer, UINT32 Size);
	virtual	BOOL	_IsEOS();

private:
	UINT16	GetNextToken();
	BOOL	GetBinTokenToTextToken(UINT16 Token, char* TextBuffer);

	BOOL	GetNameToken(char * pBuffer);
	BOOL	GetStringToken(char * pBuffer);
	BOOL	GetIntegerToken(char * pBuffer);
	BOOL	GetGUIDToken(char * pBuffer);
	BOOL	GetIntegerListToken(char * pBuffer);
	BOOL	GetFloatListToken(char * pBuffer);

protected:
	UINT8 *		m_pDataBuffer;
	UINT32		m_DataCurPointer;
	UINT32		m_DataReadedSize;

	typedef			list<REAL32>	ListReal32;
	typedef			list<INT32>		ListInt32;
	ListReal32		m_RealList;
	ListInt32		m_IntList;
};


