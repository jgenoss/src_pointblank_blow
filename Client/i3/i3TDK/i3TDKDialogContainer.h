#pragma once
#include "afxcmn.h"
#include "i3TDKDialogBase.h"
#include "i3TDKUpdateSystem.h"

// i3TDKDialogContainer dialog

struct I3TDK_SPECDLG_INFO
{
	bool				m_bInternal = false;
	i3ClassMeta *		m_pMeta = nullptr;
	i3GameObjPropertyDialog *	m_pDlg = nullptr;
	UINT32				m_PopupMask = 0;
	INT32				m_DefWidth = 0;
	INT32				m_DefHeight = 0;
} ;

namespace i3 { template<> struct less<I3TDK_SPECDLG_INFO*> : less_ptr<I3TDK_SPECDLG_INFO*> {}; }

bool	operator<( const I3TDK_SPECDLG_INFO& p1, const I3TDK_SPECDLG_INFO& p2);


class I3_EXPORT_TDK i3TDKDialogContainer : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKDialogContainer)

protected:
	i3::vector_multiset< I3TDK_SPECDLG_INFO*>			m_List;
	i3GameObjPropertyDialog *	m_pCurDlg = nullptr;

	i3::vector<i3GameObjPropertyDialog*>	m_PopList;

	INT32				m_Width = 0;
	INT32				m_Height = 0;

	CButton *			m_pBTN_Back = nullptr;

protected:
	void				OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

	bool				_isDescendantWindow( HWND hwnd);
	void				_PopSpecDialog(void);

	void				_UpdateBackButton(void);

public:
	i3TDKDialogContainer(UINT id, CWnd* pParent = nullptr) : i3TDKDialogBase(id, pParent) {}   // standard constructor
	virtual ~i3TDKDialogContainer();

	void	AddSpecDialog( i3ClassMeta * pMeta, i3GameObjPropertyDialog * pSpecDlg, UINT32 popupMask = I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, bool bInternal = false);
	void	RemoveAllSpecDialog(void);
	void	PopupDialog( i3ElementBase * pObj, bool bPush);

	I3TDK_SPECDLG_INFO *	GetSpecDialog( INT32 idx)			{ return m_List.get_vector()[idx]; }

	virtual INT32			FindSpecDialog( i3ClassMeta * pMeta, UINT32 mask);
	virtual i3ClassMeta *	OnQuerySourceObject( i3ElementBase * pElm);

protected:

	DECLARE_MESSAGE_MAP()

	void OnSize(UINT nType, int cx, int cy);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnInitDialog();
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
