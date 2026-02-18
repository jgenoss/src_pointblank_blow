#pragma once

#include "afxcmn.h"

#include "ListSubCtrl.h"

class i3TDKRegistryCtrl_Impl;

struct COLUMELIST
{
	char _Name[64] = { 0 };
	bool _State = false;
};

//ListControl에 기본 바탕에. RegKeyControl을 추가한 것.
class i3TDKListCtrl_Impl : public CListSubCtrl
{
protected:
	i3RegKey * m_pKey;

	//파일에서 읽어 들인 나라 리스트
	std::vector< std::string > m_NationList, m_DisplayNationList;

	i3::vector<COLUMELIST*> m_vecColumnList;

	UINT m_PefFileType;
	bool m_bUseChildCellClick;
	bool m_bUseCellRClick;
//	bool m_bUseDefalutNationFile;
	bool m_bAutoSize;

	HACCEL m_hAccel;

public:
	i3TDKListCtrl_Impl();
	virtual ~i3TDKListCtrl_Impl();

	void	SetRegKey( i3RegKey * pKey, ActionType action);
	i3RegKey	*	getRegKey()	{	return m_pKey;}

	//새로운 데이터를 추가한다.
	i3RegData* _AddNewData( i3RegKey* pRegKey, const CString & Name, INT32 nValue );
	i3RegData*	AddNewData( INT32 nType ,INT32 nValue = -1);

	void	AddTimeStampData( i3RegKey* pRegKey, char* pszName, char* pszTime, char* pszTimeString );
	void	UpdateTimeStampData( char* pszName, char* pszTime, char* pszTimeString );
	
	void	DeleteData( INT32 nIdx);
	void	RenameData( INT32 nIdx);
	void	PasteData( i3RegData * pData);

	void UpdateItem( INT32 nRow);

	void	EableAutoSize(bool ) { m_bAutoSize = true; }
	void	DisableAutoSize(bool ) { m_bAutoSize = false; }
	void	ChangeAutoSize(void) { m_bAutoSize = !m_bAutoSize; _UpdateKey();}
	bool	getAutoSize(void) { return m_bAutoSize; }

	// i3TDKRegistryCtrl_Impl::_PasteData() 에서만 호출해야합니다.
	void    _AddPasteRowToCustomDraw(INT32 numRowToAdd);


	bool IsNationFileListEmpty() const { return m_NationList.empty(); }


protected:
	void UpdateRegType(i3RegKey* pRegKey, ActionType action);
	void UpdateRegFileType(ActionType ACTION);
	void _UpdateKey();
	INT32 _AddData( i3RegData * pData);
	INT32 _AddData( i3RegData * pData, char* pszDefault );

	void _Convert2String( char * pszString, size_t sizeOfString, i3RegData * pData);
	void HideColumn(const i3::rc_string& sColume);
	void ShowColumn(const i3::rc_string& sColume);
	void UpdateColumnList();
	void AddColumnStorage(bool bState, const i3::rc_string& sColume);

protected:
	void ColumnRClickMenu(int nRow, POINT pt);
	virtual void OnLvnColumnRClickList(int nRow, POINT pt) override;

	//Cell 왼쪽 버튼 클릭시 나오는 행동
	virtual void OnLvnCellLClickList(int nItem, int nSubItem, POINT pt) override;
	//Cell 오른쪽 클릭시 나오는 행동
	virtual void OnLvnCellRClickList(NMHDR *pNMHDR, LRESULT *pResult) override;

	virtual CellSubType PreSubControlType(int Row, int Col) override;

	virtual void PreSubDialogControl(int Row, int Col, CListSubDialog * pDialog, const CRect& rect, const CString & CellText) override;

	virtual void EndSubControl(int Row, int Col, LPTSTR String) override;

	virtual bool isSameSubControl(int Row, int Col, LPTSTR String) override;

	virtual bool Sort_Compare(const CString& Str1, const CString& Str2, BOOL bAscending, int Row1, int Row2) override;
	virtual void Sort_Swap(int low, int high) override;

	virtual void NotifyHeaderColumn(ActionType action) override;
	

	bool CurrnetPathNationFile();
	bool MediaPathNationFile();
	void LookForPathNationFile();

	void LoadNationFile(const CString & Path);

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;

public:
	void LookForNationFile();

public:
	void Clear_AnalysisItem(AnalysisType atype);
	
	void Add_CompareItem(int nItem, int nSubItem);
	void Add_MatchItem(int nItem, int nSubItem);
	void Add_Modify(int nItem, int nSubItem);

	void AnalysisItem(int nItem, int nSubItem, AnalysisType Type);


public:
	std::vector< std::string >* GetNationListRef() 
	{ 
		static bool triedLoadNif = false;

		if(!triedLoadNif && m_NationList.empty())
		{
			CurrnetPathNationFile();	
			triedLoadNif = true;
		}

		return &m_NationList;	
	}

	std::vector< std::string >& GetDisplayListRef();

	UINT GetPefFileType() const;
	void SetSingleNation();
	void SetMultiNation(ActionType action);

	void NotifyRegFileNation();

	static bool IsStampText(const CString& Str);

	void SetUseCellLClickList(bool UseCellLClick)	{	m_bUseChildCellClick = UseCellLClick;	}

	virtual bool PreKillFocus();

	CString GetNewDataName();

	UINT GetAbleWeaponNumber();
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OpenWidthDialog(WPARAM wParam, LPARAM lParam);

};


