#pragma once

#include "stdafx.h"
#include "afxcmn.h"

class CListSubEdit;
class CListSubComboBox;
class CListSubDialog;

#define DEFAULT_COLUMNSIZE 100

//ListControl에 기본 바탕에. RegKeyControl을 추가한 것.
class CListSubCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CListSubCtrl)

////////////////////////////////////////////////////////
//CListCtrl

public :
	enum ActionType
	{
		ACT_COPYCELL,
		ACT_CHANGEKEY,
	};

protected:
	CFont m_Font;
	CString m_Name;

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
		CellSubType Cst = CST_NONE;
		bool Show = false;
	};

	std::vector< ColumnContext* > mColumnContexts;
	std::vector<int> mColumnSize;
	CStringList m_ComboStringList;

	CWnd* mSubCtrl;

	std::vector< void* > m_DataPtr;

	int m_CurrentItem, m_CurrentSubItem;
	bool m_bFocusDraw, m_bFocusDrawLine, m_bIntenalFocusDraw;

	struct CustomDrawContext
	{
		int nItem = 0;
		int nSubItem = 0;
		COLORREF cText;
		COLORREF cTextBk;
	};
	std::vector< CustomDrawContext > m_CustomDrawBank[DAT_TYPE_MAX];

	// Color Text
	void	_SetCellColor(LVITEM& lvItem);
	std::vector<INT32> different_cell_list;
protected:
	void Clear_CustomDraw(AnalysisType atype);

	void Clear_AllCustomDraw(void);
	
	void Add_CustomDraw(int nItem, int nSubItem, AnalysisType atype, COLORREF cTextBk, COLORREF cText = RGB(255,255,255));
	void Del_CustomDraw(int nItem, int nSubItem, AnalysisType atype);

	bool Get_CustomDrawCol(int nItem, int nSubItem, COLORREF & ref_cTextBk, COLORREF & ref_cText);

	void Invalidate_CustomDrawCol();

	void _Internal_NotifyHeaderColumn();	//내부에서 사용

	CRect _Internal_GetSubItemRect(int nCol, int nSumItem);


public:
	virtual void NotifyHeaderColumn(ActionType action);

	int	GetLastColumnSize(int nCol);
	void ClearLastColumnSize()			{ mColumnSize.clear(); }

	void Increase_LogFont();
	void Decrease_LogFont();

protected:
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
	void Modify_ColumnText(int nCol, LPCTSTR Title);
	void Modify_ColumnText();

//	CString Get_ColumnText(int nCol);
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

	virtual void Sort_Swap(int low, int high);
	void Modify_UnSortedColumnText();
	void Modify_SortedColumnText(int nCol, bool bAscending);
	
	//마우스 왼쪽 버튼 사용
	virtual void OnLvnCellLClickList(int nItem, int nSubItem, POINT pt)	{}
	//마우스 오른쪽 버튼 사용
	virtual void OnLvnCellRClickList(NMHDR *pNMHDR, LRESULT *pResult) {}

	//정렬
	virtual BOOL Sort_Cell(int nCol, BOOL bAscending, int low = 0, int high = -1);
	virtual bool Sort_Compare(const CString& Str1, const CString& Str2, BOOL bAscending, int Row1, int Row2)	{	return Str1 < Str2;	}

	//SubControl 시작 할 때 행동
	virtual void PreSubEditControl(int Row, int Col, CListSubEdit * pEdit, const CRect& rect, const CString & CellText) {}
	virtual void PreSubComboBoxControl(int Row, int Col, CListSubComboBox * pComboBox, const CRect& rect, const CString & CellText) {}
	virtual void PreSubDialogControl(int Row, int Col, CListSubDialog * pDialog, const CRect& rect, const CString & CellText) {}
	virtual void StartSubControl(int Row, int Col, CellSubType CellType);
	virtual CellSubType PreSubControlType(int Row, int Col)	{	return CST_NONE;	}
	
	//SubControl 완료 되었을 때 행동
	virtual void EndSubControl(int Row, int Col, LPTSTR String);
	virtual bool isSameSubControl(int Row, int Col, LPTSTR String) { return true;}

	//포커스를 잃었을 경우 화면에 표시 유무
	virtual bool PreKillFocus()	{	return false;	}

public:
	void ResetDataPtr();
	void AddDataPtr(void* Data);
	void* GetDataPtr(size_t Index);
	size_t GetDataPtrSize() const;
	void DelDataPtr(size_t Index);


public:
	CListSubCtrl();
	virtual ~CListSubCtrl();
	BOOL Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT ID);
	CString Get_Name() const	{	return m_Name;	}
	void Set_Name( const CString & Name )	{	m_Name = Name;	}

public:
	//엔터키때 나오는 행동
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	//List Control 생성시
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	//Column 왼쪽 버튼 클릭시 나오는 행동
	afx_msg void OnLvnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult);
	//Column 오른쪽 버튼 클릭시 나오는 행동 (PreTranslateMessage 를 이용해서 만든것임)
	virtual void OnLvnColumnRClickList(int nRow, POINT pt) {}

	//Cell 왼쪽 버튼 클릭시 나오는 행동
	afx_msg void OnNMClickList(NMHDR* pNMHDR, LRESULT* pResult);
	//Cell 왼쪽 더블 클릭시 나오는 행동
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	//Cell 오른쪽 클릭시 나오는 행동
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);

	//하위 윈도우 종료시 나오는 행동
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);

	//스크롤 시작시 나오는 행동
	afx_msg void OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult);

	//사용자 정의 Paint
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);

	//Focus 가 사라진다.
	afx_msg void OnNMKillfocus(NMHDR *pNMHDR, LRESULT *pResult);

	//Focus 를 얻는다.
	afx_msg void OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

protected:
	DECLARE_MESSAGE_MAP()
};