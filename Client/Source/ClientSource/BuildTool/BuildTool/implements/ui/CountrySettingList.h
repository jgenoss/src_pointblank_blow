#pragma once

#include "CustomUGCtrl.h"
#include "resource/Settings.h"
#include "ugctrl/ugctelps.h"

namespace ui
{
	class CountrySettingDlg;

	class CountrySettingList
	{
	public:
		CountrySettingList(CountrySettingDlg& owner);
		virtual ~CountrySettingList();

		void AdjustLayout(RECT& rt);
		void GetClientRect(RECT* rt);

		void AddNewRow(long* outRowUpper, long* outRowLower);
		bool DeleteCurrRow();

		void AddNewCountry(const rsc::CountryDataPtr& countryData);
		
		void SetCountryData(const long rowUpper, const long rowLower, const rsc::CountryDataPtr& countryData);
		void GetCountryDataList(std::vector<rsc::CountryData>& out);

	private:
		void _SetSelection(const long row, const int col);
		long _AddNewRowUpper();
		long _AddNewRowLower();
		void _JoinNewRow(const long rowUpper, const long rowLower);
		bool _DeleteRow(const long row);

		CustomUGCtrl m_ctrl;

		CUGEllipsisType m_ellipType;

		CPen m_penIvoryBlack, m_penIvoryBlackLight, m_penOrange;
		long m_currSelectionRow = 0;
		long m_currSelectionCol = 0;

		using CountryDataMap = std::map<int, rsc::CountryDataPtr>;
		CountryDataMap m_srcCountryDataMap;	// 입력 원본 국가 데이터.

		friend struct CountrySettingListCallback;
	};
}