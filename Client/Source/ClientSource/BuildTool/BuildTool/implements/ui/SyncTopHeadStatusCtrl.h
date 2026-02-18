#pragma once

#include "ugctrl/UGCtrl.h"

namespace ui
{
	class SyncPlayerBar;

	class SyncTopHeadStatusCtrl : public CUGCtrl
	{
	public:
		SyncTopHeadStatusCtrl(SyncPlayerBar* owner);
		virtual ~SyncTopHeadStatusCtrl();

	public:
		DECLARE_MESSAGE_MAP()

		virtual void OnSetup() override;
		virtual COLORREF OnGetDefBackColor(int section) override;
		virtual void OnDrawFocusRect(CDC *dc, RECT *rect) override;

	public:
		void AdjustLayout(const RECT& rect);
		void SetContentsName(const std::string& strContentsName);
		void SetCountryName(const std::string& strCountryName);

	private:
		SyncPlayerBar* m_owner = nullptr;
		CPen m_penIvoryBlackColor;
	};
}