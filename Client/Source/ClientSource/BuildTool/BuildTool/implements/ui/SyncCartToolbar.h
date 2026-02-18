#pragma once

#include "ugctrl/UGCtrl.h"

namespace ui
{
	class SyncCartToolbar : public CUGCtrl
	{
	public:
		SyncCartToolbar();
		virtual ~SyncCartToolbar();

	public:
		DECLARE_MESSAGE_MAP()

		virtual void OnSetup() override;
	};
}