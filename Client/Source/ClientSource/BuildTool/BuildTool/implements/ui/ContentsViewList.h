#pragma once

#include "resource/ContentsDefines.h"
#include "CustomUGCtrl.h"

namespace ui
{
	class ContentsView;

	class ContentsViewList
	{
	public:
		ContentsViewList(ContentsView& owner);
		virtual ~ContentsViewList();

	public:
		void AdjustLayout(const RECT& rt);

		void AddContents(const std::vector<rsc::ContentsDataPtr>& contentsDataList);
		void ClearContents();
		void UpdateContents(const long row, const rsc::ContentsDataPtr contentsData);

		void				SetContentsID(const long row, const std::wstring& wstrID);
		const std::wstring	GetContentsID(const long row);

		void				SetContentsName(const long row, const std::wstring& wstrName);
		const std::wstring	GetContentsName(const long row);

		void				SetContentsCategory(const long row, const std::wstring& wstrCategory);
		const std::wstring	GetContentsCategory(const long row);

		void				SetContentsSubCategory(const long row, const std::wstring& wstrSubCategory);
		const std::wstring	GetContentsSubCategory(const long row);

		void				SetContentsStatus(const long row, const std::wstring& wstrStatus);
		const std::wstring	GetContentsStatus(const long row);

	private:
		CustomUGCtrl m_ctrl;

		CFont	m_font;
		CFont	m_fontHeading;
		
		CPen	m_penIvoryBlackLight;
		CPen	m_penGray;
		CPen	m_penOrange;
		
		CBrush	m_brushIvoryBlack;

		int		m_currSelectionRow = -1;

		friend struct ContentsViewListCtrlCallback;
	};
}