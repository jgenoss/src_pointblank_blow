#pragma once

namespace rsc
{
	class SyncLogger final : public i3::noncopyable
	{
	public:
		SyncLogger(const std::string& name);
		SyncLogger(const Path& rootDir, const std::string& name);
		~SyncLogger();

		void Log(const Path& workdir, const Path& fileRelPath);

	private:
		void _CreateLogFolder(const Path& workdir, const std::string& name);

		Path m_logWorkdir;
	};

	using SyncLoggerPtr = std::shared_ptr < SyncLogger > ;
}