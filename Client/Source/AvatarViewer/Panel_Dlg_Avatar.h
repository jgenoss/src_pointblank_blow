#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "Avatar.h"

#include <map>

// CPanel_Dlg_Avatar dialog

class CPanel_Dlg_Avatar : public CDialog
{
	DECLARE_DYNAMIC(CPanel_Dlg_Avatar)

protected:
	HTREEITEM		m_hRoots[ EQUIP::ePART_COUNT];
	
	mapParts		m_PartPath[ EQUIP::ePART_COUNT];

protected:
	void			ReleasePartsList( void);

	bool			UpdatePart( Avatar * pAvatar, EQUIP::ePART part, INT32 lod,vPartsList &partlist);

	static void		Log( EQUIP::ePART part, INT32 lod, void * pData, const char * pszMsg);

public:
	CPanel_Dlg_Avatar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanel_Dlg_Avatar();

// Dialog Data
	enum { IDD = IDD_PANEL_AVATAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CTreeCtrl m_TreeCtrl;

private:
	void			_InsertPartsList( HTREEITEM hRoot, const char * filter);

public:
	virtual BOOL OnInitDialog();

	void			UpdateAll( Avatar * pAvatar);
	void			CreatePartsList( void);

	const char *	FindFullPathByName( EQUIP::ePART part, const char * pszName);

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMDBClickPartTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickPartTree(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
