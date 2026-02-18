#pragma once

#include "IFilePathable.h"
#include "RscTool/i3RscPartialFile.h"

struct i3ResourceFileObjectInfo2;

namespace rsc
{
	class IPefable : public IFilePathable	// 첨가 클래스에 상속 1 개 정도는 괜찮겠지...
	{
	public:
		IPefable() = default;
		IPefable(const Path& workdir, const Path& PEFRelPath);
		IPefable(const Path& workdir, const Path& PEFRelPath, const std::string& strCountryName);
		virtual ~IPefable() = 0;
		
		bool				OpenPef();
		i3RscPartialFile*	GetPef();

		void				SetCountryName(const std::string& name);
		const std::string&	GetCountryName() const;

		int					GetPefNationIdx() const;

	private:
		i3RscPartialFile m_rscfile;
		std::string m_strCountryName;
	};
}