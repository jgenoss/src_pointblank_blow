#include "stdafx.h"
#include "IPefable.h"
#include "IContentsCrawler.h"
#include "RscTool/i3RscPartialFile.h"
#include "ResourceController.h"

namespace rsc
{
	IPefable::IPefable(const Path& workdir, const Path& PEFRelPath)
		: IFilePathable(workdir, PEFRelPath)
	{}

	IPefable::IPefable(const Path& workdir, const Path& PEFRelPath, const std::string& strCountryName)
		: IFilePathable(workdir, PEFRelPath), m_strCountryName(strCountryName)
	{}

	IPefable::~IPefable() 
	{}

	bool IPefable::OpenPef()								
	{ 
		return m_rscfile.Open(GetAbsolutePath().string().c_str()); 
	}

	i3RscPartialFile* IPefable::GetPef()								
	{ 
		return &m_rscfile; 
	}

	void IPefable::SetCountryName(const std::string& name)	
	{ 
		m_strCountryName = name;
	}

	const std::string&	IPefable::GetCountryName() const					
	{ 
		return m_strCountryName;
	}

	int IPefable::GetPefNationIdx() const					
	{
		return CountrySettings::i()->FindData(m_strCountryName)->GetPEFCountryIdx();
	}
}