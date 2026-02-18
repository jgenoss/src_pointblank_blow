#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "i3GuiEditorDef.h"
#include "Resource.h"
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
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	
	i3TDKTabCtrl	m_ResTabCtrl;

	CListCtrl		m_TextureListView;
//	CListCtrl		m_SceneGraphListView;
//	CListCtrl		m_AnimationListView;
//	CListCtrl		m_SoundListView;
	CListCtrl		m_TemplateListView;

	int m_iCountSelectedItem;
	int* m_pSelectedItemIdxList;

protected:
	i3::shared_ptr<	i3::vector<i3GuiToolImageElement*> >	m_pTextureList;
	i3GuiControlTemplate *	m_pTemplate;

public:
	i3::shared_ptr< i3::vector<i3GuiToolImageElement*> >&	getTextureList() { return m_pTextureList;}
	i3GuiControlTemplate * getTemplate(){	return m_pTemplate;}

	void		SetTextureList(const i3::shared_ptr<i3::vector<i3GuiToolImageElement*> >& pList);
	void		SetControlTemplate( i3GuiControlTemplate * pTemplate);

	void		_SetFixedCountShapeType( BOOL bTrue, INT32 pos);

	void		_SelectTemplateFromDialog(i3GuiObjBase * pObj);
	void		_SelectTextureFromDialog(i3GuiImage * pImage);

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
	bool		AddTemplate( i3GuiTemplateElement * pElement );
	void		AddNewTemplate();

	void		ExportTemplate();
	void		ImportTemplate();

	BOOL		_IsValidTemplateElement( i3GuiTemplateElement * pElement);
	
public:
	afx_msg void OnNMClickTextureList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTemplateList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog() override;

	////////

	
protected:
	void	_ResetContext();
public:
	afx_msg void OnNMRclickReslistTemplate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickReslistTexture(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditReslistTemplate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditReslistTexture(NMHDR *pNMHDR, LRESULT *pResult);
};
