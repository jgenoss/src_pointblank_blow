#include "stdafx.h"
#include "PackInnerFileDataMgr.h"

#include "UIDefines.h"
#include "i3PackToolTree.h"
#include "RscTool/FindPefDiff.h"	
#include "PackedFileSearchBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace 
{
	void _GatherPackInnerFileName(i3PackNode* n, std::vector<std::wstring>* pIncludedFiles)
	{
		// 부하 심하면 i3PackToolTree쪽 쓰레드쪽으로 옮겨야 할 듯	

		const INT32 num_file = n->getFileCount();
		I3_PACK_FILE_REF* info = nullptr;
		CString fileName;

		for (INT32 i = 0; i < num_file; ++i)
		{
			info = n->getFile(i);

			// files which having extension name.
			if (NULL != strstr(info->m_szName, "."))
			{
				// Restrict search target to file name.
				fileName = CString(info->m_szName);
				pIncludedFiles->push_back((LPCTSTR)fileName);
			}
		}

		const INT32 num_child = n->getChildCount();
		i3PackNode* child = nullptr;

		for (INT32 i = 0; i < num_child; ++i)
		{
			child = static_cast<i3PackNode*>(n->getChild(i));
			_GatherPackInnerFileName(child, pIncludedFiles);
		}
	}
}

namespace ui
{
	// release all data.
	void PackInnerFileDataMgr::Clear()
	{
		m_Data.clear();
	}

	// 기본적으로 rsc::CountrySettings에 세팅된 국가 경로를 기반으로 컨테이너를 할당한다.
	// 내용 로드는 dlg의 버튼이 클릭될 때마다.
	void PackInnerFileDataMgr::Init(int totalNumOfCountries)
	{
		if (true != m_Data.empty())
			Clear();

		m_Data.reserve(totalNumOfCountries);

		for (int i = 0; i < totalNumOfCountries; ++i)
		{
			PackInfoContainerPtr emptyContainer(new PackInfoContainer);
			m_Data.push_back(emptyContainer);
		}
	}

	// rsc::CountrySettings가 변경되면 PackedFileSearchBar를 통해 수정, 삭제가 이루어진다. 
	bool PackInnerFileDataMgr::Load(int countryIdx, std::wstring packFilepath, PackedFileSearchBar* pMgr)
	{
		if (countryIdx >= (int)m_Data.size())
			return false;

		m_Data[countryIdx]->clear();

		CFileFind finder;
		BOOL bWorking = false;
		CString fileName, filePath;		
		
		std::string filepath;
		i3PackVolume* vol = nullptr;
		i3PackNode* rootNode = nullptr;

		m_Data[countryIdx]->reserve(DEFAULT_PACKFILE_RESERVE_NUM);

		packFilepath = packFilepath.append(_T("\\Pack\\*.*"));
		bWorking = finder.FindFile(packFilepath.c_str());

		if (TRUE != bWorking)
			return false;

		while (bWorking)
		{
			bWorking = finder.FindNextFile();

			// when find file
			if (finder.IsArchived())
			{
				fileName = finder.GetFileName();

				// exclude current folder, upper folder, thumbnail files
				if (_T(".") == fileName ||
					_T("..") == fileName ||
					_T("Thumbs.db") == fileName)
				{
					continue;
				}

				filePath = finder.GetFilePath();
				pMgr->SetMsgBoxText(filePath);	// set file text to PackedFileSearchBar .
				
				i3PackInfo packInfo;
				packInfo._i3Packfile = filePath.GetString();

				filepath.assign(packInfo._i3Packfile.begin(), packInfo._i3Packfile.end());

				vol = LoadPackVolume(i3::string(filepath.c_str()));
				if (nullptr != vol)
				{
					rootNode = vol->getRoot();

					// fill FILENAMEINFO data.
					// Search Pack Inner files Reclusively and push back.
					_GatherPackInnerFileName(rootNode, &packInfo._i3Packinnerfiles);

					vol->Release();

					m_Data[countryIdx]->push_back(packInfo);
				}	
				
			}

			// 하위 폴더가 생기면 만들자
			//else if (finder.IsDirectory())
			//{
			//}
		}		

		return true;
	}
	void PackInnerFileDataMgr::SearchPackInnerFiles(int countryIdx, std::wstring text
								, std::vector<std::wstring>* pOutPackfilenames)
	{
		// 공백 문자열 구분
		// 나중에 필요하면 만들자
		//const wchar_t* strSpace = L" ";
		//std::wstring* pstr = wcstok(text.c_str(), strSpace);

		std::wstring src = _T("");
		size_t idxPackfileMax = m_Data[countryIdx]->size();
		size_t idxPackinnerfileMax = 0;

		for (size_t i = 0; i < idxPackfileMax; ++i)
		{
			idxPackinnerfileMax = (*m_Data[countryIdx])[i]._i3Packinnerfiles.size();

			for (size_t j = 0; j < idxPackinnerfileMax; ++j)
			{
				src = (*m_Data[countryIdx])[i]._i3Packinnerfiles[j];

				transform(src.begin(), src.end(), src.begin(), toupper);
				transform(text.begin(), text.end(), text.begin(), toupper);

				if (NULL != wcsstr(src.c_str(), text.c_str()))
				{
					pOutPackfilenames->push_back((*m_Data[countryIdx])[i]._i3Packfile);
					break;
				}
			}
		}

	}
		
	// getter.
	size_t PackInnerFileDataMgr::GetTotalNumOfPackinnerFiles(int countryIdx)
	{
		if (countryIdx >= (int)m_Data.size())
			return 0;
		
		return m_Data[countryIdx]->size();
	}
}

