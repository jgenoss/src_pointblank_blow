
#ifndef _DLGNOGADAMASK_H_
#define _DLGNOGADAMASK_H_

#ifdef _USE_TOOLAM_

#include "afxwin.h"
#include "resource.h"

#include "ToolAM.h"
#include "ToolAM_Def_CharTool.h"
#include "ctl/MCtlFileDropTarget.h"


class CDlgNogadaMask : public CDialog,
	public ToolAM::MCtlFileDropTarget::CallBack,
	public ToolAM::CmdQueue::ReportCallBack
{
	DECLARE_DYNAMIC(CDlgNogadaMask)

public:
		// ToolAM::MCtlFileDropTarget::CallBack
	virtual void MCtlFileDropTarget_Reset(void* arg=NULL);	
	virtual bool MCtlFileDropTarget_OnIterateDropFiles(void* arg, INT32 totalCnt, 
		INT32 cur, bool isDirectory, const char* path); // ret false: finish, true:continue
	virtual void MCtlFileDropTarget_OnIterateAfter(void* arg, INT32 num);

		// ToolAM::CmdQueue::ReportCallBack
	virtual void ToolAM_CmdQueue_Report(DWORD errFlag, ToolAM::CmdQueue::Data& cmdData,
		const char* message);
	virtual void ToolAM_CmdQueue_Report2(const char* message);

public:
	CDlgNogadaMask(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNogadaMask();

	enum { IDD = IDD_NOGADA_MASK };

	const char* GetWorkDir();
	void GetCheckedHeroNames(std::vector<std::string>& heroNames);

	bool ProcessDropFileForMask(INT32 totalCnt, INT32 cur, bool isDir, const char* path);

public:
	char							m_ownerWorkDir[MAX_PATH];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
//	virtual void OnOK();
	virtual void PostNcDestroy();

private:
	CEdit							m_ebWorkDir;							
	
	// hero to operate
	CButton							m_ckOperHero[ToolAM::ECHR_HERO_MAX];
	ToolAM::MCtlFileDropTarget*		m_maskDropTarget;

	CListBox						m_log;

	// Afx Message Handlers
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnAutoGenMaskChr();
	afx_msg void OnClickedOk();
	afx_msg void OnDropFiles(HDROP hDropInfo);

	afx_msg void OnBtn_HeroOper_AllCheck();
	afx_msg void OnBtn_HeroOper_AllUnCheck();

	afx_msg void OnBtn_MaskPath_Del();
	afx_msg void OnBtn_MaskPath_Reset();
	afx_msg void OnBtn_Mask_LodFileNameEval();
	afx_msg void OnBtn_Mask_LodFileNamePatternPreset();

	afx_msg void OnRButtonUp(UINT vKeyStatus, CPoint pos);
public :
	//void GenerateMask(char* pchDirName);
	
};


#endif // end of _USE_TOOLAM_

#endif
