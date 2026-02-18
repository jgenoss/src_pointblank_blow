#if !defined( __PANEL_DASHBOARD_H)
#define __PANEL_DASHBOARD_H

#include "Dlg_MapName.h"

class CStageMapEditor;

class CPanel_Dashboard : public i3GameNode
{
	I3_CLASS_DEFINE( CPanel_Dashboard);
protected:
	CStageMapEditor *	m_pStage;
	i3UIFrameWnd *			m_pWnd;

	i3UIButton *			m_pBTN_Save;
	i3UIButton *			m_pBTN_Register;
	i3UIButton *			m_pBTN_MapSetting;
	i3UIButton *			m_pBTN_Test;
	i3UIButton *			m_pBTN_Undo;
	i3UIButton *			m_pBTN_Delete;
	i3UIButton *			m_pBTN_Copy;
	i3UIButton *			m_pBTN_Cut;
	i3UIButton *			m_pBTN_Paste;
	i3UIButton *			m_pBTN_Exit;

	CDlgMapName				m_DlgMapName_Driver;
public:
	CPanel_Dashboard(void);
	virtual ~CPanel_Dashboard(void);

	void			Create( CStageMapEditor * pStage);

	void			PromptSave(void);
	void			Save(void);

	virtual BOOL	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
};

#endif