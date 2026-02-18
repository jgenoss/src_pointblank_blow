#pragma once

//
// 팩스크립트 파일 열람시에는 열람 시작때 읽기처리하고....필요없으면 바로 지우도록 한다..
// 팩스크립트 파일을 실제 task로 연결하여 패킹작업을 하려고 할때 차근하게 모으는게 좋고 그전까지는 메모리 누적 저장을 
// 지연시킨다....  
//
#include "i3Base/itl/range/iterator_range.h"

// 다음과 같이 구조변경...하나의 스크립트 파일에 여러개의 패킹 수행이 있을수 있음..

typedef i3::shared_ptr<struct PackScriptUnit>		PackScriptUnitPtr;

struct PackScriptContents 
{
	i3::vector< PackScriptUnitPtr >	unit_list;	// 	
	PackScriptUnitPtr				copy_unit;	// 
};

void CopyPackScriptContents(const PackScriptContents& src, PackScriptContents& dest);
bool operator==(const PackScriptContents& lhs, const PackScriptContents& rhs);
bool operator!=(const PackScriptContents& lhs, const PackScriptContents& rhs);

struct PackScriptUnit
{
	i3::string				pack_filename;			// 재활용가능성 있으므로 rc_string처리
	// 경로+파일이름 ( 와일드카드적용가능해야함)..재활용가능성이 적으므로 rc_string은 쓰지 않음.
	i3::vector<i3::string>		add_list;
	i3::vector<i3::string>		remove_list;
	i3::vector<i3::string>		enc_list;
	i3::vector<bool>			add_opt_list;			// 서브폴더 재귀 여부..(m_add_list와 갯수 일치해야함)
};

bool operator==(const PackScriptUnit& lhs, const PackScriptUnit& rhs);
bool operator!=(const PackScriptUnit& lhs, const PackScriptUnit& rhs);
bool IsPackScriptUnitEmpty(const PackScriptUnit& unit);



// 읽는 작업은 간단하게 하나의 함수로 끝내야한다.  (클래스까지 동원하면 낭비)

// 우선 다음과 같은 헬퍼 저장 함수를 간단하게 작성...
bool ImportPackScriptFile( const i3::wstring& full_path, PackScriptContents& out);

bool ExportPackScriptFile( const PackScriptContents& in, const i3::wstring& file_path);

//

//
// 팩스크립트유닛의 입력 문자열을 토대로 결과 파일 리스트 목록 뽑아내는 함수..
//  ADD와 Remove 각각에 쓰일수 있게 처리한다..
//  ENC의 경우는 결과물 목록에 마킹할수 있는 UI 를 감안해서 처리한다..
//  

//  add 목록 모두 계산해서 하나 뽑음 -> remove목록 모두 계산해서 하나 뽑음..

//
//  add결과물 - remove 결과물 = add_remove 결과물    (3개 모두 보존해두는게 좋다...)
//													결과물들은 case sensitive하게 처리되므로 단순비교가능.
//


//
// add_remove결과물에 enc목록 반영하여 체크표시..
//

void AddResultFilesFromPackScriptStrings(const i3::string& mediaPath,
	                                    const i3::vector<i3::string>& input, 
										const i3::vector<i3::string>& filter_list,
										const i3::vector<i3::string>& nation_filter_list,
										i3::vector<i3::string>& inoutFileList, 
										const i3::vector<bool>* recursive_list = NULL);


void AddResultFilesFromPackScriptString(const i3::string& mediaPath,
	                                          const i3::string& input, 
											  const i3::vector<i3::string>& filter_list,
											  const i3::vector<i3::string>& nation_filter_list,
											  i3::vector<i3::string>& inoutFileList,
											  bool recursive = false);


void ImportResultFilesFromAddRemoveList( const i3::vector<i3::string>& addResultList,
										const i3::vector<i3::string>& removeResultList,
										i3::vector<i3::string>& outList);

void ImportEncMappingsFromEncList( const i3::vector<i3::string>& AddRemoveResultList,
									const i3::vector<i3::string>& EncResultlist,
									i3::vector<bool>& outEncMappingList );

// 전체 결과물을 뽑는 함수..

void ImportResultFilesFromPackScriptUnit(const i3::string& mediaPath, const PackScriptUnit& unit, 
										 const i3::vector<i3::string>& filter_list,
										 const i3::vector<i3::string>& nation_filter_list,
											i3::vector<i3::string>& outFileList,
											i3::vector<bool>&		outEncMappingList);

void ImportResultFilesFromPackScriptUnit(const i3::string& mediaPath, const PackScriptUnit& unit, 
										 const i3::vector<i3::string>& filter_list,
										 const i3::vector<i3::string>& nation_filter_list,
										 i3::vector<i3::string>& outFileList,
										 i3::vector<bool>&		outEncMappingList,
										 i3::vector<i3::string>& outAddResultList, 
										 i3::vector<i3::string>& outRemoveResultList,
										 i3::vector<i3::string>& outEncResultList);

// ENC 고려하지 않고, 파일결과물 위주로만 뽑는 함수 따로 작성..

void ImportResultFilesFromPackScriptUnitWithoutENC(const i3::string& mediaPath, const PackScriptUnit& unit, 
										 const i3::vector<i3::string>& filter_list,
										 const i3::vector<i3::string>& nation_filter_list,
										 i3::vector<i3::string>& outFileList);

void ImportResultFilesFromPackScriptUnitWithoutENC(const i3::string& mediaPath, const PackScriptUnit& unit, 
										 const i3::vector<i3::string>& filter_list,
										 const i3::vector<i3::string>& nation_filter_list,
										 i3::vector<i3::string>& outFileList,
										 i3::vector<i3::string>& outAddResultList, 
										 i3::vector<i3::string>& outRemoveResultList);

//
struct PackScriptExceptionString
{
	PackScriptExceptionString& operator=(const PackScriptExceptionString& rhs) 
	{ 
		str_exception = rhs.str_exception;	  
		str_force_copy = rhs.str_force_copy;
		return *this;
	}

	i3::string		str_exception;
	i3::string		str_force_copy;
};

struct PackScriptExceptionBuffer
{
	i3::vector<i3::string>	line_list;				// 저장 문제로 버퍼를 그대로 보관해둔다..
	std::size_t				exception_line;
	std::size_t				force_copy_line;
};

struct PackScriptExceptionFilterList
{
	i3::vector<i3::string>	pack_filter_list;
	i3::vector<i3::string>	qa_filter_list;
	i3::vector<i3::string>	qa_force_copy_list;
};

void InitPackScriptException(PackScriptExceptionBuffer& out1, PackScriptExceptionString& out2);
bool LoadPackScriptExceptionFile(const i3::wstring& full_path, PackScriptExceptionBuffer& out1, PackScriptExceptionString& out2);
bool SavePackScriptExceptionFile(const i3::wstring& full_path, const PackScriptExceptionBuffer& in1, const PackScriptExceptionString& in2);

void ExtractPackScriptExceptionFileList(const PackScriptExceptionString& in, PackScriptExceptionFilterList& out);
void MakePackScriptExceptionFile(const i3::vector<i3::string>& in, i3::string& out);

struct NationList
{
	i3::vector<i3::string>	list;	
};

bool LoadNationList(const i3::wstring& full_path, NationList& out);


//
// 아래는 모두 부함수들임...
//
bool LoadFileIntoBuffer( const i3::wstring& full_path, i3::vector<char>& buff);
bool SaveFileFromBuffer( const i3::vector<char>& in_buff, const i3::wstring& full_path);

bool ImportPackScriptBuffer( const i3::vector<char>& buff, PackScriptContents& out);

void ExportPackScriptIntoBuffer(const PackScriptContents& in, i3::vector<char>& out_buff);
void ExportPackScriptUnitIntoBuffer(const PackScriptUnit* unit, i3::vector<char>& out_buff);


void ExtractLineRange( const i3::const_char_range& input_range, i3::const_char_range& out_line_range, 
					   i3::const_char_range& out_left_range);

bool ParsePackScriptLine(const i3::const_char_range& line_range, PackScriptContents& inout);

// 쓰기 작업 함수를 만들어야 한다..  ( 이건 나중에.. 기존 팩스크립트폴더를 버리지 않고 연동할 에정임)

