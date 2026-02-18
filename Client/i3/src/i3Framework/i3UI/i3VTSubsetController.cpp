#include "i3FrameworkPCH.h"
#include "i3VTSubsetController.h"

#include "i3VirtualTexture.h"
#include "i3UIManager.h"
#include "i3VTFileFormat.h"
#include "i3VTFileFormatController.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3UIFilePathMgr.h"

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)					// [test required] 패킹사이즈가 누락되어 추가합니다. 2017.2.14. soon9
#endif

struct ALIGN16	I3VT_SUBSET_FILE_HEADER
{
	UINT8		m_ID[4] = { 'V', 'T', 'S', '1' };
	UINT32		m_RuleCount = 0;
	UINT32		m_SubsetCount = 0;
	UINT32		pad[4] = { 0 };
};

struct ALIGN16	I3VT_SUBSET_RULE_FILEIO
{
	char		m_szPath[MAX_PATH] = { 0 };
	char		m_szName[MAX_PATH] = { 0 };
	UINT32		m_LimitSize = 0;
	UINT32		pad[4] = { 0 };
};

struct ALIGN16	I3VT_SUBSET_FILEIO
{
	char		m_szPath[MAX_PATH] = { 0 };
	INT32		m_idxRule = 0;
	UINT32		pad[8] = { 0 };
};

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

namespace vt_detail
{
	//
	// 서브셋 자체는 리팩토링 이후에도 버릴수는 없을것 같다...( 후일 텍스쳐 머징 기능 추가된다면 쓰일수 있기도 하고..)
	// 현재 레거시 버추얼 텍스쳐 구현에는 텍스쳐 머징 기능 자체가 없다.
	//

	void RemoveAllSubsetInfo(i3::vector<i3VTSubsetRule*>& inoutRuleList, i3::vector<i3VTSubset*>& inoutSubsetList)
	{
		i3::cu::for_each_delete_clear(inoutRuleList);
		i3::cu::for_each_delete_clear(inoutSubsetList);
	}

	void	Fill_I3VT_SUBSET_RULE_FILEIO(I3VT_SUBSET_RULE_FILEIO& output, const i3VTSubsetRule* input)
	{
		// Locale 패스가 섞여 있다면, 지우고 넣어주어야 한다.
		::memset(&output, 0, sizeof(I3VT_SUBSET_RULE_FILEIO));

		i3::stack_string strRefinedPath;
		i3UIFilePathMgr::i()->ObtainUniquePath(input->m_strPath, strRefinedPath);
		
		// 호환성 문제로 구버전인 경우 UIRe류를 도로 붙여야한다..
		i3::stack_string strOldStylePath;
		i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(strRefinedPath, strOldStylePath);
		
		i3::safe_string_copy(output.m_szPath, strOldStylePath, _countof(output.m_szPath));
		
		i3::safe_string_copy(output.m_szName, input->m_strName, _countof(output.m_szName));
		output.m_LimitSize = input->m_LimitSize;
	}

	void	Fill_I3VT_SUBSET_FILEIO(I3VT_SUBSET_FILEIO& output, const i3VTSubset* input)
	{
		::memset(&output, 0, sizeof(I3VT_SUBSET_FILEIO));

		// 호환성 문제로 구버전인 경우 UIRe류를 도로 붙여야한다..
		i3::stack_string strRefinedPath;
		i3UIFilePathMgr::i()->ObtainUniquePath(input->m_strPath, strRefinedPath);
		i3::stack_string strOldStylePath;
		i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(strRefinedPath, strOldStylePath);
		//

		i3::safe_string_copy(output.m_szPath, strOldStylePath, _countof(output.m_szPath));
		output.m_idxRule = input->m_idxRule;
	}
	

	i3VTSubsetRule*	AddSubsetRule(i3::vector<i3VTSubsetRule*>& inoutRuleList, const char * pszPath, const char * pszName, UINT32 limitSz)
	{
		i3VTSubsetRule* pRule = new i3VTSubsetRule;

		i3::stack_string strUniquePath;

		i3UIFilePathMgr::i()->ObtainUniquePath(pszPath, strUniquePath);
		i3::make_unix_path(strUniquePath);

		pRule->m_strPath = strUniquePath;

		i3::stack_string strName = pszName;
		i3::make_unix_path(strName);
		pRule->m_strName = strName;

		pRule->m_LimitSize = limitSz;		// 아아아..이런거 필요없을텐데..이제....호환성때문에 남김...

		inoutRuleList.push_back(pRule);

		return pRule;
	}

	INT32 AddSubset(i3::vector<i3VTSubset*>& inoutSubsetList, const i3::literal_range& rngPath, INT32 idxRule)
	{
		i3VTSubset* pSubset = new i3VTSubset;

		NetworkDump_Malloc(pSubset, sizeof(i3VTSubset), __FILE__, __LINE__);

		i3::stack_string strUniquePath; 
		i3UIFilePathMgr::i()->ObtainUniquePath(rngPath, strUniquePath);
		i3::make_unix_path(strUniquePath);

		pSubset->m_strPath = strUniquePath;

		pSubset->m_idxRule = idxRule;

		inoutSubsetList.push_back(pSubset);
		return (INT32)inoutSubsetList.size() - 1;
	}

	
	// i3Subset확장자 파일을 읽는당..
	bool LoadSubsetInfo(i3::vector<i3VTSubsetRule*>& inoutRuleList, i3::vector<i3VTSubset*>& inoutSubsetList,
		const i3::rc_string& strVirtualTextureName)
	{
		i3FileStream file;
		UINT32 i;
		bool bResult = false;
		I3VT_SUBSET_FILE_HEADER header;

		vt_detail::RemoveAllSubsetInfo(inoutRuleList, inoutSubsetList);

		// 신규포맷 구포맷 둘 중 어느것이든 가능하기 때문에 판별을 좀 해야됨...
		i3::stack_string uniquePath;
		i3UIFilePathMgr::i()->ObtainUniquePath(strVirtualTextureName, uniquePath);		// UIRe등의 모든 앞경로를 제거..

		i3::rc_string strVTexPath =	i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(true);
		i3::stack_string strFullSubsetPath = strVTexPath;
		strFullSubsetPath += '/';
		strFullSubsetPath += uniquePath;

		i3::change_file_ext(strFullSubsetPath, "i3Subset");

		if (file.Open(strFullSubsetPath.c_str(), STREAM_READ | STREAM_SHAREREAD) == false)
		{
			I3PRINTLOG(I3LOG_NOTICE, "Could not open %s file.", strFullSubsetPath);
			return false;
		}

		{
			if (file.Read(&header, sizeof(header)) == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not load a header of subset information file (%s).", strFullSubsetPath);
				goto ErrHandle;
			}
		}

		// Rules
		for (i = 0; i < header.m_RuleCount; i++)
		{
			I3VT_SUBSET_RULE_FILEIO rule;

			if (file.Read(&rule, sizeof(rule)) == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not load a subset rule information (%s).", strFullSubsetPath);
				goto ErrHandle;
			}

			vt_detail::AddSubsetRule(inoutRuleList, rule.m_szPath, rule.m_szName, rule.m_LimitSize);
		}

		// Subsets
		I3ASSERT(header.m_SubsetCount < 100000);
		for (i = 0; i < header.m_SubsetCount; i++)
		{
			I3VT_SUBSET_FILEIO subset;

			if (file.Read(&subset, sizeof(subset)) == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not load a subset rule information (%s).", strFullSubsetPath);
				goto ErrHandle;
			}

			vt_detail::AddSubset(inoutSubsetList, subset.m_szPath, subset.m_idxRule);
		}

		bResult = true;

	ErrHandle:

		file.Close();

		return bResult;
	}



	INT32	FindMatchedSubsetRule(const i3::vector<i3VTSubsetRule*>& ruleList, const i3::literal_range& rngPath)
	{
	
		i3::stack_string res_filename;

		i3UIFilePathMgr::i()->ObtainUniquePath(rngPath, res_filename);

		i3::make_unix_path(res_filename);

		const size_t numRule = ruleList.size();

		for (size_t i = 0; i < numRule; i++)
		{
			i3VTSubsetRule* pRule = ruleList[i];

			if (i3::icontain_string_head(res_filename, pRule->m_strPath))
				return i;
		}

		return -1;
	}

	// 아...이거 정말 구현이 안좋은 함수네...일단 방치...호환성때문에 어쩔수 없이 남겨둬야한다...(16.03.21.수빈)
	void MakeUniqueSubsetName(const i3VirtualTexture* pVTex, const i3::vector<i3VTSubset*>& subsetList,
		const i3::literal_range& rngPrefix, i3::stack_string& out)
	{
		
		bool bFind = false;

		i3::stack_string refinedPrefix = rngPrefix;

		i3::make_unix_path(refinedPrefix);

		const size_t numSubset = subsetList.size();

		for (INT32 cnt = 0; cnt < 1000000; cnt++)
		{
			i3::sprintf(out, "%s_%d.i3VTexSub", refinedPrefix, cnt);

			bFind = false;


			for (size_t i = 0; i < numSubset; i++)
			{
				i3VTSubset* pSubset = subsetList[i];
				if (i3::generic_is_iequal(pSubset->m_strPath, out))
				{
					bFind = true;
					break;
				}
			}

			if (bFind == false)
			{
				return;
			}
		}
	}

	INT32		AddSubsetByRule(i3VirtualTexture* pVTex, const i3::vector<i3VTSubsetRule*>& ruleList, INT32 idxRule, i3::vector<i3VTSubset*>& inout_subsetList)
	{
		i3VTSubsetRule * pRule = ruleList[idxRule];

		i3::stack_string outSubsetName;

		vt_detail::MakeUniqueSubsetName(pVTex, inout_subsetList, pRule->m_strName, outSubsetName);

		return vt_detail::AddSubset(inout_subsetList, outSubsetName, idxRule);
	}

}


i3VTSubsetController::i3VTSubsetController(i3VirtualTexture* pVTex) : m_pVTex(pVTex)
{
	m_RuleList.reserve(32);
	m_SubsetList.reserve(32);
}

i3VTSubsetController::~i3VTSubsetController()
{
	vt_detail::RemoveAllSubsetInfo(m_RuleList, m_SubsetList);
}

/*
OpenVirtualTexture --> VirtualTexture / VTexList 중 하나를 고르는 선별 코드를 재작성 해야함..또한 선별되지 않도록 강제하는 옵션도 필요함..

i3Subset
i3RefTex
i3UIL

위 3개 파일은 구버전에서 경로가 UIRe등으로 시작한다.  WorkDir를 제외한 나머지가 모두 포함됨...
신버전에서는 UIRe가 제거되어 작동된다..

호환성을 위해서는 구버전인식되면 곧바로 UIRe등을 다시 살려 붙여야 한다..

*/

bool i3VTSubsetController::SaveAllSubsetInfo()
{
	i3FileStream file;
	INT32 i;
	bool bResult = false;
	
	i3::stack_string uniquePath;
	i3UIFilePathMgr::i()->ObtainUniquePath(m_pVTex->GetNameString(), uniquePath);

	i3::rc_string strVTexPath = i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(true);

	i3::stack_string strFullPath = strVTexPath;
	strFullPath += '/';
	strFullPath += uniquePath;
	
	i3::change_file_ext(strFullPath, "i3Subset");

	if (file.Create(strFullPath.c_str(), STREAM_WRITE) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not create %s file.", strFullPath);
		return false;
	}

	{
		I3VT_SUBSET_FILE_HEADER header;

		header.m_RuleCount = this->getSubsetRuleCount();
		header.m_SubsetCount = this->getSubsetCount();

		if (file.Write(&header, sizeof(header)) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Could not save a header of subset information file (%s).", strFullPath);
			goto ErrHandle;
		}
	}

	// Rules
	{
		I3VT_SUBSET_RULE_FILEIO rule_io;

		for (i = 0; i < getSubsetRuleCount(); i++)
		{
			i3VTSubsetRule * pRule = getSubsetRule(i);

			vt_detail::Fill_I3VT_SUBSET_RULE_FILEIO(rule_io, pRule);

			if (file.Write(&rule_io, sizeof(I3VT_SUBSET_RULE_FILEIO)) == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not save a subset rule information file (%s).", strFullPath);
				goto ErrHandle;
			}
		}
	}

	// Subsets
	{
		I3VT_SUBSET_FILEIO  subset_io;

		for (i = 0; i < getSubsetCount(); i++)
		{
			i3VTSubset* pSubset = getSubset(i);

			vt_detail::Fill_I3VT_SUBSET_FILEIO(subset_io, pSubset);

			if (file.Write(&subset_io, sizeof(I3VT_SUBSET_FILEIO)) == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not save a subset information file (%s).", strFullPath);
				goto ErrHandle;
			}
		}
	}

	bResult = true;

ErrHandle:

	file.Close();

	return bResult;
}

i3VTSubsetRule*	i3VTSubsetController::AddSubsetRule(const char * pszPath, const char * pszName, UINT32 limitSz)
{
	return vt_detail::AddSubsetRule(m_RuleList, pszPath, pszName, limitSz);
}

void		i3VTSubsetController::RemoveSubsetRule(i3VTSubsetRule* pRule)
{
	size_t idx = i3::vu::index_of(m_RuleList, pRule);

	if (idx != m_RuleList.size())
	{
		m_RuleList.erase(m_RuleList.begin() + idx);
		I3_MUST_DELETE(pRule);
	}
}


void		i3VTSubsetController::RemoveAllSubsetAndFiles()
{
	INT32 i;
	i3VTSubset * pSubset;

// 이 함수는 구버전 포맷에서만 쓰여아한다.
	i3::rc_string strVTexDir = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexFormat(true);

	i3::stack_string strPathForFind = strVTexDir;
	strPathForFind += '/';
	strPathForFind += "*.*";

	const char * _i3VTexSub = "i3VtexSub";
	HANDLE				hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		ffd;

	char szExt[16];
	
	hFind = ::FindFirstFile(strPathForFind.c_str(), &ffd);

	BOOL bFind = TRUE;
	while (bFind)
	{
		bFind = ::FindNextFile(hFind, &ffd);

		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			i3::extract_fileext(ffd.cFileName, szExt);
			if (i3::generic_is_iequal(szExt, _i3VTexSub))
			{
				i3::stack_string fullPath = strVTexDir;
				fullPath += '/';
				fullPath += ffd.cFileName;
				DeleteFile(fullPath.c_str());
				
			}
		}
	}
	::FindClose(hFind);

	for (i = 0; i < getSubsetCount(); i++)
	{
		pSubset = getSubset(i);
		I3_MUST_DELETE(pSubset);
	}

	m_SubsetList.clear();

}

bool	i3VTSubsetController::LoadAllSubsetInfo()
{
	return vt_detail::LoadSubsetInfo(m_RuleList, m_SubsetList, m_pVTex->GetNameString());		// i3Subset확장자파일로 룰과 서브셋 기본 정보 (거의 파일경로내지 이름들)
																								// 호환성을 위해서만 존재..
}

i3VTSubsetRule*	i3VTSubsetController::FindSubsetRule(const i3::literal_range& rngPath) const
{
	i3::stack_string conv = rngPath;
	i3::make_unix_path(conv);

	const size_t numRule = m_RuleList.size();

	for (size_t i = 0; i < numRule; i++)
	{
		i3VTSubsetRule* pRule = m_RuleList[i];
		if (i3::generic_is_iequal(pRule->m_strPath, conv))
			return pRule;
	}

	return nullptr;

}



// 
// 임의의 경로가 적용되는 Subset이 있는지를 검색하여 반환한다.
//
// 만약 아무런 Subset Rule에 적용되지 않는 경로라면 0을 반환하여
// Subset이 아니라 기본 파일에 저장된다.

INT32		i3VTSubsetController::FindOrCreateMatchedSubset(const i3::literal_range& rngPath, UINT32 dataSize)
{
	INT32 idxRule;
	I3VT_IOHANDLE * pHandle;

	idxRule = vt_detail::FindMatchedSubsetRule(m_RuleList, rngPath);
	if (idxRule == -1)
		return 0;

	i3VTSubsetRule* pRule = m_RuleList[idxRule];

	i3VTFileFormat* pFF = m_pVTex->GetFileFormat();

	const size_t numSubset = m_SubsetList.size();

	for (size_t i = 0; i < numSubset; ++i)
	{
		i3VTSubset* pSubset = m_SubsetList[i];

		if (pSubset->m_idxRule == idxRule)
		{
			// 해당 Rule에 의해 생성된 Subset이다.
			// 파일의 크기가 Limit Size를 넘아가는가를 확인한다.
			pHandle = pFF->getFileHandle(i + 1);

			if ((pHandle->m_LastDataOffset + dataSize) < pRule->m_LimitSize)
			{
				// 여기에 추가하면 된다!
				return i + 1;
			}
		}
	}

	// 현재까지 Subset에는 용량이 충분한 해당 Subset이 없다.
	// 새로운 이름의 Subset을 추가한다.
	INT32 addedSubsetIdx = vt_detail::AddSubsetByRule(m_pVTex, m_RuleList, idxRule, m_SubsetList);

	pFF->GetFileFormatController()->CreateFileHandle(addedSubsetIdx + 1);	// Subset Index는 1-based Index이다.		// 리팩토링 대상..

	return addedSubsetIdx + 1;
}

