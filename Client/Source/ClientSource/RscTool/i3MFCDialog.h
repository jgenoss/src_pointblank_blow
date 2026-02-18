#pragma once

class i3MFCDialog;

struct i3MFCDialogCallback 
{
	virtual int OnCreate(i3MFCDialog* dlg, LPCREATESTRUCT lpCreateStruct) { return 0; }
	virtual void OnSize(i3MFCDialog* dlg, int cx, int cy) {}
	virtual BOOL OnClose() { return TRUE; }
	virtual ~i3MFCDialogCallback() {}
};



class i3MFCDialog : public CDialog
{
	DECLARE_DYNAMIC(i3MFCDialog)
public:
	i3MFCDialog();
	~i3MFCDialog();

	BOOL CreateEx(CWnd* pParent, const CRect& rc);
	
	void SetCallback(i3MFCDialogCallback* cb) { m_callback = cb; }
	i3MFCDialogCallback* GetCallback() const { return m_callback; }
	
	void SetRect(const CRect& rc) { m_rect = rc; }
	virtual INT_PTR DoModal();

private:
	int CreateImpl(CWnd* pParent, const CRect& rc, BOOL modal);
	i3MFCDialogCallback*	m_callback;
	CRect					m_rect;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:

	afx_msg void OnClose();
};
