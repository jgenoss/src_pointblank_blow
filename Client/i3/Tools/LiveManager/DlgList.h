#pragma once

class CMediaPathDlg : public CDialog
{
	CString strV1Path;
	CString strV2Path;
	bool bState;

public:
	CMediaPathDlg();
	void LoadV1Media();
	void LoadV2Media();

	CString GetV1Path(){ return strV1Path; }
	CString GetV2Path(){ return strV2Path; }
	bool GetState(){ return bState; }

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_PEFPATH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnOK() override;
};

