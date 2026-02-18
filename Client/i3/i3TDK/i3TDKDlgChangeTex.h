#pragma once

#include "afxwin.h"

#include "i3Base/itl/svector.h"

class ChangeTexPropertyGridCtrl;
class ChangeTexFileProperty;

class i3TDKChangeTexCB
{
public:
	virtual ~i3TDKChangeTexCB() {}
	virtual bool OnSelectedi3sFilename(const i3::string& str_i3s_fullpath) = 0;
	virtual bool OnUnselectedi3sFilename(const i3::string& str_i3s_fullpath) = 0;
};

// 이제 목록에 있는 모든 i3s를 고려해야하기 때문에, ref i3s별로 구조체로 뽑아내는게 좋겠다..
struct i3TDKChangeTex_Pef;
struct i3TDKChangeTex_RefCategory;
struct i3TDKChangeTex_Refi3s;

struct i3TDKChangeTex_TexGroup;			// 미리 선행해서 뽑아놓는게 편할지도 모르겠다...
typedef i3::shared_ptr<i3TDKChangeTex_TexGroup>		i3TDKChangeTex_TexGroupPtr;

class Refi3sListCtrl;
class RefCategoryListCtrl;

class I3_EXPORT_TDK i3TDKDlgChangeTex : public CDialog
{
	DECLARE_DYNAMIC(i3TDKDlgChangeTex)
	friend class ChangeTexPropertyGridCtrl;
	friend class ChangeTexFileProperty;

	friend class ChangeTexPefListCtrl;
	friend class Refi3sListCtrl;
	friend class RefCategoryListCtrl;
public:
	i3TDKDlgChangeTex(CWnd* pParent = nullptr);   // standard constructor
	virtual ~i3TDKDlgChangeTex();

// Dialog Data
	enum { IDD = IDD_TDK_DLG_CHANGETEX };

	void				SetFilenameAndSgInfo(const char* szPath, i3SceneGraphInfo* pSgInfo);		
	void				SetFilenameFromFileDialog(const TCHAR* szPath);

	i3SceneGraphInfo*	GetSgInfo() const			{ return m_pSgInfo; }	

	const CListCtrl*	GetClonedi3sListCtrl() const { return &m_Clonedi3sListCtrl; }				
	
	const i3::vector<i3Body*>& getBodyList() const { return m_bodyList; }

	void				SetCheckLODSeperation(bool b);
	void				EnableLODSeperationCheck(bool b);
	bool				IsCheckedLODSeperation() const;
	bool				IsEnabledLODSeperationCheck() const;

	DWORD				GetMaxLODCount() const { return m_MaxLODCount; }

	void				SetCallback(i3TDKChangeTexCB* cb) 
	{
		delete m_ChangeTexCB;		m_ChangeTexCB = cb;
	}
	
	bool				IsSafeModifyCheckLODSeperation() const;


protected:
	void			Process_Save_All();

	void			Process_ResetSelection_i3sListCtrl();
	void			Process_Selection_i3sListCtrl(int nIdx);
	void			Process_EnableEditProperties(BOOL bEnable);
	void			Process_Selection_Refi3sListCtrl(int nIdx);
	void			Process_Selection_RefCategoryListCtrl(int nIdx);
	void			Process_Selection_ChangeTexPefListCtrl(int nIdx);


	void			DisplayTex_Orig(const TCHAR* szOrigTexName);
	void			DisplayTex_Target(const TCHAR* szTargetTexName);

	virtual void	DoDataExchange(CDataExchange* pDX);		// DDX/DDV support

	void			InitChangeTexPefListCtrl();						// 한번만 실행..
	void			InitRefCategoryi3sList(i3TDKChangeTex_Pef* pef);	// pef별로 최초 1회.실행..
	void			UpdateRefCategoryListCtrl();						// pef 선택때 마다 갱신 실행..

	void			UpdateRefi3sListCtrl();
	void			UpdateClonedListCtrls(INT32 ref_list_idx);   //

	void			BuildProperties();
	void			BuildPropertiesImp_IfLODSeperation();
	void			BuildPropertiesImp_IfNoSeperation();

	void			ClearProperties();

	void			ClearPropertiesValue();
	void			ClearClonedi3sListCtrl();

	void			ReplaceImageCtrl();
	
	void			AddNewRefi3sItem( const TCHAR* szI3SFileName);
	void			UpdateDlgTitle();


	void			AddNewPefFile( const TCHAR* szPefFileName);

	DECLARE_MESSAGE_MAP()

	ChangeTexPefListCtrl*		m_ChangeTexPefListCtrl;
	RefCategoryListCtrl*		m_RefCategoryListCtrl;
	Refi3sListCtrl*				m_Refi3sListCtrl;		// 
	CListCtrl					m_Clonedi3sListCtrl;	//

	ChangeTexPropertyGridCtrl*  m_wndPropList;	//	테스트후 리스트콘트롤을 이것으로 대체..
//


	i3SceneGraphInfo*			m_pSgInfo;				//  이건 하나만 유지한다.
	i3::vector<i3Body*>			m_bodyList;

	UINT32						m_MaxLODCount;			//  매 레퍼런스메시 선택때마다 변경한다..

	i3::vector<i3TDKChangeTex_TexGroupPtr>	m_vecTexGroup_NonSep;
	i3::vector<i3TDKChangeTex_TexGroupPtr>	m_vecTexGroup_NoLOD;
	i3::vector<i3::vector<i3TDKChangeTex_TexGroupPtr> >	m_vecvecTexGroup_LOD;
//

	i3TDKImageViewCtrl			m_pOrigTexCtrl;
	i3TDKImageViewCtrl			m_pTargetTexCtrl;

	
	i3::vector<i3TDKChangeTex_Pef*>			m_pefList;
	INT32									m_curPefIdx;	

//	i3RegKey*					m_pefRootKey;			//	이건 거의 불변...
//	i3::vector<i3TDKChangeTex_RefCategory*>	m_RefCategoryList;	// 
//	INT32									m_oldSelCategoryIndex;

#ifdef _UNICODE
	i3::rc_wstring				m_filename_from_filedlg;
#else
	i3::rc_string				m_filename_from_filedlg;
#endif
	


	i3TDKChangeTexCB*			m_ChangeTexCB;			// new-delete로만 관리한다.


protected:

	virtual BOOL OnInitDialog();	

	afx_msg void OnBnClickedTdkBtnPefPath();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClickClonedi3sList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
public:
	afx_msg void OnNMRClickClonedI3sList(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLvnEndlabeleditClonedI3sList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickRefI3sList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedTdkBtnSavepef();
	afx_msg void OnNMRClickRefI3sList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedCheckChangetexLodSeperation();
	afx_msg void OnBnClickedButtonOpenRefI3s();

	afx_msg void OnNMClickRefCategoryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickChangetexPefList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickChangetexPefList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditChangetexPefList(NMHDR *pNMHDR, LRESULT *pResult);
};
