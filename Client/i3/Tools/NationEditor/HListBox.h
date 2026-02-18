
// SolFS_TestDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"

#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

class CHListBox : public CListBox
{
//	DECLARE_DYNAMIC(CHListBox)

public:
	CHListBox() : m_bEnableSwap(false) {}
	virtual ~CHListBox() {}

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

protected:
	void updateWidth(LPCTSTR s);
	int m_Width;
	bool m_bEnableSwap;

	void _UpString();
	void _DownString();

	OrderList m_OrderList;

public:
    void ResetContent();

	int AddString(LPCTSTR s);
	int AppendString(LPCTSTR s);
    int InsertString(int i, LPCTSTR s);

	//해당 리스트에 위치(커서)를 얻어온다.
	int GetCurrentSel() const;

	//해당 위치에 커서를 놓는다.
	void SetCurrentSel(int i);

	//특정 위치 삭제
	int DeleteString(int i);

	//해당 리스트에 위치(커서)의 값을 얻어온다.
	CString GetCurrentString();
	CString GetCurrentString(int nIndex);

	//현재 아이템 하나 삭제
	void DeleteCurrentItem();

	StringVector GetAllStrings() const;

	void SetupOrderList();
	const OrderList* GetOrderList() const	{	return &m_OrderList;	}

	void EnableSwap()	{	m_bEnableSwap = true;	}
	void DisableSwap()	{	m_bEnableSwap = false;	}
};