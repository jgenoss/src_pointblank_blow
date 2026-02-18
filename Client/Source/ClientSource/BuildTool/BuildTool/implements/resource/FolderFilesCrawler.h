#pragma once

#include "IFileCrawler.h"
#include "IFilePathable.h"

namespace rsc
{
	class FolderFilesCrawler : public IFileCrawler, public IFilePathable
	{
	public:
		FolderFilesCrawler() = default;
		virtual ~FolderFilesCrawler();

		void SetFileSearchKeys(const std::initializer_list<std::string>& strKeyList);
		void ClearFileSearchKeys();
		void SetRecursive(const bool ok);
		void SetIncludeSubFolder(const bool ok);

	protected:
		virtual bool _Load() override;
		virtual bool _Gather(std::vector<PathPtr>& out) override;

	private:
		std::vector<std::string> m_strSearchKeyList;
		bool m_recursive = true;	// 하위 폴더 경유 여부.
		bool m_isIncludeSubFolder = false;	// 경로 추출 시 디렉터리(폴더) 경로 포함 여부.
	};
}