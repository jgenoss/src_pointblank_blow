#pragma once

#include "PackedFileSearchBar.h"
#include "PackInnerFileDataMgr.h"

namespace ui
{
	enum COLTYPE
	{
		COLTYPE_FILENAME = 0,
		COLTYPE_FILEFULLPATH,
		COLTYPE_MAX
	};
	
	class CustomUGCtrlModify;
	class SearchedPackFiles
	{
	public:
		SearchedPackFiles(PackedFileSearchBar& owner);
		~SearchedPackFiles();

	private:
		CustomUGCtrlModify* m_pCtrl = nullptr;

		CFont	m_font;
		CFont	m_fontHeading;
		
	public:
		CString GetFileString(int row, COLTYPE eType);

	public:
		void AdjustLayout(const RECT& rt);
		void Add(const std::vector<std::wstring>& NameContainer);
		void ClearList();

	};
}

