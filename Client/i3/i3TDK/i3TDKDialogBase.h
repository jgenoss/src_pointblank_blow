#pragma once
#include "afxcmn.h"

// i3TDKDialogBase dialog

class I3_EXPORT_TDK i3TDKDialogBase : public i3GameObjPropertyDialog
{
	DECLARE_DYNAMIC(i3TDKDialogBase)

protected:
	void		SubclassEditBox( HWND hwnd);

public:
	i3TDKDialogBase(UINT id, CWnd* pParent = nullptr) : i3GameObjPropertyDialog(id, pParent) {}   // standard constructor
	virtual ~i3TDKDialogBase() {}

	static BOOL CALLBACK	SubclassingControl( HWND hwnd, LPARAM lParam);

protected:

	DECLARE_MESSAGE_MAP()

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnInitDialog();
};
