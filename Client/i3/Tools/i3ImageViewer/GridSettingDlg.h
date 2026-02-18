#pragma once


// CGridSettingDlg dialog

class CGridSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CGridSettingDlg)

public:
	CGridSettingDlg(CWnd* pParent = NULL, UINT32 VertBlockCount = 1, UINT32 HoriCount = 1, INT32 PenIndex = BLACK_PEN);   // standard constructor
	virtual ~CGridSettingDlg();

// Dialog Data
	enum { IDD = IDD_GRID_SETTING_DIALOG };

public:
	UINT32	GetHoriCount()		{ return m_HoriBlockCount; }
	UINT32	GetVertCount()		{ return m_VertBlockCount; }
	INT32	GetPenIndex()		{ return m_PenIndex; }

private:
	UINT m_VertBlockCount;
	UINT m_HoriBlockCount;
	INT32 m_PenIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
};
