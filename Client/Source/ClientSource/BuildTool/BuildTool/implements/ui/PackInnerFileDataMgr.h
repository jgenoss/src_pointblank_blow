#pragma once

#include <vector>
#include "../resource/CountrySettings.h"

namespace ui
{	
	const BYTE DEFAULT_INNERFILE_RESERVE_NUM = 50;
	const UINT DEFAULT_PACKFILE_RESERVE_NUM = 2000;
		
	struct i3PackInfo
	{
		std::wstring	_i3Packfile;	// Pack File Name.
		std::vector<std::wstring>	_i3Packinnerfiles;	// Pack Inner File Names(vector container).
	};

	using PackInfoContainer = std::vector<i3PackInfo>;
	using PackInfoContainerPtr = std::shared_ptr<PackInfoContainer>;
	using PackInfoContainerPtrList = std::vector<PackInfoContainerPtr>;

	class PackedFileSearchBar;
	class PackInnerFileDataMgr final //: public i3::shared_ginst<PackInnerFileDataMgr>
	{
	public:
		PackInnerFileDataMgr() = default;

	public:	// interface method
		void Init(int totalNumOfCountries);
		bool Load(int countryIdx, std::wstring packFilepath, PackedFileSearchBar* pMgr);
		void Clear();
		void SearchPackInnerFiles(int countryIdx, std::wstring text
			, std::vector<std::wstring>* pOutPackfilenames);

	public: // getter.
		size_t GetTotalNumOfPackinnerFiles(int countryIdx);

	private:
		PackInfoContainerPtrList	m_Data;	// vector container which is vector nested vector container, LOL.
		
	};

}
