#pragma once

#include <vector>

namespace rsc
{
	class IFileCrawler
	{
	public:
		virtual ~IFileCrawler() = 0;

		virtual bool operator()(std::vector<PathPtr>& out);

	private:
		virtual bool _Load() = 0;
		virtual bool _Gather(std::vector<PathPtr>& out) = 0;
	};

	void MakeUnique(std::vector<PathPtr>& out);

	class FileCrawlerWrapper : public IFileCrawler
	{
	public:
		FileCrawlerWrapper(IFileCrawler* p);
		virtual ~FileCrawlerWrapper();

		virtual bool operator()(std::vector<PathPtr>& out);

	private:
		virtual bool _Load() { return true; }
		virtual bool _Gather(std::vector<PathPtr>& out) { return true; }

	private:
		std::unique_ptr<IFileCrawler> m_crawler;
	};
}