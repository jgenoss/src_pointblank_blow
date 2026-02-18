#pragma once

#include "afxcmn.h"

class CListSubEdit;
class CListSubComboBox;
class CListSubDialog;

//ListControl에 기본 바탕에. RegKeyControl을 추가한 것.
class CListSubCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CListSubCtrl)

////////////////////////////////////////////////////////
//CListCtrl
protected:
	CFont m_Font;

	enum CellSubType
	{
		CST_NONE,
		CST_EDIT,
		CST_COBMO,
		CST_DIALOG,
	};

	struct ColumnContext
	{
		TCHAR Title[MAX_PATH] = { 0 };
		bool SortAscend = false;
		CellSubType Cst;
		bool Show = false;
	};

	std::vector< ColumnContext* > mColumnContexts;
	CStringList m_ComboStringList;

	CWnd* mSubCtrl;

	std::vector< void* > m_DataPtr;

	int m_CurrentItem, m_CurrentSubItem;

public:
	virtual void NotifyHeaderColumn();

	void ResetComboString();
	void AddComboString( const CString & String );

protected:
	void Increase_LogFont();
	void Decrease_LogFont();
	void Set_LogFont( const LOGFONT * pLogFont);

	void Clear_Cells();

	void Clear_Columns();
	void Erase_Column(LPCTSTR Name);
	void Hide_Column(LPCTSTR Name);
	void Hide_AllColumns();
	virtual void Notify_Hide_Column(LPCTSTR Name) {}


	void Show_Column(LPCTSTR Name);
	void Show_AllColumns();
	virtual void Notify_Show_Column(LPCTSTR Name) {}

	int Get_ColumnIndex(LPCTSTR Name);
	int Get_ColumnContextIndex(LPCTSTR Name);

	void Set_Column(LPTSTR Title, int Length, CellSubType CellTyte = CST_EDIT,
		UINT Mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM, 
		UINT Format = LVCFMT_LEFT);


	CString Get_ColumnText(int nCol);
	CString Get_ColumnContextText(int nCol);

	void Insert_Cell(int nRow, LPTSTR Value);
	bool Set_Cell(int nRow, int nCol, LPTSTR Value);

	//Column 수를 알려준다. 가로 수
	int Get_ColumnCount() const;
	int Get_ColumnContextIndex(int nCol);
	int Get_ColumnContextCount() const;

	//Cell에 수를 알려준다. 세로 수
	int Get_CellCount() const;

	//해당 위치에 값을 얻어 온다.
	//Row : 가로 위치(0부터 시작)
	//Col : 세로 위치(0부터 시작)
	CString Get_CellText(int nRow, int nCol);

	int Is_HeaderClicked(POINT pt);
	bool Is_HeaderShow(int nCol);

	//SubControl 시작 할 때 행동
	virtual void PreSubEditControl(int Row, int Col, CListSubEdit * pEdit, const CRect& rect, const CString & CellText);
	virtual void PreSubComboBoxControl(int Row, int Col, CListSubComboBox * pComboBox, const CRect& rect, const CString & CellText);
	virtual void PreSubDialogControl(int Row, int Col, CListSubDialog * pDialog, const CRect& rect, const CString & CellText);
	virtual void StartSubControl(int Row, int Col, CellSubType CellType);
	virtual CellSubType PreSubControlType(int Row, int Col);
	
	//SubControl 완료 되었을 때 행동
	virtual void EndSubControl(int nRow, int nCol, LPTSTR String);


public:
	CListSubCtrl();
	virtual ~CListSubCtrl();
	BOOL Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT ID);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	//Cell 하위 클릭시 나오는 행동
	afx_msg void OnNMClickList(NMHDR* pNMHDR, LRESULT* pResult);

	//하위 윈도우 종료시 나오는 행동
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);

	//스크롤 시작시 나오는 행동
	afx_msg void OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult);

	//엔터키때 나오는 행동
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	//사용자 정의 Paint
//	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnLvnHotTrack(NMHDR *pNMHDR, LRESULT *pResult);
};


