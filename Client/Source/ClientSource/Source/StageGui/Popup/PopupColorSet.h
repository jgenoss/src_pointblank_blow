#ifndef __POPUP_COLOR_SET_H
#define __POPUP_COLOR_SET_H

#include "PopupBase.h"

class i3GuiRoot;
class CStageMapEditor;

class CPopupColorSet : public CPopupBase
{
	I3_CLASS_DEFINE(CPopupColorSet);

private:
	CStageMapEditor *	m_pStage;
	i3GuiRoot *			m_pRoot;

	i3GuiStatic*		m_pColorSetWin;

	i3GuiEditBox*		m_pColorSetCaption;
	i3GuiStatic*		m_pColorSetColor;
	i3GuiStatic*		m_pColorSetSelectedColor;
	i3GuiStatic*		m_pColorSetBrightColorSet;
	i3GuiEditBox*		m_pColorSetRGBEditBox;
	
	i3GuiButton*		m_pColorSetOK;
	i3GuiButton*		m_pColorSetCancel;	

	COLORREAL			m_SelectedColor;

public:
	CPopupColorSet(void);
	virtual ~CPopupColorSet(void);

	void			CreateColorSet(CStageMapEditor * pStage, i3GuiRoot * pRoot);
	
	
	virtual void	OnUpdate(RT_REAL32 rDeltaSeconds);
	
	virtual void	OnExitKey(void);

	virtual void	OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	virtual BOOL	OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

	virtual void	OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify) {};
	virtual BOOL	OnLinkControl(i3BinList * plist, i3GameNode * pControl) { return TRUE; }
	virtual void	OnPreGuiNotify(void) {};
};

#endif	//	__POPUP_COLOR_SET_H