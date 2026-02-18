#include "stdafx.h"
#include "CustomProgressCtrl.h"
#include "UIDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	void CustomProgressCtrlCallabck::OnSetGaugePos(CustomProgressCtrl* ctrl, const int pos)
	{
		ctrl->SetPos(pos);
	}

	CustomProgressCtrl::CustomProgressCtrl() : m_callback(std::make_unique<CustomProgressCtrlCallabck>())
	{}

	CustomProgressCtrl::~CustomProgressCtrl()
	{}

	void CustomProgressCtrl::SetGaugePos(const int pos)
	{
		PostMessage(WMU_CUSTOM_PROGRESS_GAUGE_POS, pos);
	}

	IMPLEMENT_DYNAMIC(CustomProgressCtrl, CProgressCtrl)

	BEGIN_MESSAGE_MAP(CustomProgressCtrl, CProgressCtrl)
		ON_MESSAGE(WMU_CUSTOM_PROGRESS_GAUGE_POS, &CustomProgressCtrl::OnSetGaugePos)
	END_MESSAGE_MAP()

	LRESULT CustomProgressCtrl::OnSetGaugePos(WPARAM wparam, LPARAM lparam)
	{
		m_callback->OnSetGaugePos(this, int(wparam));
		return TRUE;
	}
}