#pragma once

#include "ugctrl/UGCtrl.h"
#include "UIDefines.h"

namespace ui
{
	class CustomUGCtrl;

	struct CustomUGCtrlCallback
	{
		virtual void		OnSetup(CUGCtrl* ctrl) {}

		virtual void		OnLClickedUp(CUGCtrl* ctrl, int col, long row, RECT* rect, POINT* pt, int processed) {}
		virtual void		OnLClickedDown(CUGCtrl* ctrl, int col, long row, RECT* rect, POINT* pt, int processed) {}
		virtual void		OnDClicked(CUGCtrl* ctrl, int col, long row, RECT* rt, POINT* pt, BOOL processed) {}

		virtual void		OnSelectionChanged(CUGCtrl* ctrl, int startCol, long startRow, int endCol, long endRow, int blockNum) {}
		
		virtual void		OnMenuCommand(CUGCtrl* ctrl, int col, long row, int section, int item) {}
		virtual int			OnMenuStart(CUGCtrl* ctrl, int col, long row, int section) { return 0; }

		virtual DROPEFFECT	OnDragDrop(CUGCtrl* ctrl, COleDataObject* data, int col, long row) { return DROPEFFECT_NONE; }
		virtual DROPEFFECT	OnDragEnter(CUGCtrl* ctrl, COleDataObject* data) { return DROPEFFECT_NONE;  }
		virtual DROPEFFECT	OnDragOver(CUGCtrl* ctrl, COleDataObject* data, int col, long row) { return DROPEFFECT_NONE; }

		virtual COLORREF	OnGetDefBackColor(CUGCtrl* ctrl, int section) { return COLOR_IVORY_BLACK; }
		virtual void		OnDrawFocusRect(CUGCtrl* ctrl, CDC *dc, RECT *rect) {}

		virtual void		OnKeyDown(CUGCtrl* ctrl, UINT *vcKey, BOOL processed) {}
		virtual void		OnCharDown(CUGCtrl* ctrl, UINT *vcKey, BOOL processed) {}
		virtual void		OnCellChange(CUGCtrl* ctrl, int oldcol, int newcol, long oldrow, long newrow) {}

		virtual int			OnEditStart(CUGCtrl* ctrl, int col, long row, CWnd **edit) { return TRUE; }
		virtual int			OnEditVerify(CUGCtrl* ctrl, int col, long row, CWnd *edit, UINT *vcKey) { return TRUE; }
		virtual int			OnEditFinish(CUGCtrl* ctrl, int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag) { return TRUE; }
		virtual int			OnEditContinue(CUGCtrl* ctrl, int oldcol, long oldrow, int* newcol, long* newrow) { return TRUE; }

		virtual int			OnCellTypeNotify(CUGCtrl* ctrl, long ID, int col, long row, long msg, long param) { return TRUE; }

		virtual ~CustomUGCtrlCallback() {}
	};

	using CustomUGCtrlCallbackPtr = std::unique_ptr<CustomUGCtrlCallback>;

	class CustomUGCtrl : public CUGCtrl
	{
	public:
		CustomUGCtrl();
		virtual ~CustomUGCtrl();

	public:
		void					SetCallback(CustomUGCtrlCallbackPtr cb) { m_callback = std::move(cb); }
		CustomUGCtrlCallback*	GetCallback() const { return m_callback.get(); }

	private:
		CustomUGCtrlCallbackPtr m_callback;

	protected:
		DECLARE_MESSAGE_MAP()

		virtual void OnSetup();
		virtual void OnLClicked(int col, long row, int updn, RECT* rect, POINT* pt, int processed) override;
		virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed) override;

		virtual DROPEFFECT OnDragDrop(COleDataObject* data, int col, long row) override;
		virtual DROPEFFECT OnDragEnter(COleDataObject* data) override;
		virtual DROPEFFECT OnDragOver(COleDataObject* data, int col, long row) override;

		virtual void OnMenuCommand(int col, long row, int section, int item) override;
		virtual int	OnMenuStart(int col, long row, int section) override;

		virtual COLORREF OnGetDefBackColor(int section);
		virtual void OnDrawFocusRect(CDC *dc, RECT *rect);

		virtual void OnSelectionChanged(int startCol, long startRow, int endCol, long endRow, int blockNum) override;

		virtual void OnKeyDown(UINT *vcKey, BOOL processed) override;
		virtual void OnCharDown(UINT *vcKey, BOOL processed) override;
		virtual void OnCellChange(int oldcol, int newcol, long oldrow, long newrow);

		virtual int OnEditStart(int col, long row, CWnd **edit) override;
		virtual int OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey) override;
		virtual int OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag) override;
		virtual int OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow) override;

		virtual int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	};
}