#include "stdafx.h"
#include "IFilePathable.h"
#include <boost/algorithm/string.hpp>

namespace rsc
{
	IFilePathable::~IFilePathable() 
	{}

	IFilePathable::IFilePathable(const Path& fileAbsPath)
		: m_fileAbsPath(fileAbsPath)
	{}

	IFilePathable::IFilePathable(const Path& workdir, const Path& fileRelPath)
		: m_workdir(workdir), m_fileRelPath(fileRelPath), m_fileAbsPath(workdir/fileRelPath)
	{}

	void IFilePathable::SetWorkdir(const Path& workdir)
	{ 
		m_workdir = workdir; 
		m_fileAbsPath = workdir;
	}

	const Path& IFilePathable::GetWorkdir() const		
	{ 
		return m_workdir; 
	}

	void IFilePathable::SetRelativePath(const Path& fileRelPath)
	{ 
		m_fileRelPath = fileRelPath;

		m_fileAbsPath.clear();
		m_fileAbsPath = m_workdir / fileRelPath;
	}
	
	const Path& IFilePathable::GetRelativePath() const
	{ 
		return m_fileRelPath;
	}

	const Path& IFilePathable::GetAbsolutePath() const
	{ 
		return m_fileAbsPath;
	}

	Path ExtractRelativePath(const Path& fileAbsPath, const Path& workdir)
	{
		std::string strFileAbsPath(Path(fileAbsPath).make_preferred().string());
		i3::to_upper(strFileAbsPath);

		std::string strWorkdir(Path(workdir).make_preferred().string());
		i3::to_upper(strWorkdir);

		size_t pos = strFileAbsPath.find(strWorkdir);
		if (pos != std::string::npos)
		{
			std::string strRelPath = fileAbsPath.string().substr(pos + strWorkdir.length());
			boost::algorithm::trim_left_if(strRelPath, boost::is_any_of("/"));
			boost::algorithm::trim_left_if(strRelPath, boost::is_any_of("\\"));

			return Path(strRelPath);
		}

		return Path();
	}
}