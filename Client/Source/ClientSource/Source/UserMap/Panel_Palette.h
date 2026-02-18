#if !defined( __PANEL_PALETTE_H)
#define __PANEL_PALETTE_H

class CStageMapEditor;

class CPanel_Palette : public i3GameNode
{
	I3_CLASS_DEFINE( CPanel_Palette);
protected:
	CStageMapEditor *	m_pStage;
	i3UIFrameWnd *		m_pWnd;
	i3UIImageButton *	m_pCTRL_TImgBtn_Palette;

	i3List			m_BlockBtnList;

protected:
	void			_InitBlocks(void);

public:
	CPanel_Palette(void);
	virtual ~CPanel_Palette(void);

	void			Create( CStageMapEditor * pStage);

	void			Popup( REAL32 x, REAL32 y);

	virtual BOOL	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
};

#endif