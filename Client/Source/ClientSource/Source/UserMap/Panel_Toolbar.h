#if !defined( __PANEL_TOOLBAR_H)
#define __PANEL_TOOLBAR_H

class CStageMapEditor;

class CPanel_Toolbar : public i3GameNode
{
	I3_CLASS_DEFINE( CPanel_Toolbar);
protected:
	CStageMapEditor *	m_pStage;
	
	i3UIFrameWnd *			m_pWnd;
	i3UIButton *			m_pBTN_Select;
	i3UIButton *			m_pBTN_Move;
	i3UIButton *			m_pBTN_Rotate;
	i3UIButton *			m_pBTN_Scale;
	i3UIButton *			m_pBTN_Blocks;

public:
	CPanel_Toolbar(void);
	virtual ~CPanel_Toolbar(void);

	void			Create( CStageMapEditor * pStage);

	virtual BOOL	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
};

#endif