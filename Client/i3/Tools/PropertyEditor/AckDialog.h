#pragma once
#include "afxwin.h"

// 응용 프로그램 정보에 사용되는 C_ACK_DLG 대화 상자입니다.

class CAckDlg : public CDialog
{
public:
	CAckDlg(i3::string ack);

	// 대화 상자 데이터
	enum { IDD = IDD_ACK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 지원

	// 구현
protected:
	DECLARE_MESSAGE_MAP()

private:
	i3::string m_sAck;

public:
	virtual BOOL OnInitDialog() override;
};

//================================================================
