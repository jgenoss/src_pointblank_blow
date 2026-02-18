#include "resource.h"

class dlgColorOptionApply : public CDialog
{
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;

	i3TDKColorCtrl	m_ColorCtrl;
	i3TDKColorCtrl	m_DiffCtrl;
	i3TDKColorCtrl	m_SpecCtrl;
	i3TDKColorCtrl	m_EmisCtrl;
	CEdit 			m_ShininessCtrl;

	CButton			m_ColorCheck;
	CButton			m_DiffCheck;
	CButton			m_SpecCheck;
	CButton			m_EmisCheck;
	CButton			m_ShineCheck;

	CStatic	m_ST_ColPH;
	CStatic m_ST_DiffPH;
	CStatic m_ST_SpecPH;
	CStatic m_ST_EmisPH;

	i3Node *		m_pRoot;

public:

	enum { IDD = IDD_COLOR_OPTION_APPLY };

	dlgColorOptionApply();
	virtual	~dlgColorOptionApply();

	virtual BOOL	OnInitDialog() override;
	virtual void	OnOK() override;
	virtual void	OnCancel() override;

	void			Execute(i3Node * pRoot);

	void			Apply();	

	afx_msg void OnBnClickedColorCheck();
	afx_msg void OnBnClickedDiffCheck();
	afx_msg void OnBnClickedSpecCheck();
	afx_msg void OnBnClickedEmisCheck();
	afx_msg void OnBnClickedShineCheck();
	afx_msg void OnKillFocusShininess();

	DECLARE_MESSAGE_MAP();

};