#pragma once
#include "afxcmn.h"
#include "i3TDKDialogBase.h"
#include "i3TDKUpdateSystem.h"
#include "i3TDKWndFoldCtrl.h"
#include "i3TDKDialogContainer.h"

// i3TDKSpecDialogContainer dialog

class I3_EXPORT_TDK i3TDKSpecDialogContainer : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpecDialogContainer)

protected:
	i3::vector<I3TDK_SPECDLG_INFO*>	m_List;
	i3::vector<I3TDK_SPECDLG_INFO*>	m_PopList;

	i3TDKWndFoldCtrl		m_FoldCtrl;

	i3ElementBase *			m_pObj = nullptr;

	INT32				m_Width = 0;
	INT32				m_Height = 0;

protected:
	void				OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	void				_SetPlaceSpecs(void);

public:
	i3TDKSpecDialogContainer(UINT id, CWnd* pParent = nullptr) : i3TDKDialogBase(id, pParent) {}   // standard constructor
	virtual ~i3TDKSpecDialogContainer();

	void	AddSpecDialog( i3ClassMeta * pMeta, i3GameObjPropertyDialog * pSpecDlg, UINT32 popupMask = I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, bool bInternal = false);
	void	RemoveAllSpecDialog(void);
	void	PopupDialog( i3ElementBase * pObj);

	void	AddBuiltInSpecDialogs(void);

	I3TDK_SPECDLG_INFO *	GetSpecDialog( INT32 idx)			{ return m_List[idx]; }

	virtual INT32			FindSpecDialog( i3ClassMeta * pMeta, UINT32 mask, i3::vector<I3TDK_SPECDLG_INFO*>& List);
	virtual i3ClassMeta *	OnQuerySourceObject( i3ElementBase * pElm);

protected:

	DECLARE_MESSAGE_MAP()

	void OnSize(UINT nType, int cx, int cy);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnInitDialog();
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnPaint();
};
