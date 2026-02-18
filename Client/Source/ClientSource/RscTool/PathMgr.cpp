#include "stdafx.h"


#include "PathMgr.h"
#include "i3Base/string/ext/get_exec_path.h"
#include "PackQueue.h"
#include "ItemInfoMgr.h"
#include "RSCFileMgr.h"
#include "PackScriptMgr.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "PreprocessMessageCallback.h"


PathMgr::PathMgr() : m_curr_nation_idx(0), m_num_nation(0), m_bItemViewPreprocessNeed(false)
{
	InitPackPathFile_2_0(m_PackPathFile);
}

PathMgr::~PathMgr()
{

}

void	PathMgr::Initialize()
{
	i3::wstring wstrPath;
	i3::get_exec_path(wstrPath);

	wstrPath += L"/RscTool.path";
	LoadOrCreatePackPathFile(wstrPath, m_PackPathFile);
	
	InitPackScriptException(m_exception_buffer, m_exception_string);
	ExtractPackScriptExceptionFileList(m_exception_string, m_exception_filterlist);
}

void	PathMgr::SavePathFile()
{
	i3::wstring wstrPath;
	i3::get_exec_path(wstrPath);

	wstrPath += L"/RscTool.path";

	SavePackPathFile(wstrPath, m_PackPathFile);
}

void	PathMgr::SaveCurrExceptionFile()
{
	i3::wstring strDefineBat = m_unit.PackScriptPath;
	strDefineBat += L"/_Define.bat";
	SavePackScriptExceptionFile(strDefineBat, m_exception_buffer, m_exception_string);
}


void	PathMgr::SelectPathName(int idx)
{
	m_curr_nation_idx = 0;
	m_num_nation = 0;
	m_nation_folder_filter_list.clear();

	if (idx != -1)
	{
		PackPathUnit* unit = m_PackPathFile.unit_list[idx].get();
		m_unit = *unit;	
		PackQueue::i()->set_curr_path_unit(unit);
		
		i3::wstring strDefineBat = m_unit.PackScriptPath;
		strDefineBat += L"/_Define.bat";

		if ( LoadPackScriptExceptionFile(strDefineBat, m_exception_buffer, m_exception_string)	== false)
		{
			InitPackScriptException(m_exception_buffer, m_exception_string);
			SavePackScriptExceptionFile(strDefineBat, m_exception_buffer, m_exception_string);
		}
		
		ExtractPackScriptExceptionFileList(m_exception_string, m_exception_filterlist);

		// Packqueue처리 해야함..
		PackQueue::i()->set_curr_filter_list(m_exception_filterlist);
		//

		// 으음...LivePath에 Pack폴더가 있는지 점검하고 없으면 디렉토리를 생성한다..
		i3::wstring strLivePackDir = m_unit.LivePath;
		strLivePackDir += L"/Pack";
		if ( GetFileAttributesW(strLivePackDir.c_str()) == INVALID_FILE_ATTRIBUTES )
		{
			BOOL bRes = ::CreateDirectoryW(strLivePackDir.c_str(), NULL);
		}
		
		if (m_unit.Nation.empty() == false)
		{
			i3::wstring nation_full_path;
			if (FindMediaNationFileFullPath(nation_full_path))
			{
				i3::string curr_nation;
				i3::utf16_to_mb( m_unit.Nation, curr_nation);

				NationList nations;
				if ( LoadNationList(nation_full_path, nations) )
				{
					size_t num = nations.list.size();
					m_num_nation = num;

					int idx = i3::vu::int_index_of(nations.list, curr_nation);

					if (idx != -1)
					{
						m_curr_nation_idx = idx;

						if (idx != 0)
						{
							nations.list.erase(nations.list.begin() + idx);
							nations.list.erase(nations.list.begin());
							m_nation_folder_filter_list = nations.list;		// 복사로 처리..
						}

						PackQueue::i()->set_curr_nation_folder_filter_list(m_nation_folder_filter_list);
					}
				}
			}
		}
	}
	else
	{
		ResetPackPathUnit(m_unit);
	}

	//
	//  i3::signal을 쓸까 했는데..일단은 그냥 처리한다..
	//
	m_bItemViewPreprocessNeed = true;

	PackScriptMgr::i()->LoadPackScriptFolder();

	//

	//
	
}

bool	PathMgr::PreprocessForItemView(PreprocessMessageCallback* cb)
{
	if (m_bItemViewPreprocessNeed)
	{
		cb->OnStart();

		PackScriptMgr::i()->BuildPackScriptMap(cb);

		ItemInfoMgr::i()->LoadItemInfos(m_unit.MediaPath, cb);
		RSCFileMgr::i()->SetMediaPath(m_unit.MediaPath, cb);

		m_bItemViewPreprocessNeed = false;

		cb->OnEnd();

		return true;
	}	
	return false;
}

bool	PathMgr::IsCurrPathName(int idx) const
{
	return m_unit.PathName == m_PackPathFile.unit_list[idx]->PathName;
}

bool	PathMgr::IsEqualToCurrPathUnit(int idx) const
{
	return m_unit == *m_PackPathFile.unit_list[idx];
}

void	PathMgr::SetExceptionString(const PackScriptExceptionString& str)
{
	m_exception_string = str;
	ExtractPackScriptExceptionFileList(m_exception_string, m_exception_filterlist);
	
	PathMgr::SaveCurrExceptionFile();

	//  PackQueue 처리 코딩 해야함.....
	PackQueue::i()->set_curr_filter_list(m_exception_filterlist);
	//
}

namespace
{
	inline
	bool IsPathExist( const WCHAR* wstr )
	{
		return ::GetFileAttributesW(wstr) != INVALID_FILE_ATTRIBUTES;
	}

}

bool	PathMgr::IsMediaPathExist() const
{
	return IsPathExist(m_unit.MediaPath.c_str());
}

bool	PathMgr::IsQAPathExist() const
{
	return IsPathExist(m_unit.QAPath.c_str());
}

bool	PathMgr::IsLivePathExist() const
{
	return IsPathExist(m_unit.LivePath.c_str());
}

bool	PathMgr::IsPackScriptPathExist() const
{
	return IsPathExist(m_unit.PackScriptPath.c_str());
}

bool	PathMgr::IsMediaNationFileExist() const
{
	i3::stack_wstring strPath = m_unit.MediaPath;
	strPath += L"/config/Nation.nlf";
	return IsPathExist(strPath.c_str());
}

bool	PathMgr::FindMediaNationFileFullPath(i3::wstring& full_path) const
{
	full_path.clear();
	if (PathMgr::IsMediaNationFileExist() == false)
		return false;
	full_path = m_unit.MediaPath;
	full_path += L"/config/Nation.nlf";
	return true;
}

