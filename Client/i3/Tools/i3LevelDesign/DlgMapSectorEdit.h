#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgMapSectorEdit dialog

class CDlgMapSectorEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgMapSectorEdit)

protected:
	i3MapSectorTable *			m_pTable;
	INT32						m_idxSelect;

	//Volatile, for UndoInfo
	i3LevelWorld		*		m_pWorld;
	i3LevelResWorld *			m_pRes;
	bool						m_bEditted;

	CImageList					m_ImageList_Checkbox;

	i3::vector< i3LevelResEvent *>	m_ResList;

protected:
	

public:
	CDlgMapSectorEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMapSectorEdit();

// Dialog Data
	enum { IDD = IDD_DLG_MAPSECTION };

public:
	static bool Execute( i3LevelWorld * pWorld, i3LevelResWorld * pRes, i3MapSectorTable * pTable);
	static void	Destroy(void);

protected:
	INT32		_getSelectedSector(void);
	void		_AssignValue(void);
	void		_RemoveAllSector(void);

	void		_OnNewSector(void);
	void		_OnDeleteSector(void);

	void		_UpdateAllSector(void);
	void		_UpdateSector( INT32 idx, i3::pack::MAP_SECTOR_INFO * pInfo);
	void		_UpdateSector( INT32 idx);
	void		_SelectSector( INT32 idx);

	void		UpdateAllTrigger(void);
	void		RefreshTriggerImage(void);
	void		UpdateTrigger( INT32 idx, i3LevelPrimitive * pTrigger);
	void		GetAllTrigger( i3::vector< i3LevelPrimitive *> & elmList);
	void		FindAllRelatedElement( INT32 idxMapSector, i3::vector< i3LevelPrimitive *> & elmList);
	bool		IsActionMapSector( INT32 idxMapSector, i3LevelPrimitive * pPrim);
	bool		IsSameMapSector( INT32 idxMapSector, i3TimeEventGen * pEventGen);
	void		ToggleTrigger( INT32 idx);

	void		SetTrigger_MapSector( i3LevelPrimitive * pPrim, INT32 idxMapSector);
	void		ClearTrigger_MapSector( i3LevelPrimitive * pPrim);

	i3LevelResEvent *	PrepareEventRes(void);
	i3LevelResEvent *	FindEventRes( INT32 idxMap);
	void		ScanEventRes(void);
	void		RemoveAllRes(void);

	static BOOL EventProc( INT32 Level, char * pszPath, WIN32_FIND_DATA * pFileInfo, void * pUserData);

	void		Config( i3LevelWorld * pWorld, i3LevelResWorld * pRes, i3MapSectorTable * pTable);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listMapSector;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRclickListMapsector(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	CEdit m_edSectorName;
	CEdit m_edFloorIndex;
	afx_msg void OnEnChangeEditFloorindex();
	afx_msg void OnEnChangeEditSectorname();
	afx_msg void OnNMClickListMapsector(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnExportsector();
	CListCtrl m_LIST_Trigger;
	afx_msg void OnNMClickListTrigger(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListTrigger(NMHDR *pNMHDR, LRESULT *pResult);
};
