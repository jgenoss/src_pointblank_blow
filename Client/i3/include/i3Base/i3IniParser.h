#ifndef __I3_INI_PARSER_H__
#define __I3_INI_PARSER_H__

#include "i3ElementBase.h"
#include "i3Stream.h"

#include "itl/vector.h"

#define I3_INI_NAME_SIZE	128
#define I3_INI_VALUE_SIZE	256
#define I3_INI_BUFFER_SIZE  512

// 각 섹션의 정보 저장
struct i3IniSectionInfo
{
	UINT32	Position = 0;				// 스트림 시작 위치
	UINT32	NumLines = 0;				// 줄의 개수 (섹션과 섹션 사이의 거리)
	char	Name[I3_INI_NAME_SIZE] = { 0 };		// 섹션의 이름
};

// 라벨과 값의 문자열의 저장
struct i3IniValue
{
	char	szLabel[I3_INI_NAME_SIZE] = { 0 };	// 라벨의 이름
	char	szValue[I3_INI_VALUE_SIZE] = { 0 };	// 값의 문자열
	INT32	nLength = 0;					// 값의 문자열 길이
};

class I3_EXPORT_BASE i3IniParser : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3IniParser, i3ElementBase );
protected:

	char				m_szTemp[I3_INI_BUFFER_SIZE] = { 0 };

	bool				m_bAllowDuplicatedSection = false;			// 섹션 이름 중복 허용 여부

	bool				m_bFromFile = false;						// 직접 파일을 열어서 읽음.

	i3Stream			*m_pStream = nullptr;				// 입력 스트림 포인터

	i3::vector<i3IniSectionInfo*>	m_SectionList;			// 섹션 리스트

	char				m_szSectionName[I3_INI_NAME_SIZE] = { 0 };	// 현재 섹션의 이름

	i3IniValue			*m_pValueTable = nullptr;						// 읽어들인 값과 이름을 보관

	UINT32				m_nNumValues = 0;						// 읽어들인 값의 개수

	bool				m_bUnicode = false;

	void RemoveWhiteSpace( char *szOut, const char *szInput );
	bool ProcessString( char *szOut, const char *szInput, INT32 nOutBufLength = -1 );
	void DeleteAllValues();

	UINT32 _ReadLine( char * szLine);

public:
	virtual ~i3IniParser();

	void Close();

	// Create to save
	bool Create( const char *szFileName );

	// Get Temp Buffer (for Debug or Error Trace)
	char*	GetTempBuffer(){ return m_szTemp; }

	// Open
	bool OpenFromFile( const char *szFileName, bool bAllowDuplicatedSection = false);
	bool Open( i3Stream *pStream, bool bAllowDuplicatedSection = false, bool bFromFile = false);

	INT32 GetSectionCount() const { return INT32(m_SectionList.size()); }
	i3IniSectionInfo *GetSection( INT32 nIndex ) const { return m_SectionList[nIndex]; }
	i3IniSectionInfo *FindSection( const char *szName );
	INT32 GetSectionIndex( const char *szName );

	const char *GetCurSectionName(){ return m_szSectionName; }

	bool ReadSection( const char *szName );
	bool ReadSectionByIndex( INT32 nIndex );
	bool ReadSectionByInfo( i3IniSectionInfo *pInfo );

	UINT32 GetValueCount(){ return m_nNumValues; }
	i3IniValue* GetValueByIndex( INT32 nIndex ){ return &m_pValueTable[ nIndex ]; }

	const char *GetStringValue( const char *szLabel ); // Use internal buffer and return its pointer.
	bool GetString( const char *szLabel,	char *szValue,	INT32 nBufLength = -1);
	bool GetValue( const char *szLabel,	INT32 *nValue	);
	bool GetValue( const char *szLabel,	UINT32 *nValue	);
	bool GetValue( const char *szLabel,	REAL32 *rValue	);
	bool GetValue( const char *szLabel,	REAL64 *rValue	);

	bool GetString( const char *szLabel,	const char *szDefault,	char *szValue,	INT32 nBufLength = -1);
	bool GetValue( const char *szLabel,	INT32 nDefault,	INT32 *nValue	);
	bool GetValue( const char *szLabel,	UINT32 nDefault,	UINT32 *nValue	);
	bool GetValue( const char *szLabel,	REAL32 rDefault,	REAL32 *rValue	);
	bool GetValue( const char *szLabel,	REAL64 rDefault,	REAL64 *rValue	);

	INT32 GetValues( const char *szLabel, INT32 *nValues, INT32 NumValues, INT32 nDelimiter );
	INT32 GetValues( const char *szLabel, REAL32 *rValues, INT32 NumValues, INT32 nDelimiter );

	INT32 GetValues( const char *szLabel, INT32 nDefault, INT32 *nValues, INT32 NumValues, INT32 nDelimiter );
	INT32 GetValues( const char *szLabel, REAL32 rDefault, REAL32 *rValues, INT32 NumValues, INT32 nDelimiter );
	INT32 GetValues( const char *szLabel, INT32 *nDefault, INT32 *nValues, INT32 NumValues, INT32 nDelimiter );
	INT32 GetValues( const char *szLabel, REAL32 *rDefault, REAL32 *rValues, INT32 NumValues, INT32 nDelimiter );

	bool GetIndex( const char *szLabel, const char *szIndexString[], UINT32 nDefault, INT32 &nValue, UINT32 nCount );

	#if !defined( I3_WIPI)
	bool Write( const char *szLabel, const char *szFmt, ... );		// 문자열의 내부의 제어코드에 대한 변환 안함.
	bool WriteSectionName( const char *szName );

	bool WriteBool( const char *szLabel, bool bTrue );
	bool WriteValue( const char *szLabel, const char *szValuse );	// 문자열의 내부의 제어코드는 변환되어서 저장됨.
	bool WriteValue( const char *szLabel, INT32 nValue );
	bool WriteValue( const char *szLabel, UINT32 nValue );
	bool WriteValue( const char *szLabel, REAL32 rValue );
	bool WriteValue( const char *szLabel, REAL64 rValue );
	#endif
};

#endif //__I3_INI_PARSER_H__
