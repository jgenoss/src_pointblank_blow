#pragma once
#include "afxwin.h"


// CDlgAddAIStateToChilds dialog

class CDlgAddAIStateToChilds : public CDialog
{
	DECLARE_DYNAMIC(CDlgAddAIStateToChilds)

protected:
	i3ElementBase *		m_pObj;

public:
	CDlgAddAIStateToChilds(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAddAIStateToChilds();

	void		setObj( i3ElementBase * pObj)		{ m_pObj = pObj; }
	void		ProcessAddAIState( void);

	void		Rec_FindLastChild( i3GameNode * pNode);

	void		AddAIState( i3GameNode * pNode);
	void		UpdateAnimComboList();

	void		RecalcPos( GICDiagramAI * pDiagram, i3GICShape * pShape);

	void		RecalcPosToAuto(GICDiagramAI * pDiagram, i3GICShape * pShape, REAL32 rMaxX, REAL32 rMinX);
	void		RecalcPosToBottomOfLastNode(GICDiagramAI * pDiagram, i3GICShape * pShape);
	void		ReCalcPosToRightOfLastNode(GICDiagramAI * pDiagram, i3GICShape * pShape);

	i3Animation* FindSelectAIState(GICDiagramAI * pDiagram);
// Dialog Data
	enum { IDD = IDD_DLG_ADDAISTATE_CHILDS };

	// 노드 추가 위치
	enum ADD_POSITION		
	{
		ADD_POS_AUTO = 0,	// 자동으로 추가
		ADD_POS_BOTTOM,		// Last Node 하단에 추가
		ADD_POS_RIGHT,		// Last Node 우측에 추가
	};

protected:
	ADD_POSITION		m_eAddPos;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Ed_AIStateNameCtrl;
	CComboBox m_Cmb_AnimPathCtrl;
	CEdit m_Ed_BlendTimeCtrl;
	CEdit m_Ed_AnimFilterCtrl;
	CButton m_Chk_AnimLoopCtrl;
	afx_msg void OnEnKillfocusEditAddchildsFilter();

	//* Add Position Radio Button *//
	CButton m_Radio_PosAuto;
	afx_msg void OnBnClickedRadioAddchildsPosAuto();
	afx_msg void OnBnClickedRadioAddchildsPosBottom();
	afx_msg void OnBnClickedRadioAddchildsPosRight();

	CButton m_Chk_UseOwnPath;
	CEdit m_Ed_BasePathNodeName;
	afx_msg void OnBnClickedChkUseOwnPath();
};
