
// NationEditorDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "resource.h"

namespace Spread{

typedef void (*Proc)(i3RegKey * pKey, size_t Index1, size_t Index2);
typedef void (*Proc2)(i3RegKey * pKey, void* pData);
typedef void (*Proc3)(i3RegKey * pKey, void* pData, size_t Index1, size_t Index2);


void AddReg(i3RegKey * pKey, size_t Index1, size_t Index2);
void RemoveReg(i3RegKey * pKey, size_t Index1, size_t Index2);
void SwapReg(i3RegKey * pKey, void* pData);
void CloneReg(i3RegKey * pKey, void* pData, size_t Index1, size_t Index2);
};

// CNationEditorDlg 대화 상자
class CNationEditorDlg : public CDialog
{
// 생성입니다.
public:
	CNationEditorDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NATIONEDITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	CHListBox m_ListBox_Nation;
	CHListBox m_ListBox_Pef;
	//CListSubCtrl_Impl m_ListCtrl_Operation;
	CListSubCtrl_Impl m_OperationCtrl;

	CButton m_CheckCtrl_BakFile;

	PefContextBank m_PefContextBank;
	StringVector m_NationBank;

	bool m_bControlKeyDown;
	bool m_bCreateBakFile;
	CString m_CurrentPath;
	CString m_CurrentFilePath;

	enum
	{
		NL_RANGE = 10,
		NL_WIDTH = 210,
	};

public:
	bool IsMultiPef(i3RegKey * pRoot) const;
	void AddPefContext( const String & Name, const String & Path, 
		i3RegistrySet_Impl * pRegistrySet);

	const PefContextBank* GetPefContextBankPtr() const {	return &m_PefContextBank;	}
	const StringVector* GetNationListPtr()  {	return &m_NationBank;	}

	CHListBox* GetNationCtrl()	{	return &m_ListBox_Nation;	}
	CHListBox* GetPefCtrl()	{	return &m_ListBox_Pef;	}

protected:
	void ClearPefContextBank();

	void _SwapNationList(const OrderList* pOrderList);
	void _SwapPef(const OrderList* pOrderList);

protected:
	bool LoadList(const CString & FullPath);

	void LoadNationList();
	void SaveNationList();

	void LoadPefList();
	void SavePef(PefContext* pPefContex);
	void SavePefList();

	bool IsDuplicateString(CHListBox & ListCtrl, const CString & String);

	void AddRegNation(const CString & AddNationName);
	void DelRegNation(const CString & DelNationName);
	void CloneRegNation(size_t pefInx, StringVector* pList, int srcInx, int destInx);

	void AddListNationString(const CString & Nation);
	void DelListNationString(const CString & Nation);

	//m_ListBox_Nation에서 찾는다.
	size_t GetNationIndex(const String & Nation) const;
	size_t GetOperationNationIndex(size_t Index);

private:
	int m_ClonePefIndex;
	int m_CloneSrcIndex;
	int m_CloneDestIndex;
	StringVector m_CloneList;

public:
	void SetCloneData(int pefInx, int srcInx, int destInx, const StringVector& List);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnSelchangeListNation();

	virtual BOOL DestroyWindow() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnBnClickedCheckBakFile();
	afx_msg void OnDropFiles(HDROP hDropInfo);

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};