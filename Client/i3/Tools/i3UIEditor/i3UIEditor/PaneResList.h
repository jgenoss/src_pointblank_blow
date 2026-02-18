#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "i3UIEditorDef.h"
#include "Resource.h"
#include "i3UIControlTemplate.h"
// CPaneTextureList dialog

class CPaneResList : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneResList)

public:
	CPaneResList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneResList();

// Dialog Data
	enum { IDD = IDD_PANE_TEXTURELIST };

protected:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	
	i3TDKTabCtrl	m_ResTabCtrl;

	CListCtrl		m_TextureListView;
//	CListCtrl		m_SceneGraphListView;
//	CListCtrl		m_AnimationListView;
//	CListCtrl		m_SoundListView;
	CListCtrl		m_TemplateListView;

	int m_iCountSelectedItem;
	int* m_pSelectedItemIdxList;

protected:
	i3List				 *	m_pTextureList;
	i3UIControlTemplate *	m_pTemplate;


public:
	i3List	*	getTextureList() { return m_pTextureList;}
	i3UIControlTemplate * getTemplate(){	return m_pTemplate;}

	void		SetTextureList(i3List * pList);
	void		SetControlTemplate( i3UIControlTemplate * pTemplate);

	void		_SelectTemplateFromDialog(i3GameObjBase * pObj);
	void		_SelectTextureFromDialog(i3UIImage * pImage);

	i3GuiToolImageElement *	FindElementByTexture( i3Texture * pTexture);
public:
	//
	void		_TrackPopupMenu( POSITION pos, i3ClassMeta * pMeta);

	//RES
	void		_DeleteRes( INT32 FirstIdx, INT32 idx,  i3ClassMeta * pMeta);
	
	//Texture
	void		AddTexture( i3Texture * pTexture );
	void		SetTexture( i3ElementBase * pData);
	void		ImportTexture();
	void		Loadi3i(const char * pszFileName); 
	BOOL		_CheckFileName( const char * pszFileName);

	//Template
	bool		AddTemplate( i3UITemplate * pElement );
	void		AddNewTemplate();

	void		ExportTemplate();
	void		ImportTemplate();

	BOOL		_IsValidTemplateElement( i3UITemplate * pElement);
	
public:
	afx_msg void OnNMClickTextureList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTemplateList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();

	////////

	
protected:
	void	_ResetContext();
public:
	afx_msg void OnNMRclickReslistTemplate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickReslistTexture(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditReslistTemplate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditReslistTexture(NMHDR *pNMHDR, LRESULT *pResult);
};
