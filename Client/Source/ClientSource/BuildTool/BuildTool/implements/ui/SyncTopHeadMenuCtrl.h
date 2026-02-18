#pragma once

#include "ugctrl/UGCtrl.h"

namespace ui
{
	class SyncPlayerBar;

	class SyncTopHeadMenuCtrl : public CUGCtrl
	{
	public:
		SyncTopHeadMenuCtrl(SyncPlayerBar* owner);
		virtual ~SyncTopHeadMenuCtrl();

	public:
		DECLARE_MESSAGE_MAP()

		virtual void OnSetup() override;
		virtual void OnLClicked(int col, long row, int updn, RECT* rect, POINT* pt, int processed) override;
		virtual COLORREF OnGetDefBackColor(int section) override;
		virtual void OnDrawFocusRect(CDC *dc, RECT *rect) override;

	public:
		int	GetTotalNumOfCol();
		void AdjustLayout(const RECT& rect);

		bool IsForceCopyMode() const;
		bool IsPlayAllMode() const;

	private:
		SyncPlayerBar* m_owner = nullptr;
		CPen m_penIvoryBlackColor;

		bool m_isForceCopyMode = true;
		bool m_isPlayAllMode = true;
	};
}