#pragma once

#include <unordered_map>

namespace rsc
{
	class PackScriptMappedTable final : public i3::shared_ginst<PackScriptMappedTable>
	{
	public:
		using TableType = std::map < std::pair<std::string, bool>, Path >;	// file path, i3PackScript path.

		PackScriptMappedTable();
		~PackScriptMappedTable();

		bool Buildup(const Path& i3PackScriptWorkdir, const Path& mediaWorkdir, const std::string& ver);
		bool FindPackScript(const Path& fileRelPath, Path& outi3PackScriptAbsPath);

	private:
		bool _FindPackScript(const TableType& tbl, const Path& srcRelPath, Path& outi3PackScriptAbsPath);
		bool _FindPackScript(const TableType& tbl, const Path& srcRelPath, Path& outi3PackScriptAbsPath, const bool isIncludeSubFolder);
		bool _IsRemovalPath(const Path& srcRelPath, const Path& i3PackScriptAbsPath);

		TableType m_addList;
		TableType m_removeList;

		Path m_i3PackScriptWorkdir;
	};
}