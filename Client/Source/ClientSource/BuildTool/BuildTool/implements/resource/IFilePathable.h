#pragma once

namespace rsc
{
	class IFilePathable
	{
	public:
		IFilePathable() = default;
		IFilePathable(const Path& fileAbsPath);
		IFilePathable(const Path& workdir, const Path& fileRelPath);
		virtual ~IFilePathable() = 0;

		void		SetWorkdir(const Path& workdir);
		const Path&	GetWorkdir() const;

		void		SetRelativePath(const Path& fileRelPath);
		const Path& GetRelativePath() const;

		const Path& GetAbsolutePath() const;

	private:
		Path m_workdir;
		Path m_fileRelPath;
		Path m_fileAbsPath;
	};

	Path ExtractRelativePath(const Path& fileAbsPath, const Path& workdir);
}