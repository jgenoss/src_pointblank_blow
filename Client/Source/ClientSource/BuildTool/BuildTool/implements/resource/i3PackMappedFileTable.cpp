#include "stdafx.h"
#include "i3PackMappedFileTable.h"
#include <boost/filesystem.hpp>
#include "RscTool/FindPefDiff.h"

namespace
{
	void MakeTablekey(const Path& fileAbsPath, std::string& outKey)
	{
		const_cast<Path&>(fileAbsPath).make_preferred();

		i3::stack_string fileAbsUpperPath(fileAbsPath.string());
		i3::to_upper(fileAbsUpperPath);

		outKey.assign(fileAbsUpperPath.begin(), fileAbsUpperPath.end());
	}
}

namespace rsc
{
	i3PackMappedFileTable::~i3PackMappedFileTable()
	{
		m_table.clear();
	}

	bool i3PackMappedFileTable::Buildup(const Path& workdir)
	{
		m_table.clear();

		Path livePackAbsPath = workdir / "Pack";
		if (!fs::exists(livePackAbsPath))
			return false;

		m_workdir = workdir;

		for (fs::directory_iterator it(livePackAbsPath); it != fs::directory_iterator(); ++it)
		{
			if (fs::is_directory(it->status()))
				continue;

			if (!i3::generic_is_iequal((*it).path().extension().string(), ".i3Pack"))	// i3Pack 파일만 처리.
				continue;

			i3::string i3PackAbsPath(it->path().string());

			i3PackVolume* vol = LoadPackVolume(i3PackAbsPath);
			if (vol == nullptr)
				continue;

			if (vol->getRoot() == nullptr)
				continue;

			i3::vector<i3::string> nodeList;
			GatherInPackFileNode(vol->getRoot(), workdir.string().c_str(), nodeList);

			vol->Release();

			std::string tableKey;

			for (auto &fileAbsPath : nodeList)
			{
				MakeTablekey(fileAbsPath.c_str(), tableKey);

				m_table.insert(MapType::value_type(tableKey, i3PackAbsPath.c_str()));
			}
		}

		return true;
	}

	bool i3PackMappedFileTable::Find(const Path& fileRelPath, Path& outi3PackAbsPath)
	{
		std::string key;	MakeTablekey(m_workdir / fileRelPath, key);

		auto it = m_table.find(key);
		if (it == m_table.end())
		{
			static Path s_nullPath;
			outi3PackAbsPath = s_nullPath;
			return false;
		}

		outi3PackAbsPath = it->second;
		return true;
	}

	bool i3PackMappedFileTable::IsExist(const Path& fileRelPath)
	{
		std::string key;	MakeTablekey(m_workdir / fileRelPath, key);

		return (m_table.find(key) != m_table.end());
	}
}