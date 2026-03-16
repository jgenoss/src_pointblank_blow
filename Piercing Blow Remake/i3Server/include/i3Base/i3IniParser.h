#ifndef __I3_INI_PARSER_H__
#define __I3_INI_PARSER_H__

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"
#include "i3Stream.h"
#include "i3List.h"

#define I3_INI_NAME_SIZE	128
#define I3_INI_VALUE_SIZE	256
#define I3_INI_BUFFER_SIZE  512

// 각 섹션의 정보 저장
struct i3IniSectionInfo
{
	UINT32	Position;				// 스트림 시작 위치
	UINT32	NumLines;				// 줄의 개수 (섹션과 섹션 사이의 거리)
	char	Name[I3_INI_NAME_SIZE];		// 섹션의 이름
};

// 라벨과 값의 문자열의 저장
struct i3IniValue
{
	char	szLabel[I3_INI_NAME_SIZE];	// 라벨의 이름
	char	szValue[I3_INI_VALUE_SIZE];	// 값의 문자열
	INT32	nLength;					// 값의 문자열 길이
};

class I3_EXPORT_BASE i3IniParser : public i3ElementBase
{
	I3_CLASS_DEFINE(i3IniParser);
protected:

	char				m_szTemp[I3_INI_BUFFER_SIZE];

	BOOL				m_bAllowDuplicatedSection;			// 섹션 이름 중복 허용 여부

	BOOL				m_bFromFile;						// 직접 파일을 열어서 읽음.

	i3Stream* m_pStream;							// 입력 스트림 포인터

	i3List* m_pSectionList;					// 섹션 리스트

	char				m_szSectionName[I3_INI_NAME_SIZE];	// 현재 섹션의 이름

	i3IniValue* m_pValueTable;						// 읽어들인 값과 이름을 보관

	UINT32				m_nNumValues;						// 읽어들인 값의 개수

	bool				m_bUnicode;

	char				m_szi3INIFileName[MAX_PATH];		// 파일명
	char				m_szi3INIFileNameExt[MAX_PATH];		// 파일명 + 확장자

	void RemoveWhiteSpace(char* szOut, const char* szInput);
	BOOL ProcessString(char* szOut, const char* szInput, INT32 nOutBufLength = -1);
	void DeleteAllValues();

	UINT32 _ReadLine(char* szLine);

public:
	i3IniParser();
	virtual ~i3IniParser();

	void Close();

	// Create to save
	BOOL Create(const char* szFileName);

	// Get Temp Buffer (for Debug or Error Trace)
	char* GetTempBuffer() { return m_szTemp; }

	// Open
	BOOL OpenFromFile(const char* szFileName, BOOL bAllowDuplicatedSection = FALSE);
	BOOL Open(i3Stream* pStream, BOOL bAllowDuplicatedSection = FALSE, BOOL bFromFile = FALSE);

	INT32 GetSectionCount() { return m_pSectionList->GetCount(); }
	i3IniSectionInfo* GetSection(INT32 nIndex) { return (i3IniSectionInfo*)m_pSectionList->GetItem(nIndex); }
	i3IniSectionInfo* FindSection(const char* szName);
	INT32 GetSectionIndex(const char* szName);

	const char* GetCurSectionName() { return m_szSectionName; }
	const char* GetFileName() { return m_szi3INIFileName; }
	const char* GetFileNameExt() { return m_szi3INIFileNameExt; }

	BOOL ReadSection(const char* szName);
	BOOL ReadSectionByIndex(INT32 nIndex);
	BOOL ReadSectionByInfo(i3IniSectionInfo* pInfo);

	UINT32 GetValueCount() { return m_nNumValues; }
	i3IniValue* GetValueByIndex(INT32 nIndex) { return &m_pValueTable[nIndex]; }

	const char* GetStringValue(const char* szLabel); // Use internal buffer and return its pointer.
	BOOL GetValue(const char* szLabel, char* szValue, INT32 nBufLength = -1);
	BOOL GetValue(const char* szLabel, INT32* nValue);
	BOOL GetValue(const char* szLabel, UINT32* nValue);
	BOOL GetValue(const char* szLabel, REAL32* rValue);
	BOOL GetValue(const char* szLabel, REAL64* rValue);

	BOOL GetValue(const char* szLabel, const char* szDefault, char* szValue, INT32 nBufLength = -1);
	BOOL GetValue(const char* szLabel, INT32 nDefault, INT32* nValue);
	BOOL GetValue(const char* szLabel, UINT32 nDefault, UINT32* nValue);
	BOOL GetValue(const char* szLabel, REAL32 rDefault, REAL32* rValue);
	BOOL GetValue(const char* szLabel, REAL64 rDefault, REAL64* rValue);

	INT32 GetValues(const char* szLabel, INT32* nValues, INT32 NumValues, INT32 nDelimiter);
	INT32 GetValues(const char* szLabel, REAL32* rValues, INT32 NumValues, INT32 nDelimiter);

	INT32 GetValues(const char* szLabel, INT32 nDefault, INT32* nValues, INT32 NumValues, INT32 nDelimiter);
	INT32 GetValues(const char* szLabel, REAL32 rDefault, REAL32* rValues, INT32 NumValues, INT32 nDelimiter);
	INT32 GetValues(const char* szLabel, INT32* nDefault, INT32* nValues, INT32 NumValues, INT32 nDelimiter);
	INT32 GetValues(const char* szLabel, REAL32* rDefault, REAL32* rValues, INT32 NumValues, INT32 nDelimiter);

	BOOL GetIndex(const char* szLabel, const char* szIndexString[], UINT32 nDefault, INT32& nValue, UINT32 nCount);

#if !defined( I3_WIPI)
	BOOL Write(const char* szLabel, const char* szFmt, ...);		// 문자열의 내부의 제어코드에 대한 변환 안함.
	BOOL WriteSectionName(const char* szName);

	BOOL WriteBool(const char* szLabel, BOOL bTrue);
	BOOL WriteValue(const char* szLabel, const char* szValuse);	// 문자열의 내부의 제어코드는 변환되어서 저장됨.
	BOOL WriteValue(const char* szLabel, INT32 nValue);
	BOOL WriteValue(const char* szLabel, UINT32 nValue);
	BOOL WriteValue(const char* szLabel, REAL32 rValue);
	BOOL WriteValue(const char* szLabel, REAL64 rValue);
#endif
};

#endif //__I3_INI_PARSER_H__
