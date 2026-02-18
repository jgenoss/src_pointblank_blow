#if !defined( __PICK_TARGET_H)
#define __PICK_TARGET_H

class PickTarget : 
      public PickModeCallback,
      public PickNodeCallback 
{
protected:
	BOOL		ValidTarget( INode * pNode, bool bErrorMsg = false);

	void		SetProxy( INode * pPicked);

public:     
	i3::vector<INode*>	m_SelNodeList;
	HWND		m_hwndDlg;
	HWND		m_hwndBtn;

	PickTarget(void);

	void		Reset(void);

	virtual BOOL HitTest( IObjParam *ip, HWND hWnd, ViewExp *vpt, IPoint2 m, int flags) override;
	virtual BOOL Pick( IObjParam *ip,ViewExp *vpt) override;

	virtual void EnterMode(IObjParam *ip) override;
	virtual void ExitMode(IObjParam *ip) override;

	virtual BOOL RightClick(IObjParam *ip,ViewExp *vpt) override { return TRUE; }

	virtual BOOL Filter(INode *node) override;

	virtual PickNodeCallback *GetFilter() override {	return this;}
	virtual BOOL AllowMultiSelect() override { return FALSE;}
};

#endif
