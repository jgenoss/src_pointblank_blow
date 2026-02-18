#include "stdafx.h"
#include "ShaderList.h"
#include "ShaderSyncDlg.h"

namespace ui
{
	// ShaderList.
	ShaderList::ShaderList() 
	{}

	ShaderList::~ShaderList() 
	{}

	void ShaderList::AdjustLayout(const RECT& rt)
	{
		RECT rect = rt;

		const int gap = 2;

		rect.left += gap;		rect.right -= gap;
		rect.bottom -= gap;
		m_ctrl.MoveWindow(&rect);

		const int cx = rect.right - rect.left - 17;
		m_ctrl.SetColWidth(0, cx);
	}

	void ShaderList::CreateGridCtrl(ShaderSyncDlg* owner, const int id, CustomUGCtrlCallbackPtr cb)
	{
		m_ctrl.SetCallback(std::move(cb));
		m_ctrl.CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), owner, id);
	}

	void ShaderList::OnShowShaderList(const std::string& stri3EngineAbsPath)
	{
		dynamic_cast<IShaderListPrintable*>(m_ctrl.GetCallback())->OnPrintShaderList(&m_ctrl, stri3EngineAbsPath);
	}
}