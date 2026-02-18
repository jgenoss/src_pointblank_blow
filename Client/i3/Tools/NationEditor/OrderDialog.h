#pragma once

#include "HListBox.h"
// COrderDialog 대화 상자입니다.
class CNationEditorDlg;

class COrderDialog : public CDialog
{
	DECLARE_DYNAMIC(COrderDialog)

public:
	COrderDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~COrderDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ORDER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CNationEditorDlg* m_pParent;

	CHListBox m_ListBox_Before;
	CHListBox m_ListBox_After;


public:
	virtual BOOL OnInitDialog() override;

	const OrderList* GetOrderList() const	{	return m_ListBox_After.GetOrderList();	}
};
