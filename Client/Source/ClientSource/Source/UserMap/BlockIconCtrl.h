#if !defined( __GUI_BLOCK_ICON_CTRL_H)
#define __GUI_BLOCK_ICON_CTRL_H

class CBlockIconCtrl : public i3GuiStatic
{
	I3_CLASS_DEFINE( CBlockIconCtrl);
protected:
	VEC2D	m_UV;

public:
	CBlockIconCtrl(void);
	virtual ~CBlockIconCtrl(void);

	void			SetUV( VEC2D * pVec);

	virtual INT32	OnGuiEvent( i3GUI::EVENT event, UINT32 param1, UINT32 param2);
};

#endif
