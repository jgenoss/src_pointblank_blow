#pragma once

#include "PackedFileSearchDlg.h"
#include "SearchedPackFiles.h"
#include "PackInnerFileDataMgr.h"
#include "LoadingProgressMsgBox.h"

namespace ui
{
	struct IPackedFileSearchBar_Callback
	{
		virtual void OnAddContents(const long row, CString strItemPath) {}
		virtual ~IPackedFileSearchBar_Callback() {}
	};
	
	// PackedFileSearch
	class SearchedPackFiles;
	class PackedFileSearchBar : public CDockablePane
	{
		DECLARE_DYNAMIC(PackedFileSearchBar)

	public:
		PackedFileSearchBar() = default;

	public:	// interface method
		bool Init();
		bool Load(int countryIdx);
		bool SearchPackInnerFiles(int countryIdx, std::wstring text);

	public: // get, set.
		size_t GetTotalNumOfPackinnerFiles(int countryIdx);
		void SetCallback(IPackedFileSearchBar_Callback* cb) { m_callback = cb; }
		IPackedFileSearchBar_Callback* GetCallback() const { return m_callback;  }
		int GetIndexOfCurrentCountry() { return m_SearchDlg->GetIndexOfCurrentCountry(); }
		void SetChangedState(bool bState) { m_bChanged = bState; }
		bool GetChangedState() { return m_bChanged; }
		void SetMsgBoxText(CString Text);

	private:
		std::unique_ptr<PackedFileSearchDlg>	m_SearchDlg = nullptr;	// search bar at top.	
		std::unique_ptr<SearchedPackFiles>		m_FileList = nullptr;		// searched pack file list.
		std::shared_ptr<PackInnerFileDataMgr>	m_DataMgr = nullptr;		// pack and pack inner file names data manager.
		std::unique_ptr<LoadingProgressMsgBox>	m_msgBox = nullptr;
		std::vector<rsc::CountryDataPtr>		m_countryList;
		IPackedFileSearchBar_Callback*			m_callback = nullptr;
		bool	m_bChanged = false;
		const int	SEARCHDLG_HEIGHT = 40;
		CBrush	m_bkGndBrush;

	public:
		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	};

} // namespace ui


