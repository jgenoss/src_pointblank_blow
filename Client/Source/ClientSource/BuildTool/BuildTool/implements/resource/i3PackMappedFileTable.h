#pragma once

#include <unordered_map>

namespace rsc
{
	class Ii3PackMappedFileTable
	{
	public:
		virtual ~Ii3PackMappedFileTable() {}

		virtual bool Buildup(const Path& workdir) = 0;
		virtual bool IsExist(const Path& fileRelPath) = 0;
		virtual bool Find(const Path& fileRelPath, Path& outi3PackAbsPath) = 0;
	};

	class i3PackMappedFileTable_Null : public Ii3PackMappedFileTable
	{
	public:
		i3PackMappedFileTable_Null() = default;
		virtual ~i3PackMappedFileTable_Null() {}

		virtual bool Buildup(const Path& workdir) override		 { return false; }
		virtual bool IsExist(const Path& fileRelPath) override	 { return false; }
		virtual bool Find(const Path& fileRelPath, Path& outi3PackAbsPath) override
		{ 
			static const Path s_nullpath; outi3PackAbsPath = s_nullpath; return false;
		}
	};


	// 앞으로 팩파일을 다시 읽어들여야 하므로 동기화 시켜줘야 함!!!.
	class i3PackMappedFileTable : public Ii3PackMappedFileTable
	{
	public:
		i3PackMappedFileTable() = default;
		virtual ~i3PackMappedFileTable();

		virtual bool Buildup(const Path& workdir) override;
		virtual bool IsExist(const Path& fileRelPath) override;
		virtual bool Find(const Path& fileRelPath, Path& outi3PackAbsPath) override;

	private:
		using MapType = std::unordered_map<std::string, Path>;
		MapType m_table;

		Path m_workdir;
	};
}