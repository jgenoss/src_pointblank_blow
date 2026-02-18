#include "i3FrameworkPCH.h"
#include "i3UIFilePathMgr.h"

#include "i3Base/itl/range/algorithm/rfind_if.h"
#include "i3Base/itl/range/algorithm/find_if.h"
#include "i3Base/string/ext/remove_end_slash.h"
#include "i3Base/string/ext/resolve_path_token.h"

#include "i3Base/string/system/is_directory.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/ext/ltrim.h"

namespace vt_detail
{
	const i3::literal_range g_rngUIRe = "UIRe";
	const i3::literal_range g_rngUI_Temp = "UI_Temp";

	const i3::literal_range g_rngUI_V10 = "UI_V10";
	const i3::literal_range g_rngUI_V12 = "UI_V12";
	const i3::literal_range g_rngUI_V11 = "UI_V11";
	const i3::literal_range g_rngUI_V15 = "UI_V15";

	const i3::literal_range g_rngLocale = "Locale";

	const i3::literal_range g_rngVirtualTexture = "VirtualTexture";
	const i3::literal_range g_rngVTexList = "VTexList";
	

	const i3::literal_range g_aOldUIVerName[2] = { g_rngUI_Temp, g_rngUIRe };
	const i3::literal_range g_aNewUIVerName[4] = { g_rngUI_V10, g_rngUI_V11, g_rngUI_V15, g_rngUI_V12 };
	const i3::literal_range g_aAllUIVerName[6] = { g_rngUI_Temp, g_rngUIRe, g_rngUI_V10, g_rngUI_V11, g_rngUI_V15, g_rngUI_V12 };
	const i3::literal_range g_aVTexName[2] = { g_rngVirtualTexture, g_rngVTexList };

	const i3::literal_range g_aObtainUniquePathFilter[] = 
	{ 
		g_rngVirtualTexture, g_rngVTexList, 
		g_rngUI_Temp, g_rngUIRe, g_rngUI_V10, g_rngUI_V11, g_rngUI_V15, g_rngUI_V12
	};

}

namespace vt_detail
{
	struct comp_path_token
	{
		const i3::literal_range& rngSearch;
		comp_path_token(const i3::literal_range& rngSearch) : rngSearch(rngSearch) {}
		bool operator()(const i3::literal_range& rngElem) const
		{
			return i3::generic_is_iequal(rngSearch, rngElem);
		}
	};

	INT32 FindUIVerName_Impl(const i3::vector<i3::literal_range>& vecToken, const i3::literal_range* arr, size_t arr_size)
	{
		typedef i3::vector<i3::literal_range>::const_iterator const_iterator;

		for (size_t i = 0; i < arr_size; ++i)
		{
			const_iterator it = i3::rfind_if(vecToken.begin(), vecToken.end(), comp_path_token(arr[i]));
			if (it != vecToken.end())
				return INT32(i3::distance(vecToken.begin(), it));
		}
		return -1;
	}
	
	INT32 FindUIVerName_OldVer( i3::vector<i3::literal_range>& vecToken)
	{
		return FindUIVerName_Impl(vecToken, g_aOldUIVerName, _countof(g_aOldUIVerName) );
	}

	INT32 FindUIVerName_NewVer(i3::vector<i3::literal_range>& vecToken)
	{
		return FindUIVerName_Impl(vecToken, g_aNewUIVerName, _countof(g_aNewUIVerName) );
	}

	INT32 FindUIVerName_AllVer(i3::vector<i3::literal_range>& vecToken)
	{
		return FindUIVerName_Impl(vecToken, g_aAllUIVerName, _countof(g_aAllUIVerName) );
	}

	// 이거 string/ext에 응용함수를 만드는게 차라리 좋겠다고 생각함...시간없으므로 일단 홀드..

	void MakePathFromPathToken(const i3::literal_range* beg, const i3::literal_range* end, i3::stack_string& out)
	{
		out.clear();
				
		const i3::literal_range* it = beg;

		for ( ;  ; )
		{
			out += *it;
			++it;

			if (it == end)
				break;

			out += '/';
		}
	}


	void MakePathFromPathToken(const i3::vector<i3::literal_range>& path_token, i3::stack_string& out)
	{
		MakePathFromPathToken(path_token.begin(), path_token.end(), out);
	}
		
	void UpdateUIVTDirectory(i3VTFilePathData*	VTFilePathData)
	{
		i3::stack_string strTemp;

		i3::stack_string strWorkingDirSlash = i3ResourceFile::GetWorkingFolder();

		bool bWorkDirExist = false;

		if (!strWorkingDirSlash.empty())
		{
			i3::make_unix_path(strWorkingDirSlash);
			i3::remove_end_slash(strWorkingDirSlash);
			strWorkingDirSlash += '/';			// 떼었다가 다시 붙임...
			bWorkDirExist = true;
		}
		
		// work dir는 툴에서만 지정되는듯 하다. 없으면 그냥 없음..
		static const bool no_workdir = false;
		static const bool include_workdir = true;

		VTFilePathData->m_resultCommonUIRootDirectory_arr[no_workdir] = VTFilePathData->m_strUIVersion;	// UIRe, UI_V10 etc..
		
		if (bWorkDirExist)
		{
			strTemp = strWorkingDirSlash;
			strTemp += VTFilePathData->m_resultCommonUIRootDirectory_arr[no_workdir];
			VTFilePathData->m_resultCommonUIRootDirectory_arr[include_workdir] = strTemp;
		}
		else
		{
			VTFilePathData->m_resultCommonUIRootDirectory_arr[include_workdir] = VTFilePathData->m_resultCommonUIRootDirectory_arr[no_workdir];
		}

		strTemp.clear();

		if (VTFilePathData->m_CurrLanguagePos != -1 && 
			VTFilePathData->m_CurrLanguagePos != VTFilePathData->m_fallbackLanguagePos) // 로케일 버전..
		{
			strTemp = g_rngLocale;
			strTemp += '/';
			const i3Language_ID_Str_Pair& pr = VTFilePathData->m_LanguageSet.m_vecPair[VTFilePathData->m_CurrLanguagePos];
			strTemp += pr.strLanguage;
			strTemp += '/';
			strTemp += VTFilePathData->m_strUIVersion;		// "UIRe", "UI_Temp", "UI_VXXX"

			VTFilePathData->m_resultLocaleUIRootDirectory_arr[no_workdir] = strTemp;
		}
		else
		{
			VTFilePathData->m_resultLocaleUIRootDirectory_arr[no_workdir] = VTFilePathData->m_resultCommonUIRootDirectory_arr[no_workdir];
		}
	

		if (bWorkDirExist)
		{
			strTemp = strWorkingDirSlash;
			strTemp += VTFilePathData->m_resultLocaleUIRootDirectory_arr[no_workdir];
			VTFilePathData->m_resultLocaleUIRootDirectory_arr[include_workdir] = strTemp;
		}
		else
		{
			VTFilePathData->m_resultLocaleUIRootDirectory_arr[include_workdir] = VTFilePathData->m_resultLocaleUIRootDirectory_arr[no_workdir];
		}
		
		static const bool isVTexFormat = false;
		static const bool isVTexListFormat = true;

		if (VTFilePathData->m_isV3UI == false)
		{
			VTFilePathData->m_resultCommonUIVTResDirectory_2d_arr[isVTexFormat][no_workdir] =
				VTFilePathData->m_resultCommonUIRootDirectory_arr[no_workdir];
		
			strTemp = VTFilePathData->m_resultCommonUIRootDirectory_arr[no_workdir];
			if (!strTemp.empty())
				strTemp += '/';
			strTemp += g_rngVTexList;  // "VTexList"
			VTFilePathData->m_resultCommonUIVTResDirectory_2d_arr[isVTexListFormat][no_workdir] = strTemp;

			VTFilePathData->m_resultVTResDirectory_2d_arr[isVTexFormat][no_workdir] =
				VTFilePathData->m_resultLocaleUIRootDirectory_arr[no_workdir];

			strTemp = VTFilePathData->m_resultLocaleUIRootDirectory_arr[no_workdir];
			if (!strTemp.empty())
				strTemp += '/';
			strTemp += g_rngVTexList;  // "VTexList"
			VTFilePathData->m_resultVTResDirectory_2d_arr[isVTexListFormat][no_workdir] = strTemp;
		}
		else
		{
			strTemp = VTFilePathData->m_resultCommonUIRootDirectory_arr[no_workdir];
			if (!strTemp.empty())
				strTemp += '/';
			strTemp += g_rngVirtualTexture; // "VirtualTexture"

			VTFilePathData->m_resultCommonUIVTResDirectory_2d_arr[isVTexFormat][no_workdir] = strTemp;

			strTemp = VTFilePathData->m_resultCommonUIRootDirectory_arr[no_workdir];
			if (!strTemp.empty())
				strTemp += '/';
			strTemp += g_rngVTexList;		// "VTexList"

			VTFilePathData->m_resultCommonUIVTResDirectory_2d_arr[isVTexListFormat][no_workdir] = strTemp;
			
			//
			strTemp = VTFilePathData->m_resultLocaleUIRootDirectory_arr[no_workdir];
			if (!strTemp.empty())
				strTemp += '/';
			strTemp += g_rngVirtualTexture; // "VirtualTexture"

			VTFilePathData->m_resultVTResDirectory_2d_arr[isVTexFormat][no_workdir] = strTemp;

			strTemp = VTFilePathData->m_resultLocaleUIRootDirectory_arr[no_workdir];
			if (!strTemp.empty())
				strTemp += '/';
			strTemp += g_rngVTexList;		// "VTexList"

			VTFilePathData->m_resultVTResDirectory_2d_arr[isVTexListFormat][no_workdir] = strTemp;
			//
		}

		if (bWorkDirExist)
		{
			strTemp = strWorkingDirSlash;
			strTemp += VTFilePathData->m_resultCommonUIVTResDirectory_2d_arr[isVTexFormat][no_workdir];
			VTFilePathData->m_resultCommonUIVTResDirectory_2d_arr[isVTexFormat][include_workdir] = strTemp;

			strTemp = strWorkingDirSlash;
			strTemp += VTFilePathData->m_resultCommonUIVTResDirectory_2d_arr[isVTexListFormat][no_workdir];
			VTFilePathData->m_resultCommonUIVTResDirectory_2d_arr[isVTexListFormat][include_workdir] = strTemp;
			

			strTemp = strWorkingDirSlash;
			strTemp += VTFilePathData->m_resultVTResDirectory_2d_arr[isVTexFormat][no_workdir];
			VTFilePathData->m_resultVTResDirectory_2d_arr[isVTexFormat][include_workdir] = strTemp;

			strTemp = strWorkingDirSlash;
			strTemp += VTFilePathData->m_resultVTResDirectory_2d_arr[isVTexListFormat][no_workdir];
			VTFilePathData->m_resultVTResDirectory_2d_arr[isVTexListFormat][include_workdir] = strTemp;
		}
		else
		{
			VTFilePathData->m_resultCommonUIVTResDirectory_2d_arr[isVTexFormat][include_workdir] =
				VTFilePathData->m_resultCommonUIVTResDirectory_2d_arr[isVTexFormat][no_workdir];
			VTFilePathData->m_resultCommonUIVTResDirectory_2d_arr[isVTexListFormat][include_workdir] =
				VTFilePathData->m_resultCommonUIVTResDirectory_2d_arr[isVTexListFormat][no_workdir];
			
			VTFilePathData->m_resultVTResDirectory_2d_arr[isVTexFormat][include_workdir] = 
				VTFilePathData->m_resultVTResDirectory_2d_arr[isVTexFormat][no_workdir];
			VTFilePathData->m_resultVTResDirectory_2d_arr[isVTexListFormat][include_workdir] = 
				VTFilePathData->m_resultVTResDirectory_2d_arr[isVTexListFormat][no_workdir];
		}
		
		VTFilePathData->m_bModifiedPath = false; // 변경사항 반영 완료..
	}


	i3::pair<INT32, INT32> FindUIVersionStringRangeImp(const i3::literal_range& rngPath, const i3::literal_range* arr, size_t arr_size)
	{
		if (rngPath.empty())
		{
			return i3::pair<INT32, INT32>(-1, -1);
		}
		
		for (size_t i = 0; i < arr_size; ++i)
		{
			i3::literal_range rngPathTemp = rngPath;
			const i3::literal_range& rngSearch = arr[i];
		
			while (!rngPathTemp.empty())
			{
				INT32 iPosFound = i3::icontain_string(rngPathTemp, rngSearch);

				if (iPosFound == -1)		// 없다..그냥 다음번 arr[i]로 향함..
					break;				

				// 디렉토리인지 아닌지 좀더 구분할 필요가 있다..구분 조건은 앞이나 뒷문자가 존재시 '/'나 '\\'가 아니면 찾는 디렉토리가 아니라는 것을 확인해야함..
				const INT32 sizeUIVerName = INT32(rngSearch.size());
				INT32 iPosFoundLast = iPosFound + sizeUIVerName;

				if (iPosFound > 0)
				{
					if (rngPathTemp[iPosFound - 1] != '/' && rngPathTemp[iPosFound - 1] != '\\')
					{
						rngPathTemp.advance_begin(iPosFoundLast);		
						continue;
					}
				}
						
				if (iPosFoundLast  < rngPathTemp.size() )
				{
					if (rngPathTemp[iPosFoundLast] != '/' && rngPathTemp[iPosFoundLast] != '\\')
					{
						rngPathTemp.advance_begin(iPosFoundLast + 1);
						continue;
					}
				}
				
				// 그렇지 않으면 찾아낸것이다..
				INT32 skipped_pos = i3::distance(rngPath.begin(), rngPathTemp.begin());
				INT32 final_pos = skipped_pos + iPosFound;

				return i3::pair<INT32, INT32>(final_pos, final_pos + sizeUIVerName);
				
			}
		}

		return i3::pair<INT32, INT32>(-1, -1);		// 못찾았으면 -1로 모두 채워낸다..

	}

}


i3UIFilePathMgr::~i3UIFilePathMgr()
{
	delete m_Data;
}


void i3UIFilePathMgr::Initialize()	// 어쩔수 없이 필요함..하드코딩 적재..
{
	m_Data = new i3VTFilePathData;

	m_Data->m_CurrLanguagePos = -1;			// 일단 미지정
	m_Data->m_fallbackLanguagePos = -1;		// 
	m_Data->m_isV3UI = true;					// 디폴트는 V3
	m_Data->m_bUse_i3VTexList = true;		// 디폴트는 신규포맷
	m_Data->m_bModifiedPath = true;			// 업데이트 요구플래그는 false초기화..
}



//
// UITemp, UIRe, UI_VXX 류가 발견되면, 그 이름을 제거한 후 미디어 경로와 비교한다..
// 워킹 디렉토리는 딱 미디어 경로 그 자체로 맞춰야함...이상한 서브디렉토리는 두지 말것..
// 워킹 디렉토리는 툴에서만 쓴다..따라서 게임에서는 이 함수를 쓰면 안된다..
namespace vt_util
{
	bool Check_WorkDirectory_And_Version_With_UISPath(
		const i3::literal_range& rngUIScene, i3::stack_string& outUISWorkDirectory,
		i3::stack_string& outUIVerName, bool& out_isNewUIVer)
	{
		outUISWorkDirectory.clear();
		outUIVerName.clear();
		out_isNewUIVer = false;

		const i3::literal_range rngWorkingFolder = i3ResourceFile::GetWorkingFolder();
		i3::stack_string strWorkingFolder = rngWorkingFolder;
		i3::make_unix_path(strWorkingFolder);

		i3::vector<i3::literal_range> vecPathToken;
		i3::resolve_path_token(rngUIScene, vecPathToken);

		// 거꾸로 찾는게 이득이므로 rfind한다..
		INT32 found_pos = vt_detail::FindUIVerName_OldVer(vecPathToken);

		if (found_pos != -1)
		{
			out_isNewUIVer = false;
			outUIVerName = vecPathToken[found_pos];
		}
		else
		{
			found_pos = vt_detail::FindUIVerName_NewVer(vecPathToken);

			if (found_pos != -1)
			{
				out_isNewUIVer = true;
				outUIVerName = vecPathToken[found_pos];
			}
			else
			{
				return false;		// 못찾으면 그냥 리턴..
			}
		}

		const size_t num_remain = found_pos;		// found된 것까지는 모두 제거로 생각..

		vt_detail::MakePathFromPathToken(vecPathToken.begin(), vecPathToken.begin() + num_remain, outUISWorkDirectory);

		if (i3::generic_is_iequal(outUISWorkDirectory, strWorkingFolder))
			return true;

		return false;
	}

	void i3Language_ID_Str_AddLanguage(i3Language_ID_Str_Set& inout, INT32 nID, const i3::rc_string& strNation)
	{
		i3Language_ID_Str_Pair pr;
		pr.nID = nID;
		pr.strLanguage = strNation;
		inout.m_vecPair.push_back(pr);
	}


}


void i3UIFilePathMgr::SetUIVersionString(const i3::rc_string& strUIVer)	// 
{
	m_Data->m_strUIVersion = strUIVer;
	m_Data->m_bModifiedPath = true;		// 계산된 결과 캐싱 갱신용도로 씀..
}

void  i3UIFilePathMgr::SetLanguageSetting(const i3Language_ID_Str_Set& strSet, bool isV3UI)
{
	m_Data->m_LanguageSet = strSet;
	m_Data->m_isV3UI = isV3UI;
	m_Data->m_bModifiedPath = true;

	m_Data->m_fallbackLanguagePos = -1;

	const size_t numLang = strSet.m_vecPair.size();

	for (size_t i = 0; i < numLang; ++i)
	{
		const i3Language_ID_Str_Pair&	pr = strSet.m_vecPair[i];

		if (i3::generic_is_iequal(pr.strLanguage, "Dev"))
		{
			m_Data->m_fallbackLanguagePos = i;			// POS이기 때문에 nID 대신 i로 넣어둠..
			break;
		}
	}
	
}

void i3UIFilePathMgr::RemoveNoFolderLanguageSetting()	// 실제 폴더가 없으면 해당 국가 셋팅 제거해주는 함수..
{
//	const size_t numLang = m_Data->m_LanguageSet.m_vecPair.size();
	i3::vector<i3Language_ID_Str_Pair>& vecPair = m_Data->m_LanguageSet.m_vecPair;

	i3::vector<i3Language_ID_Str_Pair>	vecPairNew;

	const INT32 numLang = INT32(vecPair.size());

	INT32 currLangPos_New = m_Data->m_CurrLanguagePos;			// 앞단 삭제시 땡겨야 함.. 
	INT32 fallbackLangPos_New = m_Data->m_fallbackLanguagePos;

	INT32 currLangPos_Fixed = m_Data->m_CurrLanguagePos;
	INT32 fallbackLangPos_Fixed = m_Data->m_fallbackLanguagePos;

	for (INT32 i = 0; i < numLang; ++i)
	{
		if (fallbackLangPos_Fixed == i)			// fallback이 존재하는 경우 그대로 푸쉬백 해준다.
		{
			vecPairNew.push_back(vecPair[i]);
			continue;
		}

		m_Data->m_CurrLanguagePos = i;
		m_Data->m_bModifiedPath = true;			

		i3::rc_string strLocaleRootFolder =	i3UIFilePathMgr::i()->Retrieve_LocaleUIRootDirectory(true);

		if (i3::is_directory(strLocaleRootFolder) == true)			// 아 실제로 존재하는 폴더임..
		{
			vecPairNew.push_back(vecPair[i]);
			continue;
		}
		
		// 존재 안하면 신규 컨테이너에 넣지만 않으면 됨..  다만 currLangPos와 fallbackLangPos의 경우 조절이 필요하다..
		if (i < currLangPos_Fixed)
			--currLangPos_New;
		else
		if (i == currLangPos_Fixed)
			currLangPos_New = -1;

		if (i < fallbackLangPos_Fixed)
			--fallbackLangPos_New;
	}

	////
	vecPair.swap(vecPairNew);			// 신규와 구버전을 교환해준다.
	m_Data->m_CurrLanguagePos = currLangPos_New;
	m_Data->m_fallbackLanguagePos = fallbackLangPos_New;
	m_Data->m_bModifiedPath = true;
}


bool  i3UIFilePathMgr::IsV3UI() const
{
	return m_Data->m_isV3UI;
}

i3::rc_string i3UIFilePathMgr::FindLanguageString(INT32 nID) const
{
	const i3::vector<i3Language_ID_Str_Pair>& vecPair = m_Data->m_LanguageSet.m_vecPair;

	const size_t num = vecPair.size();

	for (size_t i = 0; i < num; ++i)
	{
		const i3Language_ID_Str_Pair& pr = vecPair[i];

		if (pr.nID == nID)
			return pr.strLanguage;
	}
	
	return i3::rc_string();
}

i3::rc_string i3UIFilePathMgr::GetCurrentLangaugeString() const
{
	if (m_Data->m_CurrLanguagePos == -1)
		return i3::rc_string();

	return m_Data->m_LanguageSet.m_vecPair[m_Data->m_CurrLanguagePos].strLanguage;
}

bool i3UIFilePathMgr::SetCurrentLanguageID(INT32 nID)	// 뭔지 모를 국가값..(툴과 게임이 제각각)
{
	if (nID == -1)
	{
		m_Data->m_CurrLanguagePos = -1;
		m_Data->m_bModifiedPath = true;
		return true;
	}

	struct comp_nID
	{
		INT32 nID;
		comp_nID(INT32 nID) : nID(nID) {}
		bool operator()(const i3Language_ID_Str_Pair& elem) const {	return elem.nID == nID;	}
	};

	const i3::vector< i3Language_ID_Str_Pair>& vecPair = m_Data->m_LanguageSet.m_vecPair;

	i3::vector< i3Language_ID_Str_Pair>::const_iterator it = i3::range::find_if(vecPair, comp_nID(nID));

	if (it == vecPair.end())
	{
		return false;				// 기존 값은 갱신하지 않도록 한다..
	}

	m_Data->m_CurrLanguagePos = i3::distance(vecPair.begin(), it);

	m_Data->m_bModifiedPath = true;		// 계산된 결과 캐싱 갱신용도로 씀..

	return true;
}

bool i3UIFilePathMgr::IsLanguageLocaleEnabled() const
{
	return m_Data->m_CurrLanguagePos != -1 && m_Data->m_CurrLanguagePos != m_Data->m_fallbackLanguagePos;
}

i3::rc_string i3UIFilePathMgr::Retrieve_VTexDirectory_i3VTexFormat(bool bIncludeWorkDir)
{
	if (m_Data->m_bModifiedPath)
		vt_detail::UpdateUIVTDirectory(m_Data);
	
	return m_Data->m_resultVTResDirectory_2d_arr[false][bIncludeWorkDir];
}

i3::rc_string i3UIFilePathMgr::Retrieve_VTexDirectory_i3VTexListFormat(bool bIncludeWorkDir)
{
	if (m_Data->m_bModifiedPath)
		vt_detail::UpdateUIVTDirectory(m_Data);
	
	return m_Data->m_resultVTResDirectory_2d_arr[true][bIncludeWorkDir];
}

i3::rc_string i3UIFilePathMgr::Retrieve_Current_VTexDirectory(bool bIncludeWorkDir)
{
	if (m_Data->m_bModifiedPath)
		vt_detail::UpdateUIVTDirectory(m_Data);
	return m_Data->m_resultVTResDirectory_2d_arr[m_Data->m_bUse_i3VTexList][bIncludeWorkDir];
}


i3::rc_string i3UIFilePathMgr::Retrieve_CommonUI_VTexDirectory(bool bIncludeWorkDir)
{
	if (m_Data->m_bModifiedPath)
		vt_detail::UpdateUIVTDirectory(m_Data);
	return m_Data->m_resultCommonUIVTResDirectory_2d_arr[m_Data->m_bUse_i3VTexList][bIncludeWorkDir];
}

i3::rc_string i3UIFilePathMgr::Retrieve_CommonUI_VTexDirectory_i3VTexFormat(bool bIncludeWorkDir)
{
	if (m_Data->m_bModifiedPath)
		vt_detail::UpdateUIVTDirectory(m_Data);
	return m_Data->m_resultCommonUIVTResDirectory_2d_arr[false][bIncludeWorkDir];
}

i3::rc_string i3UIFilePathMgr::Retrieve_CommonUI_VTexDirectory_i3VTexListFormat(bool bIncludeWorkDir)
{
	if (m_Data->m_bModifiedPath)
		vt_detail::UpdateUIVTDirectory(m_Data);
	return m_Data->m_resultCommonUIVTResDirectory_2d_arr[true][bIncludeWorkDir];
}


i3::rc_string i3UIFilePathMgr::Retrieve_CommonUIRootDirectory(bool bIncludeWorkDir)
{
	if (m_Data->m_bModifiedPath)
		vt_detail::UpdateUIVTDirectory(m_Data);

	return m_Data->m_resultCommonUIRootDirectory_arr[bIncludeWorkDir];
}


i3::rc_string i3UIFilePathMgr::Retrieve_LocaleUIRootDirectory(bool bIncludeWorkDir)
{
	if (m_Data->m_bModifiedPath)
		vt_detail::UpdateUIVTDirectory(m_Data);

	return m_Data->m_resultLocaleUIRootDirectory_arr[bIncludeWorkDir];
}



void	i3UIFilePathMgr::ObtainUniquePath(const i3::literal_range& inputPath, i3::stack_string& outRefinedPath)
{
	// 기준이 되는것은 UIRe나 UI_XX 류라고 판단된다..	로케일쪽으로 판별할 필요는 없을 것이다.
	i3::pair<INT32, INT32> pr = vt_detail::FindUIVersionStringRangeImp(inputPath, vt_detail::g_aObtainUniquePathFilter, _countof(vt_detail::g_aObtainUniquePathFilter));
	// i3UIFilePathMgr::FindUIVersionStringRange(inputPath);

	if (pr.first == -1)		// UIRe , UI_XX류가 발견 안되면..그냥 그대로 복사해서 리턴해도 좋다..
	{
		i3::stack_string res;

		i3::ltrim_copy(outRefinedPath, inputPath, "\\/");			// 최상단 왼쪽 상대경로문자가 붙으면 제거..
		// outRefinedPath = inputPath;
		return;
	}

	// 그렇지 않다면, 서브스트링을 계산하여 리턴한다.
	if (pr.second >= inputPath.size())
	{
		outRefinedPath.clear();
		return;
	}
	
	++pr.second;	// 맨마지막이 아니면 /가 존재할것같으므로 하나더 전진..

	outRefinedPath.assign(inputPath.begin() + pr.second, inputPath.end());

	i3::ltrim(outRefinedPath, "\\/");
}

i3::pair<INT32, INT32> i3UIFilePathMgr::FindUIVersionStringRange(const i3::literal_range& rngPath) const
{
	return vt_detail::FindUIVersionStringRangeImp(rngPath, vt_detail::g_aAllUIVerName, _countof(vt_detail::g_aAllUIVerName));
}

i3::pair<INT32, INT32> i3UIFilePathMgr::FindUIVersionStringRange_OldUI(const i3::literal_range& rngPath) const
{
	return vt_detail::FindUIVersionStringRangeImp(rngPath, vt_detail::g_aOldUIVerName, _countof(vt_detail::g_aOldUIVerName));
}

i3::pair<INT32, INT32> i3UIFilePathMgr::FindUIVersionStringRange_NewUI(const i3::literal_range& rngPath) const
{
	return vt_detail::FindUIVersionStringRangeImp(rngPath, vt_detail::g_aNewUIVerName, _countof(vt_detail::g_aNewUIVerName));
}

bool	i3UIFilePathMgr::IsIncludedUIVersionString(const i3::literal_range& rngPath) const
{
	i3::pair<INT32,INT32> res_pr = i3UIFilePathMgr::FindUIVersionStringRange(rngPath);
	return (res_pr.first != -1);
}


// 저장시 호환성을 위해 버전을 다시 넣어준다... 저장 반영된 뒤엔 원상 복귀해야함...
void	i3UIFilePathMgr::ConvertPathToOldUIStyle(const i3::literal_range& inputPath, i3::stack_string& outOldStylePath)
{
	if (m_Data->m_isV3UI == false)		// V3 UI가 아닌 경우만 처리.....
	{
		i3::pair<INT32, INT32> pr_ui_pos = i3UIFilePathMgr::FindUIVersionStringRange_OldUI(inputPath);

		if (pr_ui_pos.first == -1)		// UIRe, UI_Temp 등이 없으면 ... 없는게 일반적...붙여야한다...
		{
			outOldStylePath = m_Data->m_strUIVersion;		// UIRe or UI_Temp

			i3::literal_range modInputPath = inputPath;

			if (!modInputPath.empty())
			{
				if (modInputPath[0] == '\\' || modInputPath[0] == '/')
					modInputPath.advance_begin(1);
			}
			
			outOldStylePath += '/';
			outOldStylePath += modInputPath;
			return;
		}
	}

	outOldStylePath = inputPath;
}

// 로딩시엔 즉시 UIVersion폴더 이름을 떼버린다... 이 경우 복귀가 필요없다..
void	i3UIFilePathMgr::ConvertPathToV3UIStyle(const i3::literal_range& inputPath, i3::stack_string& outV3UIPath)
{
	// 모든 경우를 다 처리..
	i3::pair<INT32, INT32> pr_ui_pos = i3UIFilePathMgr::FindUIVersionStringRange(inputPath);

	if (pr_ui_pos.first != -1)
	{
		INT32 strip_first = pr_ui_pos.first;
		INT32 strip_last = pr_ui_pos.second;

		if (strip_first > 0)			// 앞쪽 '/'제거..
			--strip_first;

		outV3UIPath.assign(inputPath.begin(), inputPath.begin() + strip_first);	// '/' 등 제외..

		if (outV3UIPath.empty())		// 앞쪽 디렉토리가 존재하지 않으면 뒷쪽 '/'도 떼어야한다....
		{
			if (strip_last < inputPath.size())		
				++strip_last;
		}

		outV3UIPath.append(inputPath.begin() + strip_last, inputPath.end());		// '/'가 포함되도 좋음.
	}
	else
	{
		outV3UIPath = inputPath;
	}
	
}

void i3UIFilePathMgr::Enablei3VTexListFormat(bool i3VTexListEnable)
{
	if (m_Data->m_bUse_i3VTexList == i3VTexListEnable)
		return;

	m_Data->m_bUse_i3VTexList = i3VTexListEnable;
	m_Data->m_bModifiedPath = true;
}

bool i3UIFilePathMgr::Isi3VTexListFormat() const
{
	return m_Data->m_bUse_i3VTexList;
}
