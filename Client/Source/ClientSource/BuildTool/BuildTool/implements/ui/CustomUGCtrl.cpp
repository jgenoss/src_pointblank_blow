#include "stdafx.h"
#include "CustomUGCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	struct CustomUGCtrlCallback_Null : CustomUGCtrlCallback
	{

	};

	BEGIN_MESSAGE_MAP(CustomUGCtrl, CUGCtrl)
	END_MESSAGE_MAP()

	CustomUGCtrl::CustomUGCtrl() : m_callback(new CustomUGCtrlCallback_Null)
	{
	}

	CustomUGCtrl::~CustomUGCtrl()
	{
	}

	void CustomUGCtrl::OnSetup()
	{
		m_callback->OnSetup(this);
	}

	void CustomUGCtrl::OnLClicked(int col, long row, int updn, RECT* rect, POINT* pt, int processed)
	{
		if (updn)
			m_callback->OnLClickedDown(this, col, row, rect, pt, processed);
		else
			m_callback->OnLClickedUp(this, col, row, rect, pt, processed);
	}

	void CustomUGCtrl::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
	{
		m_callback->OnDClicked(this, col, row, rect, point, processed);
	}

	DROPEFFECT CustomUGCtrl::OnDragDrop(COleDataObject* data, int col, long row)
	{
		return m_callback->OnDragDrop(this, data, col, row);
	}

	DROPEFFECT CustomUGCtrl::OnDragEnter(COleDataObject* data)
	{
		return m_callback->OnDragEnter(this, data);
	}

	DROPEFFECT CustomUGCtrl::OnDragOver(COleDataObject* data, int col, long row)
	{
		return m_callback->OnDragOver(this, data, col, row);
	}

	void CustomUGCtrl::OnMenuCommand(int col, long row, int section, int item)
	{
		m_callback->OnMenuCommand(this, col, row, section, item);
	}

	int CustomUGCtrl::OnMenuStart(int col, long row, int section)
	{
		return m_callback->OnMenuStart(this, col, row, section);
	}

	COLORREF CustomUGCtrl::OnGetDefBackColor(int section)
	{
		return m_callback->OnGetDefBackColor(this, section);
	}

	void CustomUGCtrl::OnDrawFocusRect(CDC *dc, RECT *rect)
	{
		m_callback->OnDrawFocusRect(this, dc, rect);
	}

	void CustomUGCtrl::OnSelectionChanged(int startCol, long startRow, int endCol, long endRow, int blockNum)
	{
		m_callback->OnSelectionChanged(this, startCol, startRow, endCol, endRow, blockNum);
	}

	void CustomUGCtrl::OnKeyDown(UINT *vcKey, BOOL processed)
	{
		m_callback->OnKeyDown(this, vcKey, processed);
	}

	void CustomUGCtrl::OnCharDown(UINT *vcKey, BOOL processed)
	{
		m_callback->OnCharDown(this, vcKey, processed);
	}

	void CustomUGCtrl::OnCellChange(int oldcol, int newcol, long oldrow, long newrow)
	{
		m_callback->OnCellChange(this, oldcol, newcol, oldrow, newrow);
	}

	int CustomUGCtrl::OnEditStart(int col, long row, CWnd **edit)
	{
		return m_callback->OnEditStart(this, col, row, edit);
	}

	int CustomUGCtrl::OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey)
	{
		return m_callback->OnEditVerify(this, col, row, edit, vcKey);
	}

	int CustomUGCtrl::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
	{
		return m_callback->OnEditFinish(this, col, row, edit, string, cancelFlag);
	}

	int CustomUGCtrl::OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow)
	{
		return m_callback->OnEditContinue(this, oldcol, oldrow, newcol, newrow);
	}

	int CustomUGCtrl::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
	{
		return m_callback->OnCellTypeNotify(this, ID, col, row, msg, param);
	}
}