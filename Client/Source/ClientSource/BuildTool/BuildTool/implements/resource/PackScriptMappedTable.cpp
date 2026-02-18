#include "stdafx.h"
#include "PackScriptMappedTable.h"
#include <boost/filesystem.hpp>
#include "PackScriptParser.h"
#include "FileCrawlerFactory.h"
#include "FolderFilesCrawler.h"


namespace rsc
{
	void InsertTargetRelPath(PackScriptMappedTable::TableType& outList, const std::vector<std::string>& strParams, const Path& mediaWorkdir, const Path& i3PackScriptAbsPath)
	{
		Path targetRelPath = strParams[1];
		targetRelPath.make_preferred();

		bool includeSubdirOK = targetRelPath.has_extension() ? false : true;

		if (strParams.size() == 3)
			includeSubdirOK = i3::generic_is_iequal(strParams[2], "-nosub") ? false : true;

		std::string strTemp;

		if (targetRelPath.filename().string().find("*") != std::string::npos)
		{
			FolderFilesCrawler folderCwl;
			folderCwl.SetWorkdir(mediaWorkdir);
			folderCwl.SetRelativePath(targetRelPath.parent_path());
			folderCwl.SetIncludeSubFolder(false);
			folderCwl.SetFileSearchKeys({ targetRelPath.filename().string() });

			std::vector<PathPtr> asteriskRelPathList;
			folderCwl.operator()(asteriskRelPathList);

			for (auto &currRelPath : asteriskRelPathList)
			{
				strTemp = currRelPath->string();	i3::to_upper(strTemp);

				outList.insert(PackScriptMappedTable::TableType::value_type(std::make_pair(strTemp, includeSubdirOK), i3PackScriptAbsPath));
			}
		}
		else
		{
			strTemp = targetRelPath.string();	i3::to_upper(strTemp);

			outList.insert(PackScriptMappedTable::TableType::value_type(std::make_pair(strTemp, includeSubdirOK), i3PackScriptAbsPath));
		}
	}

	PackScriptMappedTable::PackScriptMappedTable()
	{

	}

	PackScriptMappedTable::~PackScriptMappedTable()
	{
		m_addList.clear();
		m_removeList.clear();
	}

	bool PackScriptMappedTable::Buildup(const Path& i3PackScriptWorkdir, const Path& mediaWorkdir, const std::string& version)
	{
		if (!fs::exists(i3PackScriptWorkdir))
			return false;

		m_i3PackScriptWorkdir = i3PackScriptWorkdir;
		
		m_addList.clear();
		m_removeList.clear();

		// 호환이 필요한 팩스크립트만(규칙이 제멋대로인) 모은다.
		// * Weapon, Avatar, Chara, World 등의 팩스크립트는 규칙이 명확하기 때문에 
		//   소스로 패킹한다(프로젝트 'pack rules' 코드들).

		// 팩스크립트 긁어 모으기.
		auto i3PackScriptCwl = CompatiblePackScriptsCrawlerFactory::i()->Create(version);

		std::vector<PathPtr> i3PackScriptRelPathList;
		i3PackScriptCwl->operator()(i3PackScriptRelPathList);

		std::vector<std::vector<std::string>> cmdParams;
		std::string strTemp;
		Path i3PackScriptAbsPath;
		for (auto &currScriptRelPath : i3PackScriptRelPathList)
		{
			i3PackScriptAbsPath = m_i3PackScriptWorkdir / *currScriptRelPath;

			PackScriptParser parser(i3PackScriptAbsPath);
			cmdParams.clear();
			parser.operator()(cmdParams);

			for (auto &params : cmdParams)
			{
				if (params.size() < 2)
					continue;

				if (i3::generic_is_iequal(params[0], "ADD"))
					InsertTargetRelPath(m_addList, params, mediaWorkdir, i3PackScriptAbsPath);
				else if (i3::generic_is_iequal(params[0], "REMOVE"))
					InsertTargetRelPath(m_removeList, params, mediaWorkdir, i3PackScriptAbsPath);
			}
		}

		return true;
	}

	bool PackScriptMappedTable::FindPackScript(const Path& srcRelPath, Path& outi3PackScriptAbsPath)
	{
		Path i3PackScriptAbsPath;
		bool ok = false;

		// 단일 파일 검색.
		if (srcRelPath.has_extension())
		{
			// 1. 원본 파일명 그대로 검색 시도.
			ok = _FindPackScript(m_addList, srcRelPath, i3PackScriptAbsPath);
		}

		if (!ok)
		{
			// 폴더 내 하위 파일들에 속하는 경우로 검색.
			Path folderRelPath = srcRelPath.has_extension() ? srcRelPath.parent_path() : srcRelPath;
			ok = _FindPackScript(m_addList, folderRelPath, i3PackScriptAbsPath);

			if (!ok)
			{
				folderRelPath = folderRelPath.parent_path();

				while (!folderRelPath.empty())
				{
					if (_FindPackScript(m_addList, folderRelPath, i3PackScriptAbsPath, true))	// 상위 폴더 경로일 경우 무조건 하위폴더를 포함하는 조건이어야만 함.
					{
						ok = true;
						break;
					}
					folderRelPath = folderRelPath.parent_path();
				}
			}
		}

		if (ok)
		{
			if (_IsRemovalPath(srcRelPath, i3PackScriptAbsPath))
				return false;

			outi3PackScriptAbsPath = i3PackScriptAbsPath;
		}

		return ok;
	}

	bool PackScriptMappedTable::_FindPackScript(const TableType& tbl, const Path& srcRelPath, Path& outi3PackScriptAbsPath)
	{
		// 하위 폴더 포함으로 검색 시도.
		if (_FindPackScript(tbl, srcRelPath, outi3PackScriptAbsPath, true))
			return true;

		// 하위 폴더 미포함으로 검색 시도.
		if (_FindPackScript(tbl, srcRelPath, outi3PackScriptAbsPath, false))
			return true;

		return false;
	}

	bool PackScriptMappedTable::_FindPackScript(const TableType& tbl, const Path& srcRelPath, Path& outi3PackScriptAbsPath, const bool isIncludeSubFolder)
	{
		std::string strRelPath(srcRelPath.string());
		i3::to_upper(strRelPath);

		Path fileRelPath(strRelPath);	
		fileRelPath.make_preferred();

		auto it = tbl.find(std::make_pair(fileRelPath.string(), isIncludeSubFolder));
		if (it != tbl.end())
		{
			outi3PackScriptAbsPath = it->second;
			return true;
		}

		return false;
	}

	bool PackScriptMappedTable::_IsRemovalPath(const Path& srcRelPath, const Path& i3PackScriptAbsPathByAddList)
	{
		Path i3PackScriptAbsPathByRemoveList;

		if (srcRelPath.has_extension())
		{
			if (_FindPackScript(m_removeList, srcRelPath, i3PackScriptAbsPathByRemoveList))
			{
				if (i3PackScriptAbsPathByRemoveList == i3PackScriptAbsPathByRemoveList)
					return true;
			}
		}

		Path folderRelPath = srcRelPath.has_extension() ? srcRelPath.parent_path() : srcRelPath;
		if (_FindPackScript(m_removeList, folderRelPath, i3PackScriptAbsPathByRemoveList))
		{
			if (i3PackScriptAbsPathByRemoveList == i3PackScriptAbsPathByRemoveList)
				return true;
		}

		folderRelPath = folderRelPath.parent_path();
		while (!folderRelPath.empty())
		{
			if (_FindPackScript(m_removeList, folderRelPath, i3PackScriptAbsPathByRemoveList, true))	// 상위 폴더 경로일 경우 무조건 하위폴더를 포함하는 조건이어야만 함.
			{
				if (i3PackScriptAbsPathByRemoveList == i3PackScriptAbsPathByRemoveList)
					return true;
			}

			folderRelPath = folderRelPath.parent_path();
		}

		return false;
	}
}