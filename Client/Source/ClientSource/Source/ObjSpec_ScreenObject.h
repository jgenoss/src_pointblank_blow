#if !defined( _OBJSPEC_SCREENOBJECT_H__)
#define _OBJSPEC_SCREENOBJECT_H__

#if defined(USE_EDITDLG)// CObjSpec_ScreenObject dialog

#include "afxwin.h"
#include "afxcmn.h"
#include "i3Framework.h"
#include "resource.h"
#include "i3TDKUpdateSystem.h"


class CObjSpec_ScreenObject : public i3GameObjPropertyDialog
{
	DECLARE_DYNAMIC(CObjSpec_ScreenObject)

public:
	CObjSpec_ScreenObject(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CObjSpec_ScreenObject();

// Dialog Data
	enum { IDD = IDD_PB_SPEC_SCREENOBJECT };

	virtual void	SetObject( i3ElementBase * pObj) override;
	
	void			OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	void			_UpdateObj(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;

	void	_EndSelectTarget(void);

public:
	CEdit			m_edSelectCam;
	CButton			m_BtnSelectCam;

	virtual BOOL	OnInitDialog() override;
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg	void	OnBnClickedSelectCam();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
};
#endif	
#endif	//#define _OBJSPEC_SCREENOBJECT_H__