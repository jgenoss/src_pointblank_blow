#pragma once

#include "CustomUGCtrl.h"

namespace ui
{
	class ContentsDescriptionBar;

	class ContentsDescList
	{
	public:
		ContentsDescList(ContentsDescriptionBar& owner);
		virtual ~ContentsDescList();

		void AdjustLayout(const int x, const int y, const int cx, const int cy);

		void SetContentsName(const std::wstring& wstrContentsName, const bool redraw);
		void SetContentsName(const std::wstring& wstrContentsName);

		void SetCountryName(const std::wstring& wstrCountryName, const bool redraw);
		void SetCountryName(const std::wstring& wstrCountryName);

		void SetCategoryName(const std::wstring& wstrCategory, const bool redraw);
		void SetCategoryName(const std::wstring& wstrCategory);

		void SetSubCategoryName(const std::wstring& wstrSubCategory, const bool redraw);
		void SetSubCategoryName(const std::wstring& wstrSubCategory);

		void SetContentsStatus(const std::wstring& wstrStatus, const bool redraw);
		void SetContentsStatus(const std::wstring& wstrStatus);

	private:
		CustomUGCtrl m_ctrl;

		CPen m_penIvoryBlack;
		CPen m_penIvoryBlackLight;

		friend struct ContentsDescListCallback;
	};
}