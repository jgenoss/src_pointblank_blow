#ifndef _HTML_PARSER_H
#define _HTML_PARSER_H

// Html코드에서 해당 테그의 값을 추출하는 파싱 클래스 
// 2011.05.11 안국정
// 2011.05.16 안국정(수정)

#define		MAX_TAG_COUNT_HTMLPARSER		16
#define		MAX_TAG_LENGTH_HTMLPARSER		32
#define		MAX_VALUE_LENGTH_HTMLPARSER		64

class CHtmlParser
{
private:
	char	m_pszTag[MAX_TAG_COUNT_HTMLPARSER][MAX_TAG_LENGTH_HTMLPARSER];
	INT8	m_i8numTag;
	char	m_pszValue[MAX_TAG_COUNT_HTMLPARSER][MAX_VALUE_LENGTH_HTMLPARSER];
	BOOL	m_bParsed;									// Run 메소드를 실행했는지 여부

public:
	CHtmlParser() { TagReset(); }
	void	TagReset()	{ m_i8numTag = 0; m_bParsed = FALSE; }
	void	addTag( char* pszTag );						// 파싱할 태그 추가
	void	RunParse( char* pszText );						// 실제 파싱 작업 실행
	BOOL	Compare( char* pszTag, char* pszValue );	// 해당 태그의 스트링을 비교
	BOOL	CompareByIdx( INT8 i8Index, char* pszValue );	// 해당 인덱스 태그의 스트링을 비교
	INT32	GetINT32( INT8 i8Index );					// 해당 인덱스 태그의 값을 int로 리턴
};

#endif // #ifndef _HTML_PARSER_H