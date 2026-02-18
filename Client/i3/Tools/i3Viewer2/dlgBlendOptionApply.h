#include "resource.h"

class dlgBlendOptionApply : public CDialog
{

protected :
	virtual void DoDataExchange(CDataExchange* pDX) override;

	CComboBox 	m_pBlendTypeCtrl;
	CButton 	m_pTwoSideCtrl;
	CButton 	m_pDepthWriteCtrl;
	CEdit 		m_pRefValCtrl;

	i3Node *	m_pRoot;

	void		SetNoBlend();
	void		SetAlphaTest(UINT8 val);
	void		SetBlendModulate(UINT8 val);
	void		SetBlendAdd(UINT8 val);
	void		SetFaceCull(BOOL isEnableFaceCull);
	void		SetZWrite(BOOL isEnableZWrite);
public:

	enum { IDD = IDD_BLEND_OPTION_APPLY };

	dlgBlendOptionApply();
	virtual ~dlgBlendOptionApply();

	virtual BOOL	OnInitDialog() override;
	virtual void	OnOK() override;

	void			Apply();
	void			Execute(i3Node * pNode);

	DECLARE_MESSAGE_MAP()
};