#pragma once

#include "i3Base/inst/ginst.h"

//
// 툴과 메인 게임 모두에서 통용될수 있는 방식으로 정리해야한다..
// 기존코드가 너무 너무 너무 안좋아서, 아예 새로 다 짠뒤 나머지것을 정리하는 방식으로 운용한다.
//

struct i3Language_ID_Str_Pair
{
	INT32			nID = 0;						// 뭔지 모를 ID값(게임과 툴이 제각각임)
	i3::rc_string	strLanguage;				// 국가명(랭귀지로 명명은 되어있음)
};

struct i3Language_ID_Str_Set
{
	i3::vector<i3Language_ID_Str_Pair>	m_vecPair;
};


struct i3VTFilePathData
{
	i3Language_ID_Str_Set	m_LanguageSet;		// 언제든 clear/recreate되어야함..(툴위주)

	i3::rc_string			m_strUIVersion;		// 이름 하나만 정확히 들어갈것..
	INT32					m_CurrLanguagePos = 0;	// 
	INT32					m_fallbackLanguagePos = 0;	// 폴백..'Dev'가 포함된 경우 m_CurrLanguagePos를 -1로 변경한다..

	i3::rc_string m_resultCommonUIRootDirectory_arr[2];		// UI루트 경로..(필수) // true인 경우 workdir 포함
	i3::rc_string m_resultCommonUIVTResDirectory_2d_arr[2][2];	// 공통경로의 버추얼텍스쳐 경로..

	i3::rc_string m_resultLocaleUIRootDirectory_arr[2];		// 로케일 UI루트 경로.. (VirtualTexture 이름 제외) // true인 경우 workdir
	i3::rc_string m_resultVTResDirectory_2d_arr[2][2];		// 버추얼텍스쳐 경로..(신포맷,구포맷으로 나눈다..양쪽 모두 필요함)
	

	bool					m_isV3UI = false;			// 
	bool					m_bUse_i3VTexList = false;	// 버추얼텍스쳐 신규포맷 여부..(일단 12월버전까지 모두 고려해보자)
	bool					m_bModifiedPath = false;	//

};


namespace vt_util
{
	void I3_EXPORT_FRAMEWORK i3Language_ID_Str_AddLanguage(i3Language_ID_Str_Set& inout, INT32 nID, const i3::rc_string& strNation);

	bool I3_EXPORT_FRAMEWORK Check_WorkDirectory_And_Version_With_UISPath(
		const i3::literal_range& rngUIScene, i3::stack_string& outUISWorkDirectory,
		i3::stack_string& outUIVerName, bool& out_isNewUIVer);
}


class I3_EXPORT_FRAMEWORK i3UIFilePathMgr : public i3::shared_ginst<i3UIFilePathMgr>
{
public:
	~i3UIFilePathMgr();

	void Initialize();	// 어쩔수 없이 필요함..하드코딩 적재..

	void SetLanguageSetting(const i3Language_ID_Str_Set& strSet, bool isV3UI);
	void RemoveNoFolderLanguageSetting();	// 실제 폴더가 없으면 해당 국가 셋팅 제거해주는 함수..

	void SetUIVersionString(const i3::rc_string& strUIVer);	// 
	bool SetCurrentLanguageID(INT32 nID);					// 뭔지 모를 국가값..(툴과 게임이 제각각)
	bool IsLanguageLocaleEnabled() const;	// 'Dev'제외한 국가로케일 잡혀있으면 true.
	bool IsV3UI() const;

	void Enablei3VTexListFormat(bool i3VTexListEnable);
	bool Isi3VTexListFormat() const;
		

	// 디폴트 매개변수 넣지 말것.. 혼동의 여지가 크다..
	i3::rc_string Retrieve_VTexDirectory_i3VTexFormat(bool bIncludeWorkDir);
	i3::rc_string Retrieve_VTexDirectory_i3VTexListFormat(bool bIncludeWorkDir);
	i3::rc_string Retrieve_Current_VTexDirectory(bool bIncludeWorkDir);

	i3::rc_string Retrieve_CommonUIRootDirectory(bool bIncludeWorkDir);
	i3::rc_string Retrieve_LocaleUIRootDirectory(bool bIncludeWorkDir);

	i3::rc_string Retrieve_CommonUI_VTexDirectory(bool bIncludeWorkDir);
	i3::rc_string Retrieve_CommonUI_VTexDirectory_i3VTexFormat(bool bIncludeWorkDir);
	i3::rc_string Retrieve_CommonUI_VTexDirectory_i3VTexListFormat(bool bIncludeWorkDir);


	void		  ObtainUniquePath(const i3::literal_range& inputPath, i3::stack_string& outRefinedPath);	// 로케일 혹은 메인 경로를 삭제한 상대경로 리턴..
	
	i3::rc_string GetUIVersionString() const { return m_Data->m_strUIVersion; }
	i3::rc_string GetCurrentLangaugeString() const;
	i3::rc_string FindLanguageString(INT32 nID) const;

	// 저장시 호환성을 위해 버전을 다시 넣어준다.
	void		ConvertPathToOldUIStyle(const i3::literal_range& inputPath, i3::stack_string& outOldStylePath);

	// 로딩시엔 즉시 UIVersion폴더 이름을 떼버린다.
	void		ConvertPathToV3UIStyle(const i3::literal_range& inputPath, i3::stack_string& outV3UIPath);

	i3::pair<INT32, INT32> FindUIVersionStringRange(const i3::literal_range& rngPath) const;
	i3::pair<INT32, INT32> FindUIVersionStringRange_OldUI(const i3::literal_range& rngPath) const;
	i3::pair<INT32, INT32> FindUIVersionStringRange_NewUI(const i3::literal_range& rngPath) const;
	

	bool		IsIncludedUIVersionString(const i3::literal_range& rngPath) const;

private:
	i3VTFilePathData*	m_Data = nullptr;

};
